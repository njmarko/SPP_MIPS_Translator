#pragma once

#include "IR.h"
#include <map>
#include <list>
#include <string>

/*
* Key is the label name, and int is the position in the code
*/
typedef std::map<std::string, Instructions> Labels;


/*
* Key is the function name, and int is the position in the code
*/
typedef std::map<std::string, int> Functions;

/*
* Class that holds information about instructions, variables, functions
*/
class SymbolTable {
public:

	/*
	* Getter for the instructions
	* @returns list of instruction pointers
	*/
	Instructions& getInstructions();

	/*
	* Getter for the variables
	* @returns list of variable pointers
	*/
	Variables& getVariables();

	/*
	* Getter for the labels
	* @returns map<string,int> of labels that hold their positions in the code
	*/
	Labels& getLabels();

	/*
	* Getter for the functions
	* @returns map<string,int> of functions that hold their positions in the code
	*/
	Functions& getFunctions();

	/*
	* Method for adding a new instructions
	* @param instruction pointer
	*/
	void addInstruction(Instruction* instr);

	/*
	* Method for adding a new variable
	* @param variable pointer
	*/
	void addVariable(Variable* var);

	/*
	* Method for adding a new label
	* @param label name 
	* @param label position
	*/
	void addLabel(std::string lab_name,int pos);

	/*
	* Method for adding a new function
	* @param function name
	* @param funciton pos
	*/
	void addFunction(std::string func_name,int pos);
private:
	Instructions instructions;
	Variables variables;
	Labels labels;
	Functions functions;
};

