#pragma once

#include "IR.h"
#include <map>
#include <list>
#include <string>
#include <regex>


/*
* List of paris for label and function names, 
* and int is the position of the  first instruction in the code
* that belongs to the label
*/
typedef std::list<std::pair<std::string, int>> Labels;


/*
* List of function names
*/
typedef std::list<std::string> Functions;

/*
* Class that holds various information about instructions, variables, functions
*/
class SymbolTable {
public:

	/*
	* Destructor that deletes instructions,memVariables and regVariables
	*/
	~SymbolTable();

	/*
	* Checks if memVariables list contains the variable
	* @param name of the mem variable
	* @returns if the mem variable exists, returns it, otherwise returns NULL
	*/
	Variable* isMemVarDefined(const std::string& name);


	/*
	* Checks if regVariables list contains the variable
	* @param name of the reg variable
	* @returns if the reg variable exists, returns it, otherwise returns NULL
	*/
	Variable* isRegVarDefined(const std::string& name);

	/*
	* Checks if functions list contains the function
	* @param name of the function
	* @returns true if the function exists, otherwise false
	*/
	bool isFunctionDefined(const std::string& name);

	/*
	* Checks if functions list contains the function
	* @param name of the function
	* @returns true if the function exists, otherwise false
	*/
	bool isLabelDefined(const std::string& name);

	/*
	* Function for getting the number of MEM variables
	* @returns number of MEM variables
	*/
	int getNumMemVariables();

	/*
	* Function for getting the number of instructions
	* @returns number of instructions
	*/
	int getNumInstructions();

	// GETTERS AND SETTERS

	/*
	* Getter for the instructions
	* @returns list of instruction pointers
	*/
	Instructions& getInstructions();

	/*
	* Getter for the memory variables
	* @returns list of memory variable pointers
	*/
	Variables& getMemVariables();

	/*
	* Getter for the register variables
	* @returns list of register variable pointers
	*/
	Variables& getRegVariables();

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
	* Method for adding a new memory variable
	* @param name of the variable
	* @param type of the variable 
	* @param value associated with the mem variable
	* @throws runtime_exception if the mem variable is already defined
	* @throws runtime_exception if the name format is incorrect
	*/
	void addMemVariable(const std::string& name, Variable::VariableType type, const std::string& value);


	/*
	* Method for adding a new register variable
	* @param name of the variable
	* @param type of the variable
	* @param value associated with the reg variable
	* @throws runtime_exception if the reg variable is already defined
	* @throws runtime_exception if the name format is incorrect
	*/
	void addRegVariable(const std::string& name, Variable::VariableType type);

	/*
	* Method for adding a new label to the labels collections
	* @param label name
	* @throws runtime_exception if the variable or function with the same name already exists
	* @throws runtime_exception if the name format is incorrect
	*/
	void addLabel(const std::string& lab_name);

	/*
	* Method for adding a new function to the functions and labels collections
	* @param function name
	* @throws runtime_exception if the variable or function with the same name already exists
	* @throws runtime_exception if the name format is incorrect
	*/
	void addFunction(const std::string& func_name);
private:
	/*
	* List of instruction pointers that were created from the code
	*/
	Instructions instructions;

	/*
	* List of memory variable pointers that were gathered from the code
	*/
	Variables memVariables;

	/*
	* List of register variable pointers that were gathered from the code
	*/
	Variables regVariables;

	/*
	* List of pairs that contain name of the variable 
	* and the position of the first instruction
	*/
	Labels labels;

	/*
	* List of function names
	*/
	Functions functions;

};

