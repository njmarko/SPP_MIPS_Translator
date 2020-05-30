#include "ResourceAlocation.h"

void ResourceAllocation::visit(SymbolTable & symTab)
{
	try
	{
		buildInterferenceGraph(symTab.getInterferenceGraph(), symTab.getInstructions(), symTab.getRegVariables());
		performSimplification(symTab.getInterferenceGraph(), symTab.getSimplificationStack(), symTab.getRegVariables());
		selectRegisters(symTab.getInterferenceGraph(), symTab.getSimplificationStack());
	}
	catch (const std::runtime_error e) // if the spill happens
	{
		std::cout << e.what() << std::endl;
		handleSpill(symTab.getInstructions(),symTab.getRegVariables(),symTab.getMemVariables(),symTab.getLabels(),symTab.getInterferenceGraph(),symTab.getSpilledVars());
		symTab.resetData(); // reset the data for the connect instructions,liveness analysis and resource alocation to work properly.
		// throws the error so another iteration of  the algorithm is performed starting from liveness analysis
		throw std::runtime_error("Register " + symTab.getSpilledVars().back()->getName() + " was spilled to memory location " + symTab.getMemVariables().back()->getName() + "!"); 
	}
}

void ResourceAllocation::handleSpill(Instructions & instr, Variables & r_vars, Variables & m_vars, Labels & labels, InterferenceGraph& ig, Variables & spilled_vars)
{
	//TODO: add r1,r2,r3 can be replaced with three instrucitons
	/* 
	* xor r1,r1,r1
	*	add r1,r1,r2
	*	add r1,r1,r3
	*
	* sub r1,r2,r3 can be replaced with
	*	xor r1,r1,r1
	*	sub r1,r1,r3
	*	add r1,r1,r2
	*
	* addi r1,r2,5 can be replaced with
	*	xor r1,r1,r1
	*	addi r1,r1,5
	*	add r1,r1,r2
	* and the ADD instruction that was created can then be transformed to immediate operand according to the procedure described below
	*	addi r1,r1,(value at the mem location)
	*
	* xor r1,r2,r3 can be replaced with
	*	xor r1,r1,r1
	*	xor r1,r1,r2
	*	xor r1,r1,r3
	*
	* la r1,m1
	* lw r2,0(r1) can be replaced if the value stored in m1 does not change ever (no sw that stores into it)
	* and if the value stored in m1 can fit in 16 bits with the isntruction
	*	li r2,(num value from m1)
	*
	/*


	//TODO: add a structure that stores all the registers that were already spilled to memory

	//TODO: add optimisation that replaces ADD r1,r2,r3 type instructions with ADDI instructions
	/*
	* first a procedure should be performed that replaces add r1,r2,r3 with three instructions, xor,add,add
	* then it should be checked if the source register that is used is not the destination register that is being defined
	* if it is not it should be checked if the register that is used is smaller than the size of immediate operand (addi instr)
	* size of the immediate operand can be 16 bits (65535 unsigned or -32,768 to +32,767  signed)
	*/


	// First the register variable with the most interferences is picked only if was not previously spilled into memory
	std::map <Variable*, int> varRang = makeVarInterferenceMap(ig,r_vars);
	Variable* replacedVar = nullptr;
	for each (std::pair<Variable*,int> p in varRang)
	{
		if (std::find(spilled_vars.cbegin(),spilled_vars.cend(),p.first) == spilled_vars.cend()) // if the var was not spilled
		{
			if (replacedVar == nullptr || p.second >varRang[replacedVar])
			{
				replacedVar = p.first;
			}
		}
	}
	
	if (replacedVar == nullptr)
	{
		// TODO: decide how to handle the case when all the variables are already spilled
		throw NoMoreSpillsPossible("No more spilling is possible because all the variables are already spilled!");
	}
	

	// A new memory variable is needed for storing the spilled data

	Variable* mem_var = createNewMemVariable(m_vars);
	Variables memList; // I make the variables list here because the instruction constructor uses it
	memList.push_back(mem_var);
	m_vars.push_back(mem_var); // added to the list of all mem variables used in the code that is in symTable

	/*
	* Before every instruction that uses the selected regVar, we need to add 2 instructions intended
	* for loading the instruction from memory to the selected register variable.
	* Also for every instruction that defines the selected register variable, we need to 
	* add an instruction after it that stores the selected variable;
	*
	* Another possible heuristic would be to find the regVar that is used the least in the program
	*
	* It would be good if we could chose a regVar that is not inside a loop
	*/


	for (Instructions::const_iterator cit = instr.cbegin(); cit != instr.cend();++cit) {
		// first check for usage of the selected regVar
		if ((*cit)->checkVarInUse(*replacedVar))
		{
			// if the regVar is used, then the regvar has to be loaded from the memory

			/*
			* real position will be calculated after all the instructions are added
			* type of instruction is load address E → la rid, mid
			* destination is a new registerVariable (we assume there are infinite ammount of them)
			* source is the memory variable that replaces the regVar (it was previously created)
			* parent label is the same lable where the encountered instruction belongs
			*/

			Variables dstList;
			Variable* r1 = createNewRegVariable(r_vars);
			r_vars.push_back(r1); // added to the list of all reg variables used in the code that is in symTable
			spilled_vars.push_back(r1); // added to the lsit of all reg variables that were spilled or added during the spilling procedure
			dstList.push_back(r1);
			Instruction* la = new Instruction((*cit)->getPos(), InstructionType::I_LA, dstList, memList,(*cit)->getParentLabel());
			la->fillDefVariables(); // la instruction only fills the def set
			instr.insert(cit, la); // inserts the new instruction before the chosen instruction pointed by the iterator

			/*
			* real position will be calculated after all the instructions are added
			* type of instruction is load word E → lw rid, num(rid)
			* destination is a the register that was selected to be spilled into memory
			* source is the register that holds the address that was loaded previously with "la" instruction
			* parent label is the same lable where the encountered instruction belongs
			* num is the value before the parenthesis in the "lw" instruction
			*/

			dstList.clear(); // could be done with less code by renaming and reusing the Variables, but code clarity is more important here
			Variables srcList;
			srcList.push_back(r1);
			dstList.push_back(replacedVar);
			Instruction* lw = new Instruction((*cit)->getPos(), InstructionType::I_LW, dstList, srcList, (*cit)->getParentLabel(),0);
			lw->fillDefVariables();
			lw->fillUseVariables();
			instr.insert(cit, lw); // inserts the new instruction before the chosen instruction pointed by the iterator
		}


		// after that check for definitions of the selected regVar
		if ((*cit)->checkVarInDef(*replacedVar))
		{
			// if the regVar is defined, then the result has to be stored in memory

			// these instructions should be added after the current instruction that defines the selected regVar
			Instructions::const_iterator after(cit);
			++after;

			/*
			* real position will be calculated after all the instructions are added
			* type of instruction is load address E → la rid, mid
			* destination is a new registerVariable (we assume there are infinite ammount of them)
			* source is the memory variable that replaces the regVar (it was previously created)
			* parent label is the same lable where the encountered instruction belongs
			*/

			Variables dstList;
			Variable* r1 = createNewRegVariable(r_vars);
			r_vars.push_back(r1);  // added to the list of all reg variables used in the code that is in symTable
			spilled_vars.push_back(r1); // added to the lsit of all reg variables that were spilled or added during the spilling procedure
			dstList.push_back(r1);
			Instruction* la = new Instruction((*cit)->getPos(), InstructionType::I_LA, dstList, memList, (*cit)->getParentLabel());
			la->fillDefVariables(); // la instruction only fills the def set
			instr.insert(after, la); // inserts the new instruction before the next pointed by the after iterator

			/*
			* real position will be calculated after all the instructions are added
			* type of instruction is load word E → sw rid, num(rid)
			* source1 is the register that was selected to be moved into memory and was defined in the current instruction
			* destination is a the register that holds the address of the new memory variable (it is technically a "source2" because it is in the use set)
			* parent label is the same lable where the encountered instruction belongs
			* num is the value before the parenthesis in the "sw" instruction
			*/

			dstList.clear(); // could be done with less code by renaming and reusing the Variables, but code clarity is more important here
			Variables srcList;
			srcList.push_back(replacedVar);
			srcList.push_back(r1);
			// In sw instruction, source register is on the left, another source register is on the right! Right reg just holds the address to the real memory destination
			Instruction* sw = new Instruction((*cit)->getPos(), InstructionType::I_SW, dstList, srcList, (*cit)->getParentLabel(), 0); // in sw src and dst change places
			sw->fillUseVariables(); // sw only has use variables, because the first and second registers are just used
			instr.insert(after, sw); // inserts the new instruction before the next pointed by the after iterator

			// because the 2 instructions are added after the current instruction iterator it has to be incremented 2 times
			++cit;
			++cit;
		}
	}
	/*
	* Next step is to go trough all the instructions and choose the new position numbers for them.
	* At the same time update the list of labels, so the labels now hold the correct position of the
	* first instruction that belongs to them
	*/

	int pos_num = 0; // start counting instructions from 0
	std::string current_label = ""; // used to keep track if the instruction in the iteration belongs to a different label
	for each (Instruction* var in instr)
	{
		var->setPosition(pos_num);
		if (var->getParentLabel() != current_label)
		{
			current_label = var->getParentLabel();
			// labels only keep track of the position of its first variable, so we assign that value on labelName change
			for each (std::pair<std::string, int> p in labels)
			{
				if (p.first == current_label) {
					p.second = pos_num;
				}
			}
		}
		++pos_num;
	}

	spilled_vars.push_back(replacedVar); // finally add the replaced variable to the list of spilled variables

	/*
	* Reset of some data is needed because connecting instructions, liveness analysis and register allocation has to be performed again.
	* This reset is done at the start of the connectInstructions function in the SymbolTable where all the data is located
	* because this algoritham jumps back to connectInstructions after the spill happens
	* connecting the instructions could have been performed here also, and then the algorithm would continue at liveness analysis phase
	*/

}

