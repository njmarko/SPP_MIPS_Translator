#ifndef __IR__
#define __IR__

#include "Types.h"
#include <string>

/**
 * This class represents one variable from program code.
 */
class Variable
{
public:
	/*
	* Supported types of variables for MAVN
	*/
	enum VariableType
	{
		/// Memory variable
		MEM_VAR,
		/// Register variable
		REG_VAR,
		/// Variable with no asigned type
		NO_TYPE	
	};
	/*
	* Default constructor that just initializes the atributes
	*/
	Variable() : m_type(NO_TYPE), m_name(""), m_position(-1), m_assignment(no_assign) {}
	/*
	* Constructor for creating a variable with name and position
	* @param name of the variable
	* @param position in the interference matrix
	*/
	Variable(std::string name, int pos) : m_type(NO_TYPE), m_name(name), m_position(pos), m_assignment(no_assign) {}
	/*
	* Constructor for creating a variable with name, position and type
	* Ususally used for variables of type REG_VAR
	* @param name of the variable
	* @param position in the interference matrix
	* @param type of the variable
	*/
	Variable(std::string name, int pos, VariableType type) : m_type(type), m_name(name), m_position(pos), m_assignment(no_assign) {}
	/*
	* Constructor for creating a variable with name, position and type
	* Used for variable of type MEM_VAR
	* @param name of the variable
	* @param position in the interference matrix
	* @param type of the variable
	* @param value of the MEM variable
	*/
	Variable(std::string name, int pos, VariableType type, std::string val) 
		: m_type(type), m_name(name), m_position(pos), m_assignment(no_assign),m_value(std::stoi(val)) {}

	/*
	* Destructor for variable class
	*/
	~Variable() {};

	/*
	* Function that prints the name of the variable
	* @param reference to the output stream
	* @param const reference to the variable that is intended for printing
	* @returns reference to the output stream that enables successive printing
	*/
	friend std::ostream& operator<<(std::ostream& out, const Variable& v);

	/*
	* Setter for the name of the variable
	* @param name of the variable
	*/
	void set_name(std::string name);

	/*
	* Setter for the type of the variable
	* @param type of the variable is either MEM_VAR,REG_VAR or NO_TYPE
	*/
	void set_type(VariableType type);

	/*
	* Setter for the position of the variable in the interference matrix
	* @param position of the variable
	*/
	void set_pos(int pos);

	/*
	* Setter for the register assigned for the variable
	* @param name of the register from Regs enumeration
	*/
	void set_assignment(Regs reg);

	/*
	* Getter for variable type
	* @returns type of the variable that can be MEM_VAR, REG_VAR or NO_TYPE
	*/
	Variable::VariableType getType();

	/*
	* Getter for variable name
	* @returns name of the variable
	*/
	std::string getName();

	/*
	* Getter for variable position in the interference matrix
	* @returns position in the interference matrix
	*/
	int getPos();

	/*
	* Getter for register asigned to the variable
	* @returns register that was assigned to the variable
	*/
	Regs getAsignment();

	/*
	* Getter for value of the MEM variable
	* @returns value that the MEM holds
	*/
	int getValue();


private:
	/*
	* Type of the variable that can be MEM_VAR, REG_VAR or NO_TYPE
	*/
	VariableType m_type;

	/*
	* Name of the variable
	*/
	std::string m_name;

	/*
	* Position in interference matrix
	*/
	int m_position; 

	/*
	* Register that was assigned
	*/
	Regs m_assignment;

	/*
	* Value assigned to _mem variable
	*/
	int m_value; 
};

/*
* Converts enum variable type to string
* @param variable type
* @returns string representation of the whole variable type
*/
std::string varTypeToStr(Variable::VariableType type);


/**
 * This type represents list of variables from program code.
 */
typedef std::list<Variable*> Variables;


/**
 * This class represents one instruction in program code.
 */
class Instruction
{
public:
	/*
	* Default constructior for the instruction
	*/
	Instruction () : m_position(0), m_type(I_NO_TYPE) {}

	/*
	* Constructor with parameters that is used for creating non branching instructions
	* @param position of the instruction
	* @param type of the instruction that MAVN suports
	* @param list of destination variables
	* @param list of source variables
	* @param name of the parent label where this instruction belongs
	*/
	Instruction (int pos, InstructionType type, Variables& dst, Variables& src,const std::string& parent_lab) :
		m_position(pos), m_type(type), m_dst(dst), m_src(src),parent_label(parent_lab) {}

	/*
	* Constructor with parameters that is used for creating non branching instructions
	* @param position of the instruction
	* @param type of the instruction that MAVN suports
	* @param list of destination variables
	* @param list of source variables
	* @param name of the parent label where this instruction belongs
	* @param number that can appear in certain types of instructions
	*/
	Instruction(int pos, InstructionType type, Variables& dst, Variables& src, const std::string& parent_lab, int num) :
		m_position(pos), m_type(type), m_dst(dst), m_src(src), parent_label(parent_lab),numValue(num) {}


	/*
	* Constructor with parameters that is used for creating branching instructions
	* @param position of the instruction
	* @param type of the instruction that MAVN suports
	* @param list of destination variables
	* @param list of source variables
	* @param name of the label where this instruction jumps
	* @param name of the parent label where this instruction belongs
	*/
	Instruction(int pos, InstructionType type, Variables& dst, Variables& src,
		const std::string& j_label, const std::string& parent_lab) :
		m_position(pos), m_type(type), m_dst(dst), m_src(src),jump_label(j_label),parent_label(parent_lab){}

	/*
	* Destructor for the instruction
	* Doesn't destroy items in any of the list it contains
	* because items in those lists can exist in other places
	*/
	~Instruction();

	/*
	* Getter for the optional member numValue
	*/
	int getNumValue();

	/*
	* Getter for the optional member jumpLabel
	* @returns name of the label this instructions could jump to
	*/
	std::string getJumpLabel();

	/*
	* Getter for the position asigned to the instruction
	* @returns position of the instruction
	*/
	int getPos();

	/*
	* Getter for the instruction type
	* @returns position the type of instruction
	*/
	InstructionType getType();


	/*
	* Used for adding a successor instruction
	* @param poinster to the successor instruction
	*/
	void addSucc(Instruction* instr);


	/*
	* Used for adding a predecesor instruction
	* @param poinster to the predecesor instruction
	*/
	void addPred(Instruction* instr);
private:
	/*
	* Position of the Instruction
	*/
	int m_position;

	/*
	* Type of instruction suppored by MAVN
	*/
	InstructionType m_type;

	/*
	* Optional: Name of the label where the instruction should jump
	*/
	std::string jump_label; 

	/*
	* Label where this instruction belongs
	* instruction can have no parent label
	* Used when creating MIPS output code
	*/
	std::string parent_label;

	/*
	* Optional: Number before the parenthesis in the lw r1, 0(r4); type of instructions
	* or Number in instructions like addi rid,rid,num;
	*/
	int numValue;

	/*
	* Destination set
	*/
	Variables m_dst;

	/*
	* Source set
	*/
	Variables m_src;

	/*
	* Use set
	*/
	Variables m_use;

	/*
	* Defines set
	*/
	Variables m_def;

	/*
	* In set
	*/
	Variables m_in;

	/*
	* Out set
	*/
	Variables m_out;

	/*
	* List of succesor instructions
	*/
	std::list<Instruction*> m_succ;
	
	/*
	* List of predecesor instructions
	*/
	std::list<Instruction*> m_pred;
};


/**
 * This type represents a list of instructions from program code.
 */
typedef std::list<Instruction*> Instructions;


#endif
