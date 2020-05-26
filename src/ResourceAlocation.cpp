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
		std::cout << e.what();
		handleSpill(symTab.getInstructions(),symTab.getRegVariables(),symTab.getMemVariables(),symTab.getLabels(),symTab.getInterferenceGraph());
		// throws the error so another iteration of  the algorithm is performed starting from liveness analysis
		throw std::runtime_error("Handling of the spilling was performed."); 
	}
}

void ResourceAllocation::handleSpill(Instructions & instr, Variables & r_vars, Variables & m_vars, Labels & labels, InterferenceGraph& ig)
{
	// First the register variable with the most interferences is picked
	std::map <Variable*, int> varRang = makeVarInterferenceMap(ig,r_vars);
	Variable* replacedVar = nullptr;
	for each (std::pair<Variable*,int> p in varRang)
	{
		if (replacedVar == nullptr || p.second >varRang[replacedVar])
		{
			replacedVar = p.first;
		}
	}
	// A new memory variable is needed for storing the spilled data
	std::string memVarName = "m1";
	// First we find an original name for the variable
	int number = 1;
	bool repeat = false;
	while (true)
	{
		for each (Variable* var in m_vars)
		{
			if (var->getName() == memVarName)
			{
				repeat = true;
				memVarName = "m" + (++number);
				break;
			}
		}
		if (!repeat) {
			break;
		}
	}
	m_vars.push_back(new Variable(memVarName,-1,Variable::MEM_VAR,0));
	/*
	* Before every instruction that uses the selected regVar, we need to add 2 instructions intended
	* for loading the instruction from memory to the selected register variable.
	* Also for every instruction that defines the selected register variable, we need to 
	* add an instruction after it that stores the selected variable;
	*/


	for (Instructions::const_iterator cit = instr.cbegin(); cit != instr.cend();++cit) {
		// first check for definitions of the selected regVar

	}



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

		//TODO: MOVE INSTRUCTIONS?

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
			if (highestRank == nullptr) {
				if (p.second < __REG_NUMBER__)
				{
					highestRank = p.first;
				}
			}
			else if (p.second > varRang[highestRank])
			{
				highestRank = p.first;
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
