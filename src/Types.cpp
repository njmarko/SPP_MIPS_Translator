#include "Types.h"

std::string instrTypeToWholeInstrStr(InstructionType type)
{
	switch (type)
	{
	case I_NO_TYPE:		return "No Type";
	case I_ADD:			return "add rid, rid, rid";
	case I_ADDI:		return "addi rid, rid, num";
	case I_SUB:			return "sub rid, rid, rid";
	case I_LA:			return "la rid, mid";
	case I_LI:			return "li rid, num";
	case I_LW:			return "lw rid, num(rid)";
	case I_SW:			return "sw rid, num(rid)";
	case I_BLTZ:		return "bltz rid, id";
	case I_B:			return "b id";
	case I_NOP:			return "nop";
	case I_MUL:			return "mul rid, rid, rid";
	case I_XOR:			return "xor rid, rid, rid";
	case I_NEG:			return "neg rid, rid";
	case I_BGEZ:		return "bgez rid, id";
	default:			return "";
	}
}

std::string instrTypeStr(InstructionType type)
{
	switch (type)
	{
	case I_NO_TYPE:		return "No Type";
	case I_ADD:			return "add";
	case I_ADDI:		return "addi";
	case I_SUB:			return "sub";
	case I_LA:			return "la";
	case I_LI:			return "li";
	case I_LW:			return "lw";
	case I_SW:			return "sw";
	case I_BLTZ:		return "bltz";
	case I_B:			return "b";
	case I_NOP:			return "nop";
	case I_MUL:			return "mul";
	case I_XOR:			return "xor";
	case I_NEG:			return "neg";
	case I_BGEZ:		return "bgez";
	default:			return "";
	}
}
