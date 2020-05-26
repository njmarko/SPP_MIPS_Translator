#pragma once

#include "SymbolTable.h"
#include <iostream>
#include <algorithm>

/*
* Class that inherits abstract class Visitor and implements pure virtual method visit.
* It does liveness analysis on the data contained in the Symbol Table.
* It's goal is to fill in and out sets in instructions.
*/
class LivenessAnalysis: public Visitor{
public:
	/*
	* Implements the pure virtual method inherited from the Visitor parent class.
	* It starts the liveness analysis.
	* @param symbol table with all the data
	*/
	void visit(SymbolTable& symTab)override;

	/*
	* Function that performs liveness analysis on the instructions
	* and fills their in and out sets
	* @param reference to a list of instruction pointers
	*/
	void doLivenessAnalysis(Instructions& instr);
};