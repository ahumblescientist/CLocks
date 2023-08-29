#include "vm.h"
#include "compiler.h"

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

InterpretResult run() {
	uint8_t instruction;
#define BINARY_OP(o)\
	do { \
		Value y = pop();\
		Value x = pop();\
		push(x o y);\
	} while(0)


	while(1) {
		#ifdef DEBUG_ENABLED
			for(Value *v = vm.stack;v<vm.sp;v++) {
				printf("%04lX [ ", v - vm.stack);
				printValue(*v);
				printf(" ]\n");
			}
			printf("\n");
			size_t throwaway = vm.pc - vm.chunk->code;
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
			case OP_NEGATE:
					*(vm.sp-1) = -(*(vm.sp-1));
					break;
			case OP_ADD: BINARY_OP(+); break;
			case OP_SUBTRACT: BINARY_OP(-); break;
			case OP_MULTIPLY: BINARY_OP(*); break;
			case OP_DIVIDE: BINARY_OP(/); break;
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
	compile(source);
	return run();
}

