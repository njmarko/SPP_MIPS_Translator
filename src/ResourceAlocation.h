#pragma once
#include "SymbolTable.h"
#include "IR.h"

/*
* Class that will do the resource allocation
* There are four phases to resource allocation:
* Build, Simplify, Spill and Select
* 
*/
class ResourceAllocation :public Visitor {

	/*
	* Visitor class has pure virtual function visit that has to be implemented.
	* Function visit is called by the class that this Visitor visits
	*/
	void visit(SymbolTable& symTab)override;

	/*
	* Function that builds an interference graph fro mthe given instructions
	* @param reference to the interference graph
	* @param reference to the list of instruction pointers
	* @param reference to the list of variable poinsters
	* @returns reference to the updated interference graph
	*/
	InterferenceGraph& buildInterferenceGraph(InterferenceGraph& ig, Instructions& instr, Variables& vars);
};