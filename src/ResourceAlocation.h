/*
*	Author: Marko Njegomir sw-38-2018
*	Project: MAVN: Translates higher level 32bit MIPS assembly code into low level MIPS assembly code
*/
#pragma once
#include "SymbolTable.h"
#include "IR.h"
#include <map>
#include <set>
#include <string>
#include <algorithm>



/**
* An exception that is thrown when no more register variables can be spilled into the memory
* It is used to end the spilling loop
*/
struct NoMoreSpillsPossible :public std::runtime_error {
	/**
	* Constructor with a message that will be displayed
	* @param message that will be displayed
	*/
	NoMoreSpillsPossible(const char* message);
};

/**
* Class that will do the resource allocation
* There are four phases to resource allocation:
* Build, Simplify, Spill and Select
*/
class ResourceAllocation : public Visitor {
public:
	/**
	* Visitor class has pure virtual function visit that has to be implemented.
	* Function visit is called by the class that this Visitor visits
	* @param symTab class that is being visited
	*/
	void visit(SymbolTable& symTab) override;

	/**
	* Funcion that is activated when the spill happens. 
	* It's role is to find the register variable with the most interferences and 
	* replace it with memory variable, and also add load instructions before every use of that variable
	* and store instructions after every declaration of that variable.
	* @param instr reference to the list of instruction pointers
	* @param r_vars reference to the list of register variable pointers
	* @param m_vars reference to the list of memory variable pointers
	* @param labels reference to the list of pairs of label and function names, and the first instructions that follow after them
	* @param ig reference to the interference graph
	* @param spilled_vars reference to the list of variable pointers that were already spilled to the memory
	* @throw NoMoreSpillsPossible if all the register variables are already spilled to the memory
	*/
	void handleSpill(Instructions& instr, Variables& r_vars, Variables& m_vars, Labels& labels, InterferenceGraph& ig, Variables& spilled_vars);


	/**
	* Decomposes the instructions that contain the variable that is being replaced 
	*	example: Instruction sub r1,r2,r3 can be decomposed into
	*	 neg r1,r3; add r1,r1,r2;
	*	 And if the register r2 can be replaced with immediate value then the resulting instructions are
	*	 neg r1,r3; addi r1,r1,immediate_val
	*	 That reduces the number of live variables.
	* @param instr reference to a list of instruction pointers
	* @param replaced_var pointer to a variable that is being spilled
	*/
	void decomposeInstructions(Instructions& instr, Variable* replaced_var);

	/**
	* Checks if the memory variables value is changed in the program
	* and if the value is not changed (no la and sw instruction combination that writes to it)
	* then it tries to replace lw instructions with li instruction and also tries to replace one source parameter
	* in some instructions with immediate value (example: add instruction is changed into addi)
	* @param instr reference to a list of instruction pointers
	* @param m_vars reference to a list of memory variables
	*/
	void moveFromMemToImmediate(Instructions& instr,Variables& m_vars);

	/**
	* Helper function for the handleSpill function
	* First it chooses a nemae for the var that is free, then It creates a new memory variable.
	* @param m_vars reference to the list of memory variable pointers
	* @returns pointer to the new memory variable
	*/
	Variable* createNewMemVariable(Variables& m_vars);

	/**
	* Helper function for the handleSpill function
	* First it chooses a nemae for the var that is free, then It creates a new reg variable.
	* @param r_vars reference to the list of register variable pointers
	* @returns pointer to the new register variable
	*/
	Variable* createNewRegVariable(Variables& r_vars);

	/**
	* Function that creates a map for the register interferences from the interference matrix
	* Each key is the variable pointer, and the associated value is the number of interferences
	* for that variable
	* @param ig reference to the insterference graph
	* @param r_vars reference to a list of pointers of register variables
	*/
	std::map<Variable*, int> makeVarInterferenceMap(InterferenceGraph& ig, Variables& r_vars);


	/**
	* First phase of ResourceAllocation: Function that builds an interference graph from the given instructions
	* @param ig reference to the interference graph
	* @param instr reference to the list of instruction pointers
	* @param vars reference to the list of register variable pointers
	* @returns reference to the updated interference graph
	*/
	InterferenceGraph& buildInterferenceGraph(InterferenceGraph& ig, Instructions& instr, Variables& vars);

	/**
	* Second phase of ResourceAllocation: Function that builds the simplification stack
	* @param ig reference to the interference graph
	* @param sims reference to the simplification stack that contains Variable pointers
	* @param vars reference to the list of register variable pointers
	* @returns reference to the updated interference graph
	* @throw runtime_error if the spill happens because the simplification stack could not be filled
	*/
	SimplificationStack& performSimplification(InterferenceGraph& ig,SimplificationStack& sims, Variables& vars);

	/**
	* Last phase of ResourceAllocation: Function that selects the registers for the variables
	* @param ig reference to the interference graph
	* @param ss reference to the simplification stack that contains Variable pointers
	* @throw runtime_error if the spill happens because there were not enough register left to assign
	*/
	void selectRegisters(InterferenceGraph& ig, SimplificationStack& ss);
};


