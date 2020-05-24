#include "LivenessAnalysis.h"

void LivenessAnalysis::visit(SymbolTable & symTab)
{
	doLivenessAnalysis(symTab.getInstructions());
}

void LivenessAnalysis::doLivenessAnalysis(Instructions & instr)
{



}
