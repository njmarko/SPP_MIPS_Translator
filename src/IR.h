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
	enum VariableType
	{
		MEM_VAR,
		REG_VAR,
		NO_TYPE
	};

	Variable() : m_type(NO_TYPE), m_name(""), m_position(-1), m_assignment(no_assign) {}
	Variable(std::string name, int pos) : m_type(NO_TYPE), m_name(name), m_position(pos), m_assignment(no_assign) {}
	Variable(std::string name, int pos, VariableType type) : m_type(type), m_name(name), m_position(pos), m_assignment(no_assign) {}
	Variable(std::string name, int pos, VariableType type, std::string val) : m_type(type), m_name(name), m_position(pos), m_assignment(no_assign),m_value(val) {}

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
	* Setter for the position of the variable
	* @param position of the variable
	*/
	void set_pos(int pos);

	/*
	* Setter for the register assigned for the variable
	* @param name of the register from Regs enumeration
	*/
	void set_assignment(Regs reg);

private:
	VariableType m_type;
	std::string m_name;
	int m_position;
	Regs m_assignment;
	std::string m_value; // value assigned to _mem variable
};


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
	Instruction () : m_position(0), m_type(I_NO_TYPE) {}
	Instruction (int pos, InstructionType type, Variables& dst, Variables& src) :
		m_position(pos), m_type(type), m_dst(dst), m_src(src) {}

private:
	int m_position;
	InstructionType m_type;
	
	Variables m_dst;
	Variables m_src;

	Variables m_use;
	Variables m_def;
	Variables m_in;
	Variables m_out;
	std::list<Instruction*> m_succ;
	std::list<Instruction*> m_pred;
};


/**
 * This type represents list of instructions from program code.
 */
typedef std::list<Instruction*> Instructions;

#endif
