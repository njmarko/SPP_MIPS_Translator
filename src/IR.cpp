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
