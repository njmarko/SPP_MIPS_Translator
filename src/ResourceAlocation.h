#pragma once
#include "SymbolTable.h"
#include "IR.h"
#include <map>
#include <set>
#include <string>

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
	* Funcion that is activated when the spill happens. 
	* It's role is to find the register variable with the most interferences and 
	* replace it with memory variable, and also add load instructions before every use of that variable
	* and store instructions after every declaration of that variable.
	* @param reference to the list of instruction pointers
	* @param reference to the list of register variable pointers
	* @param reference to the list of memory variable pointers
	* @param reference to the list of pairs of label and function names, and the first instructions that follow after them
	* @param reference to the interference graph
	*/
	void handleSpill(Instructions& instr, Variables& r_vars, Variables& m_vars, Labels& labels, InterferenceGraph& ig);

	/*
	* Helper function for the handleSpill function
	* First it chooses a nemae for the var that is free, then It creates a new memory variable.
	* @param reference to the list of memory variable pointers
	* @returns pointer to the new memory variable
	*/
	Variable* createNewMemVariable(Variables& m_vars);

	/*
	* Helper function for the handleSpill function
	* First it chooses a nemae for the var that is free, then It creates a new reg variable.
	* @param reference to the list of register variable pointers
	* @returns pointer to the new register variable
	*/
	Variable* createNewRegVariable(Variables& r_vars);

	/*
	* Function that creates a map for the register interferences from the interference matrix
	* Each key is the variable pointer, and the associated value is the number of interferences
	* for that variable
	* @param reference to the insterference graph
	* @param reference to a list of pointers of register variables
	*/
	std::map<Variable*, int> makeVarInterferenceMap(InterferenceGraph& ig, Variables& r_vars);


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
	* @param reference to the simplification stack that contains Variable pointers
	* @param reference to the list of register variable pointers
	* @returns reference to the updated interference graph
	* @throws runtime_error if the spill happens because the simplification stack could not be filled
	*/
	SimplificationStack& performSimplification(InterferenceGraph& ig,SimplificationStack& sims, Variables& vars);

	/*
	* Last phase of ResourceAllocation: Function that selects the registers for the variables
	* @param reference to the interference graph
	* @param reference to the simplification stack that contains Variable pointers
	* @throws runtime_error if the spill happens because there were not enough register left to assign
	*/
	void selectRegisters(InterferenceGraph& ig, SimplificationStack& ss);
};