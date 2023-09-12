#include "vm.h"
#include "compiler.h"
#include <stdarg.h>

VM vm;

#define DEBUG_ENABLED

void resetStack() {
	vm.sp = (&vm.stack[0]);
}

void initVM() {
	resetStack();
}

void freeVM() {
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

InterpretResult run() {
	uint8_t instruction;
#define BINARY_OP(o)\
	do { \
		if(!(isNumber(peek(0)) && isNumber(peek(1)))) { runtimeError("Operand must be a number"); return INTERPRET_RUNTIME_ERROR; }\
		double y = getNumber(pop());\
		double x = getNumber(pop());\
				push(makeNumber(x o y));\
	} while(0)


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
			case OP_ADD: BINARY_OP(+); break;
			case OP_SUBTRACT: BINARY_OP(-); break;
			case OP_MULTIPLY: BINARY_OP(*); break;
			case OP_DIVIDE: BINARY_OP(/); break;
			case OP_TRUE: push(makeBool(0xFF)); break;
			case OP_FALSE: push(makeBool(0x00)); break;
			case OP_NIL: push(makeNil()); break;
			case OP_NOT: NOT(); break;
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

