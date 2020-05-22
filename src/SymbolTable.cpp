#include "SymbolTable.h"

Instructions & SymbolTable::getInstructions()
{
	return instructions;
}

Variables & SymbolTable::getVariables()
{
	return variables;
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

void SymbolTable::addVariable(Variable * var)
{
	variables.push_back(var);
}

void SymbolTable::addLabel(std::string lab_name, int pos)
{
	labels.emplace(lab_name, pos);
}

void SymbolTable::addFunction(std::string func_name, int pos)
{
	functions.emplace(func_name, pos);
}
