#pragma once

#include "IR.h"
#include <map>
#include <list>
#include <string>
#include <regex>
#include "Types.h"
#include <vector>

/*
* Forward declaration
*/
class SymbolTable;

/*
* Absract class that is used for implementing Visitor pattern
* It forces the class to inherit the accept method that must be redefined
*/
class ProgramData {
public:
	/*
	* Pure virtual function that accepts the visitor and should call his visit function
	* @param reference to the class that inherits the Visitor class
	*/
	virtual void accept(class Visitor& v) = 0;
};

/*
* Absract class that is used for implementing Visitor pattern
* It forces the class to inherit the visit method that must be redefined
*/
class Visitor {
public:
	/*
	* Pure virtual function that allows the visiting class to operate on the host class
	* @param reference to the SymbolTable class that will be the host for this visitor
	*/
	virtual void visit(SymbolTable& data) = 0;
};


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
class SymbolTable :public ProgramData{
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
	* Overwrited pure virtual function accept that is used for implementing Visitor pattern.
	* This function will call visitors visit function.
	* @param reference to the class that inherits the visitor abstract class
	*/
	void accept(Visitor& v) override;


	/*
	* Function that fills successor and predecessor sets
	* @throws runtime_error (Semantic) if the lable that is being jump to doesn't exist 
	*/
	void connectInstructions();


	/*
	* Reset the instruction sets that are filled in instruction connection, liveness analysis and
	* resource allocation phases. Also resets the Interference graph and variable assignments
	*/
	void resetData();


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
	* Getter for the list that holds label and function names and positions of their first instructions
	* @returns list of pairs<string,int> of labels that hold their positions in the code
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
	* Method for adding a new label or a function to the labels collections
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
	* Method for adding a pointer to a variable that is selected to be spilled into memory
	* into a list of variable pointers
	* @param pointer to a variable that is being spilled into memory
	*/
	void addSpilledVar(Variable * v);

	/*
	* Creates the error message for semantic instruction errors
	* @param position of the parameter that is insvalid (can be 0,1,2)
	* @param type of the variable that is invalid
	* @param instruction type needed for printing the instruction format
	* @param name of the invalid variable
	* @returns string representation of the error message that can be used as an runtime_error message
	*/
	std::string makeInstructionErrorMsg(int argPos,Variable::VariableType varType, InstructionType i_type,const std::string& faultyParam);

	/*
	* Used for printing out all the instrucitons contained in class SymbolTable
	*/
	void printInstructions();

	/*
	* Used for printing out all the instrucitons
	* @param cosnt reference to a List of instruction pointers
	*/
	void printInstructions(const Instructions& instrs);

	/*
	* Getter for interference graph
	* @returns reference to the interference graph
	*/
	InterferenceGraph& getInterferenceGraph();

	/*
	* Getter for simplification stack
	* @returns reference to the simplification stack
	*/
	SimplificationStack& getSimplificationStack();

	/*
	* Getter for spilled variables list
	* @returns reference to the spilled variables list of variable pointers
	*/
	Variables& getSpilledVars();
private:

	/*
	* Counter for instructions that is incremented whenever new instruction is added
	*/
	int instrCount;

	/*
	* Counter for register variables that is incremented whenever new register is added
	* Used for assigning names to the reg variables
	*/
	int regVarsCount;

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
	* List of pairs that contain name of the label or a function
	* and the position of the first instruction
	*/
	Labels labels;

	/*
	* List of function names
	*/
	Functions functions;

	/*
	* Interference graph used for resource allocation
	* It contains a matrix with interferences
	*/
	InterferenceGraph ig;

	/*
	* Simplification stack used for resource allocation
	* It constains register variable pointers
	*/
	SimplificationStack ss;

	/*
	* List of variable pointer that were spilled to memory as well as variables that 
	* were added in the process of spilling because they should not be spilled.
	* This list is needed to avoid infinite loops if the same variable is selected
	* over and over again.
	*/
	Variables spilledVars;

};

