#include "SymbolTable.h"



SymbolTable::~SymbolTable()
{
	//TODO
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

void SymbolTable::addInstruction(Instruction * instr)
{
	instructions.push_back(instr);
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
