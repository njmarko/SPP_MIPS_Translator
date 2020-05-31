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
	*	xor r1,r1,r1
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
	* and if the value stored in m1 can fit in 16 bits with the instruction
	*	li r2,(num value from m1)
	*
	/*

	//DONE: add a structure that stores all the registers that were already spilled to memory

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
	/*
	* Replace instructions where the spilled variable appears with the appropriate set of instructions
	* if the conditions for the instruction replacement are satisfied 
	* (example: replace add instruction olny if 3 different registers appear in it)
	*/
	decomposeInstructions(instr, replacedVar);

	/*
	* Check if the memory variable is being used for reading only, and if it is
	* check if it can be transformed into the immediate operand
	*/
	moveFromMemToImmediate(instr, m_vars);

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
			//TODO: check if the variable that is being defined is being used anywhere. If not, it is not neccesary to store it in memory

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
	* Next add the replaced variable to the list of spilled variables. It was aded here after the other variables that were added
	* in the process of spilling because of the easier error printing because it needs to show the variable that was chosen for the spill
	*/
	spilled_vars.push_back(replacedVar);


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


	/*
	* Reset of some data is needed because connecting instructions, liveness analysis and register allocation has to be performed again.
	* This reset is done at the start of the connectInstructions function in the SymbolTable where all the data is located
	* because this algoritham jumps back to connectInstructions after the spill happens
	* connecting the instructions could have been performed here also, and then the algorithm would continue at liveness analysis phase
	*/

}

