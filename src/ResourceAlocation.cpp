#include "ResourceAlocation.h"

void ResourceAllocation::visit(SymbolTable & symTab)
{
	buildInterferenceGraph(symTab.getInterferenceGraph(), symTab.getInstructions(), symTab.getRegVariables());
}

InterferenceGraph & ResourceAllocation::buildInterferenceGraph(InterferenceGraph& ig, Instructions & instr, Variables& vars)
{
	ig.setVars(&vars);
	ig.setMatrix(InterferenceMatrix(ig.getVars()->size(), std::vector<int>(ig.getVars()->size(), __EMPTY__)));
	for (Instructions::const_iterator cit = instr.cbegin(); cit != instr.cend(); ++cit) {

		//TODO: MOVE INSTRUCTIONS?

		//prolazim kroz listu definisanih promenljivih (u ovom primeru ce uvek najvise jedna)
		for (Variables::const_iterator vcit = (*cit)->getDef().cbegin(); vcit != (*cit)->getDef().end();++vcit) {
			for (Variables::const_iterator vcit2 = (*cit)->getOut().cbegin(); vcit2 != (*cit)->getOut().cend();++vcit2) {
				if ((*vcit)->getName() != (*vcit2)->getName()) { //ne dodaje se smetnja ako je u pitanju ista promenljiva
					ig.getIGMatrix()[(*vcit)->getPos()][(*vcit2)->getPos()] = __INTERFERENCE__;
					ig.getIGMatrix()[(*vcit2)->getPos()][(*vcit)->getPos()] = __INTERFERENCE__;
				}
			}
		}
		
	}
	return ig;
}
