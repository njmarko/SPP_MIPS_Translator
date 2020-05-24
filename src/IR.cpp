#include "IR.h"

void Variable::set_name(std::string name)
{
	m_name = name;
}

void Variable::set_type(VariableType type)
{
	m_type = type;
}

void Variable::set_pos(int pos)
{
	m_position = pos;
}

void Variable::set_assignment(Regs reg)
{
	m_assignment = reg;
}

Variable::VariableType Variable::getType()
{
	return m_type;
}

std::string Variable::getName()
{
	return m_name;
}

int Variable::getPos()
{
	return m_position;
}

Regs Variable::getAsignment()
{
	return m_assignment;
}

int Variable::getValue()
{
	return m_value;
}


Instruction::~Instruction()
{
}

int Instruction::getNumValue()
{
	return numValue;
}



std::string Instruction::getJumpLabel()
{
	return jump_label;
}

int Instruction::getPos()
{
	return m_position;
}

InstructionType Instruction::getType()
{
	return m_type;
}

void Instruction::addSucc(Instruction * instr)
{
	m_succ.push_back(instr);
}

void Instruction::addPred(Instruction * instr)
{
	m_pred.push_back(instr);
}

void Instruction::printInstruction()
{
	std::cout << "Position:\t" <<  m_position;

	std::cout << "\n\nType: " << instrTypeToStr(m_type);


	std::cout << "\n\n\nPRED:\t";
	printInstructionsPos(m_pred);


	std::cout << "\n\nSUCC:\t";
	printInstructionsPos(m_succ);


	std::cout << "\n\nUSE:\t";
	printVarNames(m_use);
	

	std::cout << "\nDEF:\t";
	printVarNames(m_def);

	std::cout << "\nIN:\t";
	printVarNames(m_in);

	std::cout << "\nOUT:\t";
	printVarNames(m_out);
	std::cout << std::endl;
}

void Instruction::printInstructionsPos(const Instructions & instrs)
{
	for each (Instruction* var in instrs)
	{
		std::cout << var->getPos() << " ";
	}
}

void Instruction::printVarNames(const Variables & vars)
{
	for each (Variable* var in vars)
	{
		std::cout << *var << " ";
	}
	std::cout << std::endl;
}

std::ostream & operator<<(std::ostream & out, const Variable & v)
{
	return out << v.m_name;
}

std::string varTypeToStr(Variable::VariableType type)
{
	switch (type)
	{
	case Variable::MEM_VAR:			return "memory";
	case Variable::REG_VAR:			return "register";
	case Variable::NO_TYPE:			return "no type";
	default:						return "";
	};
}
