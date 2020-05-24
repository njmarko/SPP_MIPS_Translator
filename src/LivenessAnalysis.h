#pragma once

#include "SymbolTable.h"
#include <iostream>

class LivenessAnalysis: public Visitor{
public:
	void visit(SymbolTable& symTab)override;
	void doLivenessAnalysis(Instructions& instr);
};