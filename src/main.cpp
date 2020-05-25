#include <iostream>
#include <exception>

#include "LexicalAnalysis.h"
#include "SyntaxAnalysis.h"
#include "SymbolTable.h"
#include "LivenessAnalysis.h"
#include "ResourceAlocation.h"
#include "MyFileHandler.h"


using namespace std;


int main(int argc, char* argv[])
{
	try
	{

		MyFileHandler fileHandler(argc,argv);

		bool retVal = false;

		LexicalAnalysis lex;
		SymbolTable symTab;
		LivenessAnalysis livenessAnalysis;
		ResourceAllocation resourceAllocation;

		
		if (argc == 3)
		{
			if (!lex.readInputFile(fileHandler.getInFilepath()))
				throw runtime_error("\nException! Failed to open input file!\n");
		}
		else if (argc == 4) {
			if (!lex.readInputFile(fileHandler.getInFilepath())) {
				if (!lex.readInputFileName(fileHandler.createProgramFromFilenames()))
					throw runtime_error("\nException! Failed to open input file or folder!\n");
			}
		}


		lex.initialize();

		retVal = lex.Do();

		if (retVal)
		{
			std::cout << "Lexical analysis finished successfully!" << endl;
			lex.printTokens();
		}
		else
		{
			lex.printLexError();
			throw runtime_error("\nException! Lexical analysis failed!\n");
		}

		SyntaxAnalysis syn(lex,symTab);
		std::cout << "\nPerforming syntax analysis..." << endl;
		retVal = syn.Do();

		if (retVal)
		{
			std::cout << "Syntax analysis finished successfully!" << endl;
		}
		else
		{
			throw runtime_error("\nException! Syntax analysis failed!\n");
		}

		std::cout << "\nConnecting the instructions..." << endl;
		symTab.connectInstructions();
		std::cout << "Instructions connection successfull. Successor and predecessor sets were succesfully formed.\n" << endl;
		symTab.printInstructions();

		std::cout << "\nDoing Liveness analysis..." << endl;
		symTab.accept(livenessAnalysis);
		std::cout << "Liveness analysis finished successfully! In and out sets were formed!\n" << endl;
		symTab.printInstructions();

		std::cout << "\nDoing Resource allocation..." << endl;
		symTab.accept(resourceAllocation);
		std::cout << "Resource allocation finished successfully! Registers were successfully allocated!\n" << endl;
		symTab.printInstructions();
		symTab.getInterferenceGraph().printIGMatrix();

	}
	catch (runtime_error e)
	{
		std::cout << e.what() << endl;
		return 1;
	}

	return 0;
}
