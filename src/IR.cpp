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

Variables & Instruction::getIn()
{
	return m_in;
}

Variables & Instruction::getOut()
{
	return m_out;
}

Variables & Instruction::getUse()
{
	return m_use;
}

Variables & Instruction::getDef()
{
	return m_def;
}

Instructions& Instruction::getSucc()
{
	return m_succ;
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


	std::cout << "\n\nPRED:\t";
	printInstructionsPos(m_pred);


	std::cout << "\nSUCC:\t";
	printInstructionsPos(m_succ);


	std::cout << "\nUSE:\t";
	printVarNames(m_use);
	

	std::cout << "DEF:\t";
	printVarNames(m_def);

	std::cout << "IN:\t";
	printVarNames(m_in);

	std::cout << "OUT:\t";
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

void Instruction::fillDefVariables()
{
	for each (Variable* var in m_dst)
	{
		m_def.push_back(var);
	}
}

void Instruction::fillUseVariables()
{
	for each (Variable* var in m_src)
	{
		m_use.push_back(var);
	}
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

void InterferenceGraph::printIGMatrix()
{
	std::string line = "========";
	for (size_t i = 0; i < matrix.size(); i++)
	{
		line += "========";
	}

	std::cout << line << "\nInterference matrix: \n" << line << std::endl;

	for each (Variable* var in *vars)
	{
		std::cout << "\t" << *var;
	}
	std::cout << std::endl;
	
	Variables::const_iterator vcit = (*vars).cbegin();
	for (size_t i = 0; i < matrix.size(); ++i,++vcit)
	{
		std::cout << *(*vcit);
		for (size_t j = 0; j < matrix[i].size(); ++j)
		{
			std::cout << "\t" << matrix[i][j];
		}
		std::cout << std::endl;
	}
	std::cout << line << std::endl;
}

InterferenceMatrix & InterferenceGraph::getIGMatrix()
{
	return matrix;
}

Variables * InterferenceGraph::getVars()
{
	return vars;
}

void InterferenceGraph::setMatrix(InterferenceMatrix & m)
{
	matrix = m;
}

void InterferenceGraph::setVars(Variables * v)
{
	vars = v;
}
