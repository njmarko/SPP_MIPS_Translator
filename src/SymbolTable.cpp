#include "SymbolTable.h"



SymbolTable::SymbolTable():instrCount(0)
{
}

SymbolTable::~SymbolTable()
{
	for each (Variable* var in memVariables)
	{
		delete var;
	}

	for each (Variable* var in regVariables)
	{
		delete var;
	}

	for each (Instruction* var in instructions)
	{
		delete var;
	}
	memVariables.clear();
	regVariables.clear();
	instructions.clear();
}

std::string SymbolTable::getParentLabel()
{
	if (labels.size() > 0)
	{
		return labels.back().first;
	}
	else {
		return "";
	}
}

Variable * SymbolTable::isMemVarDefined(const std::string& name)
{
	for each (Variable* var in memVariables)
	{
		if (var->getName() == name) {
			return var;
		}
	}

	return nullptr;
}

Variable * SymbolTable::isRegVarDefined(const std::string& name)
{
	for each (Variable* var in regVariables)
	{
		if (var->getName() == name) {
			return var;
		}
	}
	return nullptr;
}

bool SymbolTable::isFunctionDefined(const std::string& name)
{
	//Labels list contains all labels and functions
	//and funcion name can't overlap with either of them
	return isLabelDefined(name); 
}

bool SymbolTable::isLabelDefined(const std::string& name)
{
	for each (std::pair<std::string,int> p in labels)
	{
		if (p.first == name) {
			return true;
		}
	}
	return false;
}

int SymbolTable::getNumMemVariables()
{
	return memVariables.size();
}

int SymbolTable::getNumInstructions()
{
	return instructions.size();
}

Instructions & SymbolTable::getInstructions()
{
	return instructions;
}

Variables & SymbolTable::getMemVariables()
{
	return memVariables;
}

Variables & SymbolTable::getRegVariables()
{
	return regVariables;
}

Labels & SymbolTable::getLabels()
{
	return labels;
}

Functions & SymbolTable::getFunctions()
{
	return functions;
}

void SymbolTable::makeInstruction(const std::list<std::string>& params, InstructionType i_type)
{
	std::list<std::string>::const_iterator cit_params = params.cbegin();
	Variable* v;
	Instruction* ins;
	Variables src;
	Variables dst;
	switch (i_type)
	{
	case I_SUB: // E → sub rid, rid, rid
	case I_ADD: // E → add rid, rid, rid
		if ((v=isRegVarDefined(*cit_params))==nullptr)
		{
			throw std::runtime_error("Undefined first register variable in the ADD instruction: " + *cit_params);
		}
		dst.push_back(v);
		if ((v = isRegVarDefined(*(++cit_params))) == nullptr)
		{
			throw std::runtime_error("Undefined second register variable in the ADD instruction: " + *cit_params);
		}
		src.emplace_back(v);
		if ((v = isRegVarDefined(*(++cit_params))) == nullptr)
		{
			throw std::runtime_error("Undefined third register variable in the ADD instruction: " + *cit_params);
		}
		src.emplace_back(v);
		// Increments the instruction counter whenever an instruction is created
		ins = new Instruction(instrCount++,i_type,dst,src,getParentLabel());
		break;
	case I_ADDI:
		break;
	case I_LA:
		break;
	case I_LI:
		break;
	case I_LW:
		break;
	case I_SW:
		break;
	case I_BLTZ:
		break;
	case I_B:
		break;
	case I_NOP:
		break;
	case I_NO_TYPE: // just report the error if this happens
		break;
	default:
		break;
	}
	addInstruction(ins);
}

void SymbolTable::addInstruction(Instruction * instr)
{
	instructions.push_back(instr);

	/*
	* This part is for adding the position of the first instruction
	* to the latest label. If there are no labels 
	* or if the label already has instruction assigned
	* then this part is skipped.
	*/
	if (labels.size() != 0)
	{
		Labels::reverse_iterator rit = labels.rbegin();
		if (rit->second == -1) {
			rit->second = instr->getPos();
		}
	}
}

void SymbolTable::addMemVariable(const std::string& name, Variable::VariableType type, const std::string& value)
{
	if (isMemVarDefined(name) != nullptr)
	{
		throw std::runtime_error("Memory variable with the name: " + name + " is already defined!");
	}

	std::regex regExp("m[0-9]+"); // varName – has sto start with letter 'm' and can be followed by any number
	if (!std::regex_match(name,regExp))
	{
		throw std::runtime_error("Incorrect memory variable name: " + name);
	}

	memVariables.push_back(new Variable(name,-1,type,value));
}

void SymbolTable::addRegVariable(const std::string & name, Variable::VariableType type)
{
	if (isRegVarDefined(name) != nullptr)
	{
		throw std::runtime_error("Register variable with the name: " + name + " is already defined!");
	}

	std::regex regExp("r[0-9]+"); // varName – has sto start with letter 'r' and can be followed by any number
	if (!std::regex_match(name, regExp))
	{
		throw std::runtime_error("Incorrect register variable name: " + name);
	}

	regVariables.push_back(new Variable(name, -1, type));
}

void SymbolTable::addLabel(const std::string& lab_name)
{
	if (isFunctionDefined(lab_name))
	{
		throw std::runtime_error("Funcion or label with the name " + lab_name + " already exists!");
	}
	// labelName – has sto start with lowercase letter and can be followed by any number of alphanumeric characters
	std::regex regExp("[a-zA-Z]+[a-zA-Z0-9]*");
	if (!std::regex_match(lab_name, regExp))
	{
		throw std::runtime_error("Incorrect label name: " + lab_name);
	}

	// position of the first instruction that follows will be added when the instruction is added
	labels.emplace_back(lab_name, -1);
}

void SymbolTable::addFunction(const std::string& func_name)
{
	if (isFunctionDefined(func_name))
	{
		throw std::runtime_error("Funcion or label with the name " + func_name + " already exists!");
	}
	// funcName – has sto start with lowercase letter and can be followed by any number of alphanumeric characters
	std::regex regExp("[a-zA-Z]+[a-zA-Z0-9]*"); 
	if (!std::regex_match(func_name, regExp))
	{
		throw std::runtime_error("Incorrect function name: " + func_name + 
			"\nfuncName – has sto start with lowercase letter and can be followed by any number of alphanumeric characters");
	}

	functions.emplace_back(func_name);
	// position of the first instruction that follows will be added when the instruction is added
	labels.emplace_back(func_name,-1); 
}
