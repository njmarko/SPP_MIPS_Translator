#pragma once

#include "IR.h"
#include <map>
#include <list>
#include <string>
#include <regex>
#include "Types.h"
#include <vector>


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
	* Default constructor
	*/
	SymbolTable();

	/*
	* Destructor that deletes instructions,memVariables and regVariables
	*/
	~SymbolTable();

	/*
	* Function that fills successor and predecessor sets
	* @throws runtime_error (Semantic) if the lable that is being jump to doesn't exist 
	*/
	void connectInstructions();

	/*
	* Gets the first instruction encountered after jump to the label
	* @param name of the label
	* @returns pointer to the first instruction encountered after the jump to the label
	*/
	Instruction* getInstrFromLabel(const std::string& label);

	/*
	* Gets the last declared label
	* @returns name of the last declared label, or empty string "" if there are none
	*/
	std::string getParentLabel();

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
	* Method for creating a new instructions
	* @param const reference to a list of strings that constans parameter variables
	* @param instruction type
	* @throws runtime_error with informations related to the type of instruction and variable
	*/
	void makeInstruction(const std::list<std::string>& params, InstructionType type);

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


	/*
	* Creates the error message for semantic instruction errors
	* @param position of the parameter that is insvalid (can be 0,1,2)
	* @param type of the variable that is invalid
	* @param instruction type needed for printing the instruction format
	* @param name of the invalid variable
	* @returns string representation of the error message that can be used as an runtime_error message
	*/
	std::string makeInstructionErrorMsg(int argPos,Variable::VariableType varType, InstructionType i_type,const std::string& faultyParam);
private:

	/*
	* Counter for instructions that is incremented whenever new instruction is added
	*/
	int instrCount;

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

