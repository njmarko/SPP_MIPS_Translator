#include <iostream>
#include <exception>

#include "LexicalAnalysis.h"
#include "SyntaxAnalysis.h"
#include "SymbolTable.h"
#include "LivenessAnalysis.h"
#include "ResourceAlocation.h"
using namespace std;


int main()
{
	try
	{
		std::string fileName = ".\\..\\examples\\simple.mavn";
		bool retVal = false;

		LexicalAnalysis lex;
		SymbolTable symTab;
		LivenessAnalysis livenessAnalysis;
		ResourceAllocation resourceAllocation;

		if (!lex.readInputFile(fileName))
			throw runtime_error("\nException! Failed to open input file!\n");

		lex.initialize();

		retVal = lex.Do();

		if (retVal)
		{
			cout << "Lexical analysis finished successfully!" << endl;
			lex.printTokens();
		}
		else
		{
			lex.printLexError();
			throw runtime_error("\nException! Lexical analysis failed!\n");
		}

		SyntaxAnalysis syn(lex,symTab);
		cout << "\nPerforming syntax analysis..." << endl;
		retVal = syn.Do();

		if (retVal)
		{
			cout << "Syntax analysis finished successfully!" << endl;
		}
		else
		{
			throw runtime_error("\nException! Syntax analysis failed!\n");
		}

		cout << "\nConnecting the instructions..." << endl;
		symTab.connectInstructions();
		cout << "Instructions connection successfull. Successor and predecessor sets were succesfully formed.\n" << endl;
		symTab.printInstructions();

		cout << "\nDoing Liveness analysis..." << endl;
		symTab.accept(livenessAnalysis);
		cout << "Liveness analysis finished successfully! In and out sets were formed!\n" << endl;
		symTab.printInstructions();

		cout << "\nDoing Resource allocation..." << endl;
		symTab.accept(resourceAllocation);
		cout << "Resource allocation finished successfully! Registers were successfully allocated!\n" << endl;
		symTab.printInstructions();

	}
	catch (runtime_error e)
	{
		cout << e.what() << endl;
		return 1;
	}

	return 0;
}