Variable * ResourceAllocation::createNewMemVariable(Variables& m_vars)
{
	std::string memVarName = "m1";
	// First we find an original name for the variable
	int number = 1;
	bool repeat = false;
	while (true)
	{
		repeat = false;
		for each (Variable* var in m_vars)
		{
			if (var->getName() == memVarName)
			{
				repeat = true;
				memVarName = "m" + std::to_string(++number);
				break;
			}
		}
		if (!repeat) {
			break;
		}
	}
	return new Variable(memVarName, -1, Variable::MEM_VAR, "0"); // take string as a value;
}

Variable * ResourceAllocation::createNewRegVariable(Variables& r_vars)
{
	std::string memVarName = "r1";
	// First we find an original name for the variable
	int number = 1;
	bool repeat = false;
	while (true)
	{
		repeat = false;
		for each (Variable* var in r_vars)
		{
			if (var->getName() == memVarName)
			{
				repeat = true;
				memVarName = "r" + std::to_string(++number);
				break;
			}
		}
		if (!repeat) {
			break;
		}
	}
	return new Variable(memVarName, number-1, Variable::REG_VAR);
}

std::map<Variable*, int> ResourceAllocation::makeVarInterferenceMap(InterferenceGraph & ig, Variables & r_vars)
{
	/*
	* First the rank is calculated for all the variables, and is stored in a map
	*/
	std::map <Variable*, int> varRang;
	int interferenceCount = 0;
	for each (Variable* var in r_vars)
	{
		interferenceCount = 0;
		for (size_t i = 0; i < ig.getIGMatrix().size(); i++)
		{
			// Interference matrix is simetric around the main diagonal so it doesn't matter how it is traversed
			if (ig.getIGMatrix()[i][var->getPos()] == __INTERFERENCE__)
			{
				++interferenceCount;
			}
		}
		varRang[var] = interferenceCount;
	}
	return varRang;
}

