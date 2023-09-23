#include "vm.h"
#include "compiler.h"
#include "obj.h"
#include <stdarg.h>
#include <string.h>

#define DEBUG_ENABLED

VM vm;

void resetStack() {
	vm.sp = (&vm.stack[0]);
}

void initVM() {
	resetStack();
}

void freeVM() {
	freeObjects();
}

void push(Value value) {
	*vm.sp = value;
	vm.sp++;
}

Value pop() {
	vm.sp--;
	return *vm.sp;
}

Value peek(size_t n) {
	return *(vm.sp - n - 1);
}

void runtimeError(char *m, ...) {
	size_t line = vm.chunk->lines[vm.pc - 1 - vm.chunk->code];
	printf("line %zu \n", line);
	va_list args;
	va_start(args, m);
	vprintf(m, args);
	va_end(args);
	printf("\n");
	resetStack();
}

uint8_t readByte() {
	return *(vm.pc++);
}

Value readConstant() {
	return vm.chunk->constants.values[readByte()];
}

Value readConstantLong() {
	uint8_t lo = readByte();
	uint8_t hi = readByte();
	uint16_t addr = (hi << 8) | lo;
	return vm.chunk->constants.values[addr];
}

void NOT() {
	if(!isBool(peek(0)) && !isNil(peek(0))) {
		runtimeError("Operand must be of the type boolean or nil");
	}
	if(isNil(peek(0))) {
		pop();
		push(makeBool(0xFF));
		return;
	}
	push(makeBool(getBool(pop()) ? 0x00 : 0xFF));
}

Value objEqual(Value a, Value b) {
	if(objType(a) != objType(b)) return makeBool(0x00);
	switch(objType(a)) {
		case OBJ_STRING: {
			if(getString(a)->length != getString(b)->length) return makeBool(0x00);
			if(memcmp(getCString(a), getCString(b), getString(a)->length)) return makeBool(0x00);
			break;
		}
		default: break;
	}
	return makeBool(0xFF);
}

void valuesEqual(Value a, Value b) {
	if(a.type != b.type) push(makeBool(0x00));
	switch(a.type) {
		case VALUE_NUMBER: push(makeBool(getNumber(a) == getNumber(b))); break;
		case VALUE_BOOL: push(makeBool(getNumber(a) == getNumber(b))); break;
		case VALUE_NIL: push(makeBool(0xFF)); break;
		case VALUE_OBJ: push(objEqual(a, b)); break;
		default: break;
	}
}

void addStrings() {
	Value b = (pop());
	Value a = (pop());
	size_t length = getString(a)->length + getString(b)->length; 
	char *s1 = getCString(a);
	char *s2 = getCString(b);
	size_t l1 = getString(a)->length;
	size_t l2 = getString(b)->length;
	char *temp = ALLOCATE(char, length + 1);
	for(size_t i=0;i<l1;i++) {
		temp[i] = s1[i];
	}
	for(size_t i=0;i<l2;i++) {
		temp[l1+i] = s2[i];
	}
	temp[length] = '\0';
	push(makeObj((Obj *)takeString(temp, length)));
}

InterpretResult run() {
	uint8_t instruction;
#define BINARY_OP(o)\
	do { \
		if(!(isNumber(peek(0)) && isNumber(peek(1)))) { runtimeError("Operand must be a number"); return INTERPRET_RUNTIME_ERROR; }\
		double y = getNumber(pop());\
		double x = getNumber(pop());\
				push(makeNumber(x o y));\
	} while(0)

#define BOOL_OP(o)\
	do {\
		if(!(isNumber(peek(0)) && isNumber(peek(1)))) { runtimeError("Operand must be a number"); return INTERPRET_RUNTIME_ERROR; }\
		double y = getNumber(pop());\
		double x = getNumber(pop());\
				push(makeBool(x o y));\
	}while(0)

	while(1) {
		#ifdef DEBUG_ENABLED
			for(Value *v = vm.stack;v<vm.sp;v++) {
				printf("%04lX [ ", v - vm.stack);
				printValue(*v);
				printf(" ]\n");
			}
			printf("\n");
		#endif
		instruction = readByte();
		switch((OPCODE)instruction) {
			case OP_CONSTANT: {
				Value constant = readConstant();
				push(constant);
				break;
			}
			case OP_CONSTANT_LONG: { 
				Value constant = readConstantLong();
				push(constant);
				break;
			}
			case OP_NEGATE: {
					if(!isNumber(*(vm.sp))) {
						runtimeError("Operand must be of the Type number");
						return INTERPRET_RUNTIME_ERROR;
					}
					*(vm.sp - 1) = makeNumber(-getNumber(*(vm.sp - 1)));
					break;
											}
			case OP_ADD: {
				Value a = peek(0);
				Value b = peek(1);
				if(isString(a) && isString(b)) {
					addStrings();
				} else if(isNumber(a) && isNumber(b)) {
					pop();
					pop();
					push(makeNumber(getNumber(a) + getNumber(b)));
				} else {
					runtimeError("Operands must be of the type number or string");
				}
				break;
			}
			case OP_SUBTRACT: BINARY_OP(-); break;
			case OP_MULTIPLY: BINARY_OP(*); break;
			case OP_DIVIDE: BINARY_OP(/); break;
			case OP_TRUE: push(makeBool(0xFF)); break;
			case OP_FALSE: push(makeBool(0x00)); break;
			case OP_NIL: push(makeNil()); break;
			case OP_NOT: NOT(); break;
			case OP_EQUAL: {
				Value a = pop();
				Value b = pop();
				valuesEqual(a, b);
				break;
										 }
			case OP_GREATER: BOOL_OP(>); break;
			case OP_LESS: BOOL_OP(<); break;
			case OP_RETURN:
				goto END;
				break;
		}
	}
END:
	return INTERPRET_OK;
#undef BINARY_OP
}

InterpretResult interpret(char *source) {
	Chunk chunk;
	initChunk(&chunk);
	uint8_t compileResult = compile(source, &chunk);
	if(!compileResult) {
		freeChunk(&chunk);
		return INTERPRET_COMPILE_ERROR;
	}
	vm.chunk = &chunk;
	vm.pc = chunk.code;
	InterpretResult runResult = run();
	freeChunk(&chunk);
	return runResult;
}

