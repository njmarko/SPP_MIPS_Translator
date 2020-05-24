#pragma once

#include "SymbolTable.h"
#include <iostream>
#include <algorithm>

class LivenessAnalysis: public Visitor{
public:
	void visit(SymbolTable& symTab)override;
	void doLivenessAnalysis(Instructions& instr);
};