InterferenceGraph & ResourceAllocation::buildInterferenceGraph(InterferenceGraph& ig, Instructions & instr, Variables& vars)
{
	ig.setVars(&vars);
	ig.setMatrix(InterferenceMatrix(ig.getVars()->size(), std::vector<int>(ig.getVars()->size(), __EMPTY__)));
	for (Instructions::const_iterator cit = instr.cbegin(); cit != instr.cend(); ++cit) {

		// Go trough the list of DEF variables
		for (Variables::const_iterator vcit = (*cit)->getDef().cbegin(); vcit != (*cit)->getDef().end();++vcit) {
			for (Variables::const_iterator vcit2 = (*cit)->getOut().cbegin(); vcit2 != (*cit)->getOut().cend();++vcit2) {
				if ((*vcit)->getName() != (*vcit2)->getName()) { // do not add interference if it is the same reg variable
					ig.getIGMatrix()[(*vcit)->getPos()][(*vcit2)->getPos()] = __INTERFERENCE__;
					ig.getIGMatrix()[(*vcit2)->getPos()][(*vcit)->getPos()] = __INTERFERENCE__;
				}
			}
		}
	}
	return ig;
}

SimplificationStack & ResourceAllocation::performSimplification(InterferenceGraph & ig, SimplificationStack & sims, Variables & vars)
{
	// TODO: Change map key from variable pointer to string so the order is always the same for the same input code

	/*
	* First the rank is calculated for all the variables, and is stored in a map
	*/
	std::map <Variable*, int> varRang = makeVarInterferenceMap(ig,vars);

	// Repeat untill the stack is filled or runtime_error is thrown
	while (sims.size() < vars.size()) {

		// First search for the reg variable with the highest rank that is lower than the number of available registers
		Variable* highestRank = nullptr;
		for each (std::pair<Variable*,int> p in varRang)
		{
			if (p.second < __REG_NUMBER__)
			{
				if (highestRank == nullptr) {
						highestRank = p.first;
				}
				else if (p.second > varRang[highestRank])
				{
					highestRank = p.first;
				}
			}

		}
		if (highestRank == nullptr) // if there is no suitable register that means there is a spill that needs to be adressed
		{
			throw std::runtime_error("Spill happened while performing the simplification procedure!");
		}
		// add the variable to the simplification stack
		sims.push(highestRank);
		// reduce the rank of all the neighbours of the variable
		for each (std::pair<Variable*,int> p in varRang)
		{
			if (ig.getIGMatrix()[p.first->getPos()][highestRank->getPos()] == __INTERFERENCE__) {
				--varRang[p.first];
			}
		}
		// remove the variable from the map
		varRang.erase(highestRank);
	}
	return sims;
}

