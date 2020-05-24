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

std::string Variable::getValue()
{
	return m_value;
}

Instruction::~Instruction()
{
}

int Instruction::getOffsetNum()
{
	return offset_num;
}

std::string Instruction::getJumpLabel()
{
	return jump_label;
}

int Instruction::getPos()
{
	return m_position;
}

std::ostream & operator<<(std::ostream & out, const Variable & v)
{
	return out << v.m_name;
}
