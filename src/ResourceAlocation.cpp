#include "ResourceAlocation.h"

void ResourceAllocation::visit(SymbolTable & symTab)
{
	buildInterferenceGraph(symTab.getInterferenceGraph(), symTab.getInstructions(), symTab.getRegVariables());
	performSimplification(symTab.getInterferenceGraph(), symTab.getSimplificationStack(), symTab.getRegVariables());
	selectRegisters(symTab.getInterferenceGraph(), symTab.getSimplificationStack());
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
	std::map <Variable*, int> varRang;
	int interferenceCount = 0;
	for each (Variable* var in vars)
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

	std::set<Regs> allocated_regs;


	while (!ss.empty()) { //dok ne prodjem kroz sve variable
		Variable* v = ss.top();
		ss.pop();
		int color = -1;
		allocated_regs.clear();
		for (Variables::const_iterator cit = ig.getVars()->cbegin(); cit != ig.getVars()->cend(); ++cit) {
			int pos = (*cit)->getPos();
			if (ig.getIGMatrix()[v->getPos()][pos] == __INTERFERENCE__ && (*cit)->getAsignment() != Regs(0)) { //ako postoji smetnja za tu varijablu
				allocated_regs.emplace((*cit)->getAsignment());
			}
		}
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
