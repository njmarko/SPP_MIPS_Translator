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
	Instruction* ins = nullptr;
	Variables src;
	Variables dst;
	int offsetNum;
	switch (i_type)
	{
	case I_SUB: // E → sub rid, rid, rid
	case I_ADD: // E → add rid, rid, rid
		if ((v=isRegVarDefined(*cit_params))==nullptr)
		{
			throw std::runtime_error("Undefined first variable (register) in the " + instrTypeToStr(i_type) + " instruction: " + *cit_params);
		}
		dst.push_back(v);
		if ((v = isRegVarDefined(*(++cit_params))) == nullptr)
		{
			throw std::runtime_error("Undefined second variable (register) in the " + instrTypeToStr(i_type) + " instruction: " + *cit_params);
		}
		src.push_back(v);
		if ((v = isRegVarDefined(*(++cit_params))) == nullptr)
		{
			throw std::runtime_error("Undefined third variable (register) in the " + instrTypeToStr(i_type) + " instruction: " + *cit_params);
		}
		src.push_back(v);
		// Increments the instruction counter whenever an instruction is created
		ins = new Instruction(instrCount++,i_type,dst,src,getParentLabel());
		break;
	case I_ADDI: // E → addi rid, rid, num
		if ((v = isRegVarDefined(*cit_params)) == nullptr)
		{
			throw std::runtime_error("Undefined first variable (register) in the " + instrTypeToStr(i_type) + " instruction: " + *cit_params);
		}
		dst.push_back(v);
		if ((v = isRegVarDefined(*(++cit_params))) == nullptr)
		{
			throw std::runtime_error("Undefined second variable (register) in the " + instrTypeToStr(i_type) + " instruction: " + *cit_params);
		}
		src.push_back(v);
		// Increments the instruction counter whenever an instruction is created
		ins = new Instruction(instrCount++, i_type, dst, src, getParentLabel(),std::stoi(*(++cit_params)));
		break;
	case I_LA: // E → la rid, mid
		if ((v = isRegVarDefined(*cit_params)) == nullptr)
		{
			throw std::runtime_error("Undefined first variable (register) in the " + instrTypeToStr(i_type) + " instruction: " + *cit_params);
		}
		dst.push_back(v);
		if ((v = isMemVarDefined(*(++cit_params))) == nullptr)
		{
			throw std::runtime_error("Undefined second variable (memory) in the " + instrTypeToStr(i_type) + " instruction: " + *cit_params);
		}
		src.push_back(v);
		// Increments the instruction counter whenever an instruction is created
		ins = new Instruction(instrCount++, i_type, dst, src, getParentLabel());
		break;
	case I_LI: // E → li rid, num
		if ((v = isRegVarDefined(*cit_params)) == nullptr)
		{
			throw std::runtime_error("Undefined first variable (register) in the " + instrTypeToStr(i_type) + " instruction: " + *cit_params);
		}
		dst.push_back(v);
		// Increments the instruction counter whenever an instruction is created
		ins = new Instruction(instrCount++, i_type, dst, src, getParentLabel(),std::stoi(*(++cit_params)));
		break;
	case I_LW: // E → lw rid, num(rid)
	case I_SW: // E → sw rid, num(rid)
		if ((v = isRegVarDefined(*cit_params)) == nullptr)
		{
			throw std::runtime_error("Undefined first variable (register) in the "+ instrTypeToStr(i_type) + " instruction: " + *cit_params);
		}
		dst.push_back(v);
		offsetNum = std::stoi(*(++cit_params));
		if ((v = isRegVarDefined(*(++cit_params))) == nullptr)
		{
			throw std::runtime_error("Undefined second variable (memory) in the " + instrTypeToStr(i_type) + " instruction: " + *cit_params);
		}
		src.push_back(v);
		// Increments the instruction counter whenever an instruction is created
		ins = new Instruction(instrCount++, i_type, dst, src, getParentLabel(),offsetNum);
		break;
	case I_BLTZ: // E → bltz rid, id
		/*
		* Existance of the lable that is being jumped to will not be checked here
		* because the target lable can be declared after this instruction.
		* Existance of the lable will be checked in the later steps
		* one all instructions,labels and variables have been added
		*/
		if ((v = isRegVarDefined(*cit_params)) == nullptr)
		{
			throw std::runtime_error("Undefined first variable (register) in the " + instrTypeToStr(i_type) + " instruction: " + *cit_params);
		}
		dst.push_back(v);
		// Increments the instruction counter whenever an instruction is created
		ins = new Instruction(instrCount++, i_type, dst, src, *(++cit_params), getParentLabel());
		break;
	case I_B: // E → b id
		/*
		* Existance of the lable that is being jumped to will not be checked here
		* because the target lable can be declared after this instruction.
		* Existance of the lable will be checked in the later steps 
		* one all instructions,labels and variables have been added
		*/
		// Increments the instruction counter whenever an instruction is created
		ins = new Instruction(instrCount++, i_type, dst, src, *(cit_params),getParentLabel());
		break;
	case I_NOP: // E → nop
		ins = new Instruction(instrCount++, i_type, dst, src, getParentLabel());
		break;
	case I_NO_TYPE: // just report the error if this happens
		break;
	default:
		break;
	}
	if (ins != nullptr) {
		addInstruction(ins);
	}
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
		throw std::runtime_error("variable (memory) with the name: " + name + " is already defined!");
	}

	std::regex regExp("m[0-9]+"); // varName – has sto start with letter 'm' and can be followed by any number
	if (!std::regex_match(name,regExp))
	{
		throw std::runtime_error("Incorrect variable (memory) name: " + name);
	}

	memVariables.push_back(new Variable(name,-1,type,value));
}

void SymbolTable::addRegVariable(const std::string & name, Variable::VariableType type)
{
	if (isRegVarDefined(name) != nullptr)
	{
		throw std::runtime_error("variable (register) with the name: " + name + " is already defined!");
	}

	std::regex regExp("r[0-9]+"); // varName – has sto start with letter 'r' and can be followed by any number
	if (!std::regex_match(name, regExp))
	{
		throw std::runtime_error("Incorrect variable (register) name: " + name);
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