void ResourceAllocation::decomposeInstructions(Instructions & instr, Variable * replaced_var)
{
	// here i will store instructions that will be deleted after the replacement instructions are added
	Instructions deleted;
	// iterate trough the instructions and add replacement instructions where it is necessary
	for (Instructions::const_iterator cit = instr.cbegin(); cit != instr.cend();++cit) {
		// only replace those instructions that satisfy the criteria that all 3 reg vars it contains are unique
		std::set<Variable*> unique_vars; // set cant hold duplicates
		unique_vars.insert((*cit)->getDst().cbegin(), (*cit)->getDst().cend()); // add variables from the dst set
		unique_vars.insert((*cit)->getSrc().cbegin(), (*cit)->getSrc().cend()); // add variables from the src set
		if (unique_vars.size()==3 && std::find(unique_vars.cbegin(),unique_vars.cend(),replaced_var)!=unique_vars.cend())
		{

			Variables dst_list;
			Variables src_list;
			Instruction* i1;
			Instruction* i2;
			Instruction* i3;
			switch ((*cit)->getType())
			{
			case I_ADD: // add r1, r2, r3
				/*	xor r1, r1, r1
				*	add r1, r1, r2
				*	add r1, r1, r3
				*
				* This procedure would actually not help the program with reducing registers
				* because if there is an register r4 that is live at the current instruction
				* that would mean that r2,r3 and r4 are live in add r1,r2,r3
				* and decomposing the add instruction to the 3 instructions wouldn't change much
				* because liveness would look like this:
				*	xor r1,r1,r1 = live r1,r4
				*	add r1,r1,r2 = live r1,r2,r4
				*	add r1,r1,r3 = live r1,r3,r4
				* So two of those instructions still have 3 variables that are live
				*/

				//// xor r1, r1, r1
				//dst_list = (*cit)->getDst();
				//src_list.push_back((*cit)->getDst().front());
				//src_list.push_back((*cit)->getDst().front());
				//i1 = new Instruction((*cit)->getPos(), InstructionType::I_XOR, dst_list, src_list, (*cit)->getParentLabel(), 0);
				//i1->fillDefVariables();
				//i1->fillUseVariables();

				////add r1, r1, r2
				//src_list.clear();
				//src_list.push_back((*cit)->getDst().front());
				//src_list.push_back((*cit)->getSrc().front());
				//i2 = new Instruction((*cit)->getPos(), InstructionType::I_ADD, dst_list, src_list, (*cit)->getParentLabel(), 0);
				//i2->fillDefVariables();
				//i2->fillUseVariables();

				////add r1, r1, r2
				//src_list.clear();
				//src_list.push_back((*cit)->getDst().front());
				//src_list.push_back((*cit)->getSrc().back());
				//i3 = new Instruction((*cit)->getPos(), InstructionType::I_ADD, dst_list, src_list, (*cit)->getParentLabel(), 0);
				//i3->fillDefVariables();
				//i3->fillUseVariables();

				//instr.insert(cit, i1); // inserts the new instruction before the chosen instruction pointed by the iterator
				//instr.insert(cit, i2); // inserts the new instruction before the chosen instruction pointed by the iterator
				//instr.insert(cit, i3); // inserts the new instruction before the chosen instruction pointed by the iterator
				//deleted.push_back(*cit); // queue the replaced instruction for deletion
				break;
			case I_SUB: // sub r1, r2, r3
				/*
				*	xor r1, r1, r1
				*	add r1, r1, r2
				*	sub r1, r1, r3		
				* Decomposition above would not help (see add example) with the problem of reducing the live variables
				*
				* On the other hand this decomposition below might help if the add can be transformed into addi
				*	neg r1,r3
				*	add r1,r1,r2
				* Lest assume that r4 is live at the sub instruction. That means that r2,r3 and r4 are live at the same time.
				* If the sub instruction is replaced with neg and addi instructions then liveness looks like this
				*	neg r1,r3 = live r3,r4
				*   addi r1,r1,(value that r2 was holding) = live r1,r4
				* In this case only 2 variables are live at each instruction that replaced sub instruction!
				*/

				//// xor r1, r1, r1
				//dst_list = (*cit)->getDst();
				//src_list.push_back((*cit)->getDst().front());
				//src_list.push_back((*cit)->getDst().front());
				//i1 = new Instruction((*cit)->getPos(), InstructionType::I_XOR, dst_list, src_list, (*cit)->getParentLabel(), 0);
				//i1->fillDefVariables();
				//i1->fillUseVariables();

				//// add r1, r1, r2
				//src_list.clear();
				//src_list.push_back((*cit)->getDst().front());
				//src_list.push_back((*cit)->getSrc().front());
				//i2 = new Instruction((*cit)->getPos(), InstructionType::I_ADD, dst_list, src_list, (*cit)->getParentLabel(), 0);
				//i2->fillDefVariables();
				//i2->fillUseVariables();

				//// sub r1, r1, r3	
				//src_list.clear();
				//src_list.push_back((*cit)->getDst().front());
				//src_list.push_back((*cit)->getSrc().back());
				//i3 = new Instruction((*cit)->getPos(), InstructionType::I_SUB, dst_list, src_list, (*cit)->getParentLabel(), 0);
				//i3->fillDefVariables();
				//i3->fillUseVariables();

				//instr.insert(cit, i1); // inserts the new instruction before the chosen instruction pointed by the iterator
				//instr.insert(cit, i2); // inserts the new instruction before the chosen instruction pointed by the iterator
				//instr.insert(cit, i3); // inserts the new instruction before the chosen instruction pointed by the iterator
				//deleted.push_back(*cit); // queue the replaced instruction for deletion
				break;
			case I_XOR: // xor r1,r2,r3
				/*
				*	xor r1,r1,r1
				*	xor r1,r1,r2
				*	xor r1,r1,r3
				*
				* This decomposition doesn't reduce number of live varaibles (see add and sub instructions)
				* One solution for reducing would be to replace xor with xori in moveFromMemToImmediate function if possible
				*/

				//// xor r1,r1,r1
				//dst_list = (*cit)->getDst();
				//src_list.push_back((*cit)->getDst().front());
				//src_list.push_back((*cit)->getDst().front());
				//i1 = new Instruction((*cit)->getPos(), InstructionType::I_XOR, dst_list, src_list, (*cit)->getParentLabel(), 0);
				//i1->fillDefVariables();
				//i1->fillUseVariables();

				//// xor r1,r1,r2
				//src_list.clear();
				//src_list.push_back((*cit)->getDst().front());
				//src_list.push_back((*cit)->getSrc().front());
				//i2 = new Instruction((*cit)->getPos(), InstructionType::I_XOR, dst_list, src_list, (*cit)->getParentLabel(), 0);
				//i2->fillDefVariables();
				//i2->fillUseVariables();

				//// xor r1,r1,r3
				//src_list.clear();
				//src_list.push_back((*cit)->getDst().front());
				//src_list.push_back((*cit)->getSrc().back());
				//i3 = new Instruction((*cit)->getPos(), InstructionType::I_XOR, dst_list, src_list, (*cit)->getParentLabel(), 0);
				//i3->fillDefVariables();
				//i3->fillUseVariables();

				//instr.insert(cit, i1); // inserts the new instruction before the chosen instruction pointed by the iterator
				//instr.insert(cit, i2); // inserts the new instruction before the chosen instruction pointed by the iterator
				//instr.insert(cit, i3); // inserts the new instruction before the chosen instruction pointed by the iterator
				//deleted.push_back(*cit); // queue the replaced instruction for deletion
				break;
			default:
				break;
			}
		}
		// this if is for addi instruction
		else if (unique_vars.size() == 2 && std::find(unique_vars.cbegin(), unique_vars.cend(), replaced_var) != unique_vars.cend())
		{
			switch ((*cit)->getType())
			{
			case I_ADDI: // addi r1, r2, number
					/*
					*	xor r1, r1, r1
					*	addi r1, r1, 5
					*	add r1, r1, r2
					* and the ADD instruction that was created can then be transformed to immediate operand according to the procedure described below
					*	addi r1, r1, (value at the mem location)
					*
					* This doesn't reduce number of live variables (see add,sub,xor descriptions)
					*/
				
				break;
			default:
				break;
			}
		}
	}
	/* 
	* Remove all the instructions that were decomposed from the list of instruction pointers 
	* and also delete the instruction to free dynamically allocated memory
	*/
	for each (Instruction* var in deleted)
	{
		instr.remove(var);
		delete var;
	}
	deleted.clear();
}

