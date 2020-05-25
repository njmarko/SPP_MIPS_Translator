#pragma once
#include "SymbolTable.h"
#include "IR.h"
#include <map>

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
	* First phase of ResourceAllocation: Function that builds an interference graph from the given instructions
	* @param reference to the interference graph
	* @param reference to the list of instruction pointers
	* @param reference to the list of register variable pointers
	* @returns reference to the updated interference graph
	*/
	InterferenceGraph& buildInterferenceGraph(InterferenceGraph& ig, Instructions& instr, Variables& vars);

	/*
	* Second phase of ResourceAllocation: Function that builds the simplification stack
	* @param reference to the interference graph
	* @param reference to the list of instruction pointers
	* @param reference to the list of register variable pointers
	* @returns reference to the updated interference graph
	* @throws runtime_error if the spill happens because the simplification stack could not be filled
	*/
	SimplificationStack& performSimplification(InterferenceGraph& ig,SimplificationStack& sims, Variables& vars);
};