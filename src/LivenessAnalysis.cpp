#include "LivenessAnalysis.h"

void LivenessAnalysis::visit(SymbolTable & symTab)
{
	doLivenessAnalysis(symTab.getInstructions());
}

void LivenessAnalysis::doLivenessAnalysis(Instructions & instr)
{
	bool difference;
	int num_steps = 0;
	do {
		difference = false;
		for (Instructions::reverse_iterator ri = instr.rbegin(); ri != instr.rend();++ri) {
			Variables in = (*ri)->getIn();
			Variables out = (*ri)->getOut();

			// First all the in elements are added from the successors
			for (Instructions::const_iterator cit = (*ri)->getSucc().cbegin(); cit != (*ri)->getSucc().cend(); ++cit)
			{
				(*ri)->getOut().insert((*ri)->getOut().end(), // adds variables to the end of the OUT list
					(*cit)->getIn().cbegin(), // starting point of the IN list from where to add
					(*cit)->getIn().cend()); // ending point
			}
			// Sort the out variables
			(*ri)->getOut().sort();
			// Get rid of duplicates from the out list
			(*ri)->getOut().unique();

			// Add USE variables to the IN list
			(*ri)->getIn().insert((*ri)->getIn().end(),// adds variables to the end of the IN list
				(*ri)->getUse().cbegin(), // starting point of the USE list from where to add
				(*ri)->getUse().cend()); // ending point
			// First calculate the difference of the sets, and only then join them with the USE set
			for each (Variable* var in (*ri)->getOut())
			{
				if (std::find((*ri)->getDef().cbegin(), (*ri)->getDef().cend(),var) == (*ri)->getDef().cend())
				{
					(*ri)->getIn().insert((*ri)->getIn().end(),var);
				}
			}
			// sort the IN variables
			(*ri)->getIn().sort();
			// get rid of duplicates
			(*ri)->getIn().unique();
			// check if there have been changes to the in and out sets
			if (in != (*ri)->getIn() || out != (*ri)->getOut())
			{
				difference = true;
			}
		}
		++num_steps;
	} while (difference);


}