void ResourceAllocation::moveFromMemToImmediate(Instructions & instr, Variables & m_vars)
{
	// if the instruction for storing has an offset then this program will not try to 
	// evaluate any further and no moving from memory to immediate will happen
	Variables a_regs;
	for each (Instruction* i_var in instr)
	{
		if (i_var->getDst().size() > 0 && find(a_regs.cbegin(), a_regs.cend(), i_var->getDst().front()) != a_regs.cend())
		{
			// if the register that holds the adress is being redefined, remove it from the list of address registers
			a_regs.remove(i_var->getDst().front());
		}
		if (i_var->getType() == InstructionType::I_LA)
		{
			// keep track of all the registers that hold memory addresses
			a_regs.push_back(i_var->getDst().front());
		}

		if (i_var->getType() == InstructionType::I_SW) {
			/*
			* if offset is used, this heuristic with replacing mem values with immedaite operands
			* can potentialy not work so it is aborted.
			* This is because it is hard to keep track what mem location is only being read from
			*/
			if (i_var->getNumValue() != 0)
			{
				return;
			}
			/*
			* For this heuristic to keep track of memory locations that are used for reading only
			* memory address has to be loaded into register, and that register cannot be changed
			* before it is used in the sw instruction.
			* If sw instruction uses a register that doesn't contain an unchanged address loaded by la
			* then this procedure is aborted.
			* 
			*/
			if (find(a_regs.cbegin(), a_regs.cend(),i_var->getSrc().back()) == a_regs.cend())
			{
				return;
			}
		}
	}

	// candidate mem vars that will be candidates for moving to immediate operands
	Variables m_vars_candidates;

	// first we need to find mem vars that are read only and can fit into immediate operands (16 bits signed)
	for each (Variable* m_var in m_vars)
	{
		/*
		* Check if the mem variable value is in range to be replaced by immediate operand
		* range is -32,768 to +32,767 for signed immediate operand (16 bits)
		*/
		if (m_var->getValue() < -32768 || m_var->getValue() > 32767)
		{
			continue; // if it is not in range then move to the next mem var
		}

		/*
		* Check if there are any instructions that write to the current memory variables
		* First check what reg variable holds the address of the m_var, and then if that reg_var
		* is not redefined and is in sw instruction as a second source parameter, and the nubmber 
		* before the parenthesis is 0
		*/
		Variables address_regs; // registers that hold the address of the memory location
		m_vars_candidates.push_back(m_var); // add it right away, and only remove it if sw writes to that location
		for each (Instruction* i_var in instr)
		{
			if (i_var->getDst().size() > 0 && find(address_regs.cbegin(),address_regs.cend(),i_var->getDst().front()) != address_regs.cend())
			{
				// if the register that holds the adress is being redefined, remove it from the list of address registers
				address_regs.remove(i_var->getDst().front());
			}

			if (i_var->getType() == InstructionType::I_LA)
			{
				// if the memory adress was loaded into register with la instruction
				if (find(i_var->getSrc().cbegin(), i_var->getSrc().cend(),m_var) != i_var->getSrc().cend())
				{
					// this is a potential candidate for replacement by immediate operator if it turns out the memory is read only
					address_regs.push_back(i_var->getDst().front());
				}
			}
			if (i_var->getType() == InstructionType::I_SW) {
				// if sw instruction writes to the memory location then that memory location cant be transformed into immediate operand
				if (find(address_regs.cbegin(),address_regs.cend(),i_var->getSrc().back()) != address_regs.cend())
				{
					address_regs.clear();
					m_vars_candidates.remove(m_var); // this mem variable was written to so it is not a candidate
					break;
				}
			}			
		}
	}

	/*
	* la instructions that load the readonly mem loc address can't be deleted 
	* because maybe they are used for some other purpose (rare but possible)
	* example: system call that prints the address
	*/

	/*
	* At this point the mem variables that are read only are known
	* and they can now possibly be replaced with imediate operands
	*/
	for each (Variable* m_var in m_vars_candidates)
	{
		// reg variables that hold the address of the mem var
		Variables address_regs;

		// reg variables that hold the value loaded from the memory
		Variables regs_loaded_vals; 
		for each (Instruction* i_var in instr)
		{
			if (i_var->getDst().size() > 0 && find(address_regs.cbegin(), address_regs.cend(), i_var->getDst().front()) != address_regs.cend())
			{
				// if the register that holds the adress is being redefined, remove it from the list of address registers
				address_regs.remove(i_var->getDst().front());
			}
			if (i_var->getType() == InstructionType::I_LA)
			{
				// if the memory adress was loaded into register with la instruction
				if (find(i_var->getSrc().cbegin(), i_var->getSrc().cend(), m_var) != i_var->getSrc().cend())
				{
					// this is a potential candidate for replacement by immediate operator if it turns out the memory is read only
					address_regs.push_back(i_var->getDst().front());
				}
			}
			if (i_var->getType() == InstructionType::I_LW) {
				// if lw instruction loads from the current mem var into register
				if (find(address_regs.cbegin(), address_regs.cend(), i_var->getSrc().back()) != address_regs.cend())
				{
					// this register will be monitored and if it appears in add instruction it will be replaced with immediate value
					regs_loaded_vals.push_back(i_var->getDst().front()); 
					// also change this instruction to the load immediate (I_LI) type
					i_var->getSrc().clear(); // cleares the source set because load immediate only have destination set
					i_var->getUse().clear(); // cleares the use set because load immediate only have def set
					i_var->setNumValue(m_var->getValue()); // assigns the mem value as immediate operand
					i_var->setInstrType(InstructionType::I_LI); // change to type of the instruction to load immediate
					continue; // to avoid removing of the variable from regs_loaded_vals at the end of the loop
				}
			}

			if (i_var->getType() == InstructionType::I_ADD) {
				// if the add instruction contains register loaded from the memory

				// check if the last operand can be replaced by immediate value
				if (find(regs_loaded_vals.cbegin(), regs_loaded_vals.cend(), i_var->getSrc().back()) != regs_loaded_vals.cend())
				{
					i_var->getSrc().pop_back(); // remove the last operand from source
					i_var->getUse().pop_back(); // remove the last operand from use
					i_var->setNumValue(m_var->getValue()); // assigns the mem value as immediate operand
					i_var->setInstrType(InstructionType::I_ADDI); // change to type of the instruction to addi
				}
				// check if the first operand can be replaced by immediate value
				else if (find(regs_loaded_vals.cbegin(), regs_loaded_vals.cend(), i_var->getSrc().front()) != regs_loaded_vals.cend())
				{
					i_var->getSrc().pop_front(); // remove the first operand(this was the second operand will become the "first" operand)
					i_var->getUse().pop_front(); // remove the first operand from use set
					i_var->setNumValue(m_var->getValue()); // assigns the mem value as immediate operand
					i_var->setInstrType(InstructionType::I_ADDI); // change to type of the instruction to addi
				}
			}

			// this is at the end because instruction can use the reg with value from memory and at the same time define it (example add r1,r1,r2)
			if (i_var->getDst().size() > 0 && find(regs_loaded_vals.cbegin(), regs_loaded_vals.cend(), i_var->getDst().front()) != regs_loaded_vals.cend())
			{
				// if the register that holds value is being redefined, remove it from the list of registers with loaded values
				regs_loaded_vals.remove(i_var->getDst().front());
			}

		}
	}
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
