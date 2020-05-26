#include "SymbolTable.h"



SymbolTable::SymbolTable():instrCount(0),regVarsCount(0)
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
	// there is no need to delete simplification stack (ss)
	// because it holds the same variables that are in the regVariables list
	memVariables.clear();
	regVariables.clear();
	instructions.clear();
}

void SymbolTable::accept(Visitor & v) {
	v.visit(*this);
}

void SymbolTable::connectInstructions()
{
	Instructions::iterator sucit = instructions.begin();
	Instructions::iterator predit = ++instructions.begin(); // predit is one step ahead of the sucit instruction
	InstructionType type;
	Instruction* jump_instr;
	for (;predit!=instructions.end();++sucit,++predit)
	{
		type = (*sucit)->getType();
		//This is where i connect branching instructions and throw error if the label doesn't exist
		if (InstructionType::I_B==type || InstructionType::I_BLTZ==type)
		{
			//TODO: Change how label is checked if it is not possible to jump to the function
			if (!isLabelDefined((*sucit)->getJumpLabel())) {
				throw std::runtime_error("Label " + (*sucit)->getJumpLabel() 
					+ " that the instruction " + instrTypeToStr((*sucit)->getType())
					+" is trying to jump to is not defined!");
			}
			jump_instr = getInstrFromLabel((*sucit)->getJumpLabel());
			if (jump_instr != nullptr) //it could happen if the gramar is changed
			{
				// adds the current branch instruction to the predecesors of the instruction that is being jumped to
				jump_instr->addPred(*sucit);
				// adds the instruction that is being jumped to to the list of successors of the current branch instruction
				(*sucit)->addSucc(jump_instr);
			}
		}
		if (InstructionType::I_B == type != type)
		{
			(*sucit)->addSucc(*predit); // predit is one step ahead of the sucit instruction
		}
		(*predit)->addPred((*sucit)); // predit is one step ahead of the sucit instruction
	}
}

void SymbolTable::resetData()
{
	// reset in,out,succ,pred sets in the instructions
	for each (Instruction* var in instructions)
	{
		var->resetInstruction();
	}
	// reset reg assignments from register allocation phase to regVariables
	for each (Variable* var in regVariables)
	{
		var->resetVariableAssignment();
	}
	// resets the interference graph data
	ig.resetIGData();

	// resets the simplificationstack
	while (!ss.empty()) {
		ss.pop();
	}
}


