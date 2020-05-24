#include "Types.h"

std::string instrTypeToStr(InstructionType type)
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
	default:			return "";
	}
}
