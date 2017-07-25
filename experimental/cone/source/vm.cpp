
#include <stdint.h>

// experimental vm:

#define vmInstructionStart() /**/
#define vmInstructionEnd() goto lVmNextOpcode;

enum vmOpcodeEnum : uint32_t
{
	kOpcodeNop,
	kOpcodeEoP, // End of Program
};

struct vmInstruction
{
	vmOpcodeEnum opcode;
	uint16_t register_a;
	uint16_t register_b;
};

void run ()
{
	int pc = 0;

	// set up basic program of nops:
	vmInstruction instructionArray [4] = {
		kOpcodeNop, 0,0,
		kOpcodeNop, 0,0,
		kOpcodeNop, 0,0,
		kOpcodeEoP, 0,0, };

	// Start off w/ the first instruction:
	vmInstruction* instr = &instructionArray[pc];
	instr->opcode = instructionArray[pc].opcode;
	goto lVmPerformOpcode;


	// Increment program counter:
lVmNextOpcode:
	// Increment program counter and get that next instruction.
	instr = &instructionArray[pc++];
	// Go to perform the instruction
	goto lVmPerformOpcode;


	// Opcode perform:
lVmPerformOpcode:
	// Lookup opcode in the table~
	switch (instr->opcode)
	{
		case kOpcodeNop:	goto lNop;
		case kOpcodeEoP:	goto lEoP;
		default:			goto lVmNextOpcode;
	}


	// Opcodes:

lNop:
	vmInstructionStart();
	// Nothing happens in a no-op.
	vmInstructionEnd();

lVmEarlyDeath:
	// Print error message here. Falls through to EoP below.
lEoP:
	vmInstructionStart();
	return; // Has no end. EoP is End of Program.
}

#undef vmInstructionEnd