Instruction * SymbolTable::getInstrFromLabel(const std::string & label)
{
	/*
	* This may be an overcomplicated solution, because the gramar doesn't allow for some of the cases to happen
	* but the idea is that it will continue to work if the gramar for the labels changes
	* It also accounts for instructions being removed in some later steps
	*/

	bool lab_encountered = false; // begin checking instructions after this is true
	//first we iterate trough the labels
	for (Labels::const_iterator cit = labels.cbegin(); cit != labels.cend(); ++cit) {
		if (!lab_encountered && (*cit).first == label)
		{
			lab_encountered = true;
		}
		if (lab_encountered) // begin checking for instructions after label has been
		{
			// this can only happen if the the gramatic rules are changed or if it is possible to jump to a function
			// current rule is S → id: E , and that guarantees that instruction must come after the label
			if ((*cit).second == -1) continue; 
			for (Instructions::iterator instr_it = instructions.begin();instr_it != instructions.end();++instr_it)
			{
				if ((*instr_it)->getPos() >= (*cit).second) // if the instruction was removed it will take the next one
				{
					return *instr_it;
				}
			}
		}
	}
	return nullptr;
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
			throw std::runtime_error(makeInstructionErrorMsg(0,Variable::REG_VAR,i_type,*cit_params));
		}
		dst.push_back(v);
		if ((v = isRegVarDefined(*(++cit_params))) == nullptr)
		{
			throw std::runtime_error(makeInstructionErrorMsg(1, Variable::REG_VAR, i_type, *cit_params));
		}
		src.push_back(v);
		if ((v = isRegVarDefined(*(++cit_params))) == nullptr)
		{
			throw std::runtime_error(makeInstructionErrorMsg(2, Variable::REG_VAR, i_type, *cit_params));
		}
		src.push_back(v);
		// Increments the instruction counter whenever an instruction is created
		ins = new Instruction(instrCount++,i_type,dst,src,getParentLabel());
		ins->fillDefVariables();
		ins->fillUseVariables();
		break;
	case I_ADDI: // E → addi rid, rid, num
		if ((v = isRegVarDefined(*cit_params)) == nullptr)
		{
			throw std::runtime_error(makeInstructionErrorMsg(0, Variable::REG_VAR, i_type, *cit_params));
		}
		dst.push_back(v);
		if ((v = isRegVarDefined(*(++cit_params))) == nullptr)
		{
			throw std::runtime_error(makeInstructionErrorMsg(1, Variable::REG_VAR, i_type, *cit_params));
		}
		src.push_back(v);
		// Increments the instruction counter whenever an instruction is created
		ins = new Instruction(instrCount++, i_type, dst, src, getParentLabel(),std::stoi(*(++cit_params)));
		ins->fillDefVariables();
		ins->fillUseVariables();
		break;
	case I_LA: // E → la rid, mid
		if ((v = isRegVarDefined(*cit_params)) == nullptr)
		{
			throw std::runtime_error(makeInstructionErrorMsg(0, Variable::REG_VAR, i_type, *cit_params));
		}
		dst.push_back(v);
		if ((v = isMemVarDefined(*(++cit_params))) == nullptr)
		{
			throw std::runtime_error(makeInstructionErrorMsg(1, Variable::MEM_VAR, i_type, *cit_params));
		}
		src.push_back(v);
		// Increments the instruction counter whenever an instruction is created
		ins = new Instruction(instrCount++, i_type, dst, src, getParentLabel());
		ins->fillDefVariables();
		break;
	case I_LI: // E → li rid, num
		if ((v = isRegVarDefined(*cit_params)) == nullptr)
		{
			throw std::runtime_error(makeInstructionErrorMsg(0, Variable::REG_VAR, i_type, *cit_params));
		}
		dst.push_back(v);
		// Increments the instruction counter whenever an instruction is created
		ins = new Instruction(instrCount++, i_type, dst, src, getParentLabel(),std::stoi(*(++cit_params)));
		ins->fillDefVariables();
		break;
	case I_LW: // E → lw rid, num(rid)
		if ((v = isRegVarDefined(*cit_params)) == nullptr)
		{
			throw std::runtime_error(makeInstructionErrorMsg(0, Variable::REG_VAR, i_type, *cit_params));
		}
		dst.push_back(v);
		// it has to be a number to pass the syntax analysis, so no need to check for that
		offsetNum = std::stoi(*(++cit_params)); // gets the number before the parenthesis
		if ((v = isRegVarDefined(*(++cit_params))) == nullptr)
		{
			throw std::runtime_error(makeInstructionErrorMsg(1, Variable::REG_VAR, i_type, *cit_params));
		}
		src.push_back(v);
		// Increments the instruction counter whenever an instruction is created
		ins = new Instruction(instrCount++, i_type, dst, src, getParentLabel(), offsetNum);
		ins->fillDefVariables();
		ins->fillUseVariables();
		break;
	case I_SW: // E → sw rid, num(rid)
		// in store instruction, source is the first register and the second register is also used as a source
		// because SW instruction doesn't use memory variable name directly, it has no "destination"
		// instead of the destination, it uses the register that holds the address of the memory variable as a source
		if ((v = isRegVarDefined(*cit_params)) == nullptr)
		{
			throw std::runtime_error(makeInstructionErrorMsg(0, Variable::REG_VAR, i_type, *cit_params));
		}
		src.push_back(v); // adds the first register as a source
		// it has to be a number to pass the syntax analysis, so no need to check for that
		offsetNum = std::stoi(*(++cit_params)); // gets the number before the parenthesis
		if ((v = isRegVarDefined(*(++cit_params))) == nullptr)
		{
			throw std::runtime_error(makeInstructionErrorMsg(1, Variable::REG_VAR, i_type, *cit_params));
		}
		src.push_back(v); // also added to the list of source registers
		// Increments the instruction counter whenever an instruction is created
		ins = new Instruction(instrCount++, i_type, dst, src, getParentLabel(),offsetNum);
		//ins->fillDefVariables();
		ins->fillUseVariables();
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
			throw std::runtime_error(makeInstructionErrorMsg(0, Variable::REG_VAR, i_type, *cit_params));
		}
		dst.push_back(v);
		// Increments the instruction counter whenever an instruction is created
		ins = new Instruction(instrCount++, i_type, dst, src, *(++cit_params), getParentLabel());
		ins->fillUseVariables();
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
		throw std::runtime_error("Variable (memory) with the name: " + name + " is already defined!");
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
		throw std::runtime_error("Variable (register) with the name: " + name + " is already defined!");
	}

	std::regex regExp("r[0-9]+"); // varName – has sto start with letter 'r' and can be followed by any number
	if (!std::regex_match(name, regExp))
	{
		throw std::runtime_error("Incorrect variable (register) name: " + name);
	}
	// increments the regVarsCount
	regVariables.push_back(new Variable(name, regVarsCount++, type));
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

std::string SymbolTable::makeInstructionErrorMsg(int argPos, Variable::VariableType varType, InstructionType i_type, const std::string & faultyParam)
{
	std::vector <std::string> posName = { "first", "second","third" };
	if (argPos >=0 && argPos <=2)
	{
		return std::string("Undefined "+posName[argPos] +" variable (" + varTypeToStr(varType) + ") in the " + instrTypeToStr(i_type) + " instruction: " + faultyParam);
	}
	else {
		return std::string("Undefined variable (" + varTypeToStr(varType) + ") in the " + instrTypeToStr(i_type) + " instruction: " + faultyParam);
	}
}

void SymbolTable::printInstructions()
{
	printInstructions(instructions);
}

void SymbolTable::printInstructions(const Instructions& instrs)
{
	for each (Instruction* var in instrs)
	{
		std::cout << std::string(60,'=')<< std::endl;
		var->printInstruction();
		std::cout << std::string(60, '=') << std::endl;
	}
}

InterferenceGraph & SymbolTable::getInterferenceGraph()
{
	return ig;
}

SimplificationStack & SymbolTable::getSimplificationStack()
{
	return ss;
}
