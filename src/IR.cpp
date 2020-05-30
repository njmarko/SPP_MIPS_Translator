#include "IR.h"

void Variable::printFullInfo()
{
	std::cout << "\tName: " << m_name
		<< "\tPos:" << m_position << "\tAssignment: ";
	if (m_assignment <= 0)
	{
		std::cout << "no assignment"<< std::endl;
	}
	else {
		std::cout << "t" << m_assignment - 1<< std::endl;
	}
}

void Variable::resetVariableAssignment()
{
	set_assignment(Regs::no_assign);
}

std::ostream & operator<<(std::ostream & out, const Variable & v)
{
	return out << v.m_name;
}

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

Variable::VariableType Variable::getType() const
{
	return m_type;
}

std::string Variable::getName() const
{
	return m_name;
}

int Variable::getPos() const
{
	return m_position;
}

Regs Variable::getAsignment() const
{
	return m_assignment;
}

int Variable::getValue() const
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

Variables & Instruction::getDst()
{
	return m_dst;
}

Variables & Instruction::getSrc()
{
	return m_src;
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

	std::cout << "Type: " << instrTypeToWholeInstrStr(m_type);


	std::cout << "\t\tPosition: " <<  m_position;

	std::cout  << std::endl;
	std::cout << std::string(60, '-') << std::endl;
	int counter = 1;
	for each (Variable* var in m_dst)
	{
		std::cout << "Dst" << counter++ << ": ";
		var->printFullInfo();
	}
	counter = 1;
	for each (Variable* var in m_src)
	{
		std::cout << "Src" << counter++ << ": ";
		var->printFullInfo();
	}

	std::cout << std::string(60, '-') << std::endl;
	std::cout << "PRED:\t";
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

std::string Instruction::convertToMIPSString()
{
	// TODO: Group the output cases together and just type the name of the instruction based on type
	// TODO: Add instruction type to string method to support the request above

	std::stringstream ss; // string stream used to build MIPS instructions
	bool first;
	switch (m_type)
	{
	case I_NO_TYPE:
		break;
	case I_XOR:	// E → xor rid, rid, rid
	case I_MUL:	// E → mul rid, rid, rid
	case I_SUB: // E → sub rid, rid, rid
	case I_ADD: // E → add rid, rid, rids
		ss << "\t" << instrTypeStr(m_type)  << "\t";
		first = true;
		for each (Variable* var in m_dst)
		{
			if (first)
			{
				ss << "$t" << var->getAsignment() - 1;
				first = false;
			}
			else
			{
				ss << ", $t" << var->getAsignment() - 1;
			}
			
		}
		for each (Variable* var in m_src)
		{
				ss << ", $t" << var->getAsignment() - 1;
		}
		break;
	case I_ADDI: // E → addi rid, rid, num
		ss << "\taddi\t";
		first = true;
		for each (Variable* var in m_dst)
		{
			if (first)
			{
				ss << "$t" << var->getAsignment() - 1;
				first = false;
			}
			else
			{
				ss << ", $t" << var->getAsignment() - 1;
			}

		}
		for each (Variable* var in m_src)
		{
			ss << ", $t" << var->getAsignment() - 1;
		}
		ss << ", "  << getNumValue();
		break;
	case I_LA: // E → la rid, mids
		ss << "\tla\t";
		first = true;
		for each (Variable* var in m_dst)
		{
			if (first)
			{
				ss << "$t" << var->getAsignment() - 1;
				first = false;
			}
			else
			{
				ss << ", $t" << var->getAsignment() - 1;
			}

		}
		for each (Variable* var in m_src)
		{
				ss << ", " << var->getName();
		}
		break;
	case I_LI: // E → li rid, num
		ss << "\tli\t";
		first = true;
		for each (Variable* var in m_dst)
		{
			if (first)
			{
				ss << "$t" << var->getAsignment() - 1;
				first = false;
			}
			else
			{
				ss << ", $t" << var->getAsignment() - 1;
			}
		}
		ss << " ," << getNumValue();
		break;
	case I_LW: // E → lw rid, num(rid)
		ss << "\tlw\t";
		first = true;
		for each (Variable* var in m_dst)
		{
			if (first)
			{
				ss << "$t" << var->getAsignment() - 1;
				first = false;
			}
			else
			{
				ss << ", $t" << var->getAsignment() - 1;
			}
		}
		ss << ", " << getNumValue();
		ss << "($t" << (*m_src.cbegin())->getAsignment() - 1 << ")";
		break;
	case I_SW: // E → sw rid, num(rid)
		// SW instruction has 2 source registers and no destination registers
		ss << "\tsw\t";
		first = true;
		ss << "$t" << (*(m_src.cbegin()))->getAsignment() - 1; // first source register
		ss << ", " << getNumValue();
		// second source register actually holds the address of the mem variable that is the actuall destination of the instruction
		ss << "($t" << (*(++m_src.cbegin()))->getAsignment() - 1 << ")"; 
		break;
	case I_BLTZ: // E → bltz rid, id
		ss << "\tbltz\t";
		first = true;
		for each (Variable* var in m_src)
		{
			if (first)
			{
				ss << "$t" << var->getAsignment() - 1;
				first = false;
			}
			else
			{
				ss << ", $t" << var->getAsignment() - 1;
			}
		}
		ss << ", " << getJumpLabel();
		break;
	case I_B: // E → b id
		ss << "\tlw\t";
		ss << ", " << getJumpLabel();
		break;
	case I_NOP:
		ss << "\tnop";
		break;
	default:
		break;
	}
	return ss.str();
}

std::string Instruction::getParentLabel()
{
	return parent_label;
}

bool Instruction::checkVarInDef(Variable & v)
{
	for each (Variable* var in m_def)
	{
		if (var->getName() == v.getName()) {
			return true;
		}
	}
	return false;
}

bool Instruction::checkVarInUse(Variable & v)
{
	for each (Variable* var in m_use)
	{
		if (var->getName() == v.getName()) {
			return true;
		}
	}
	return false;
}

void Instruction::setPosition(int pos)
{
	m_position = pos;
}

void Instruction::resetInstruction()
{
	m_in.clear();
	m_out.clear();
	m_succ.clear();
	m_pred.clear();
}

void Instruction::setNumValue(int val)
{
	numValue = val;
}

void Instruction::setInstrType(InstructionType type)
{
	m_type = type;
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





InterferenceGraph::InterferenceGraph()
{
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

void InterferenceGraph::resetIGData()
{ 
	matrix.clear();
	vars = nullptr;
}
