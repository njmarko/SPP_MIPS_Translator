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
	for each (const std::string fun in functions)
	{
		if (fun == name) {
			return true;
		}
	}
	return false;
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

Variables & SymbolTable::getVariables()
{
	return memVariables;
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

void SymbolTable::addMemVariable(std::string name, Variable::VariableType type, std::string value)
{
	if (isMemVarDefined(name) == nullptr)
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

void SymbolTable::addLabel(std::string lab_name, int pos)
{
	labels.emplace_back(lab_name,pos);
}

void SymbolTable::addFunction(std::string func_name, int pos)
{
	functions.emplace_back(func_name);
	addLabel(func_name, pos);
}
