#pragma once
#include "SymbolTable.h"

/*
* Class that will do the resource allocation
* There are four phases to resource allocation:
* Build, Simplify, Spill and Select
* 
*/
class ResourceAllocation :public Visitor {

	/*
	* Visitor class has pure virtual function visit that has to be implemented.
	* Function visit is called by the class that this Visitor visits
	*/
	void visit(SymbolTable& symTab)override;
};