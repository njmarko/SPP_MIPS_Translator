#include <iostream>
#include <exception>

#include "LexicalAnalysis.h"
#include "SyntaxAnalysis.h"
#include "SymbolTable.h"
#include "LivenessAnalysis.h"
#include "ResourceAlocation.h"
#include "MyFileHandler.h"


using namespace std;

/*
* Adding new isntrucitons process:
* Constants.h			 = increace NUM_STATES
* Types.h				 = add the instruction type to TokenType and to the InstructionType
* FiniteStateMachine.cpp = add TokenType to stateToTokenTable and add apropriate values to stateMatrix
* Token.cpp				 = in tokenTypeToString add token type and string value
* Expand the grammar	 = for instructions add it to nonterminal symbol E
* SyntaxAnalysis.cpp     = in E function add token type T_ according to gramar and call makeInstruction function
* SymbolTable.cpp		 = in function makeInstruction add case to switch according to instructionType;
						   If it is the same as format as some other instruction just add it without break above it
						   If it is different format, write code for creating the instruction of the new format
* IR.cpp				 = Add the case with the desired output for the MIPS instruction that will be written to file
*/

/*
* System call for printing int from memory 
	li $v0, 1       # $system call code for print_int
	la $t4,	m3
	lw $a0, 0($t4)       # $integer to print
	syscall         # print it
* System call for printing int from register
	li $v0, 1       # $system call code for print_int
	addi $a0, $t0, 0
	syscall         # print it
* System call for printing string from memory
	.data
	str:	.asciiz "\nThis is the result after add operation: "
	.text
	li $v0, 4       # $system call code for print_str
	la $a0, str     # $address of string to print
	syscall         # print the string
* System call for exitin the program
	li  $v0, 10
	syscall
*/

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

		while (true) {
			std::cout << "\nConnecting the instructions..." << endl;
			symTab.connectInstructions();
			std::cout << "Instructions connection successfull. Successor and predecessor sets were succesfully formed.\n" << endl;
			symTab.printInstructions();

			std::cout << "\nDoing Liveness analysis..." << endl;
			symTab.accept(livenessAnalysis);
			std::cout << "Liveness analysis finished successfully! In and out sets were formed!\n" << endl;
			symTab.printInstructions();

			try
			{
				std::cout << "\nDoing Resource allocation..." << endl;
				symTab.accept(resourceAllocation);
				std::cout << "Resource allocation finished successfully! Registers were successfully allocated!\n" << endl;
				symTab.printInstructions();
				symTab.getInterferenceGraph().printIGMatrix();
				break; // if no spills happen
			}
			catch (runtime_error e)
			{
				std::cout << e.what() << endl;
				continue;
			}
		}
		std::cout << "\nCreating MIPS output file..." << endl;
		symTab.accept(fileHandler);
		std::cout << "\nOutput file with the MIPS code successfully created!" << endl;

	}
	catch (runtime_error e)
	{
		std::cout << e.what() << endl;
		return 1;
	}

	return 0;
}