void ResourceAllocation::selectRegisters(InterferenceGraph & ig, SimplificationStack & ss)
{
	// set is used because we don't need duplicate register names
	std::set<Regs> allocated_regs;

	// we need to go through the enitre simplification stacks
	while (!ss.empty()) { 
		Variable* v = ss.top();
		ss.pop();
		int color = -1;
		allocated_regs.clear(); // each register has different interferences, so this set gets cleared
		for (Variables::const_iterator cit = ig.getVars()->cbegin(); cit != ig.getVars()->cend(); ++cit) {
			int pos = (*cit)->getPos();
			if (ig.getIGMatrix()[v->getPos()][pos] == __INTERFERENCE__ && (*cit)->getAsignment() != Regs(0)) { 
				// if the interference exists between the two variables, and it has an assigned register
				allocated_regs.emplace((*cit)->getAsignment());
			}
		}
		// look for the lowest numbered register color that is not taken by the neighbours of the variable
		bool taken = false;
		for (int i = 0; i < __REG_NUMBER__; i++)
		{
			taken = false;
			for each (Regs var in allocated_regs)
			{
				if (Regs(i+1) == var) {
					taken = true;
					break;
				}
			}
			if (!taken) {
				color = i+1;
				break;
			}
		}
		if (color == -1) {
			// if non of the standard available registers can be assigned
			throw std::runtime_error("Spill happened while performing register allocations!");
		}
		else
		{
			v->set_assignment(Regs(color));
		}
	}
}
