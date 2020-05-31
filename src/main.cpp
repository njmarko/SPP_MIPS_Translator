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
* Types.h				 = add the instruction type to TokenType and to the InstructionType
* Types.cpp				 = add the instruction to to the functions that convert instructions to string
* FiniteStateMachine.cpp = add TokenType to stateToTokenTable and add apropriate values to stateMatrix
* Constants.h			 = increace NUM_STATES
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

/*
* Program arguments ( '/' is used to symbolize choice between possible values):
*	[inFilename.mavn] [outFilename]  for reading the program from the .mavn file and outputing MIPS code
*	[inFilename.mavn] [outFilename] [hex/bin] same as above, but also creates temporary zerobytes program in temp directory
*	[inFoldername] [outFilename] [hex/bin] [zerobytes] loads the MAVN program from the zerobytes hex/binary file names in the folder
*		and creates the MIPS output file.
*/
int main(int argc, char* argv[])
{
	try
	{

		MyFileHandler fileHandler(argc, argv);

		bool retVal = false;

		LexicalAnalysis lex;
		SymbolTable symTab;
		LivenessAnalysis livenessAnalysis;
		ResourceAllocation resourceAllocation;
		
		if (argc == 3 || argc == 4)
		{
			if (!lex.readInputFile(fileHandler.getInFilepath()))
				throw runtime_error("\nException! Failed to open input file!\n");
		}
		else if (argc == 5) {
			if (!lex.readInputFileName(fileHandler.createProgramFromFilenames()))
				throw runtime_error("\nException! Failed to open input folder!\n");
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

		SyntaxAnalysis syntaxAnalysis(lex);
		std::cout << "\nPerforming syntax analysis..." << endl;
		symTab.accept(syntaxAnalysis);
		std::cout << "Syntax analysis finished successfully!" << endl;

		while (true) {
			std::cout << "\nConnecting the instructions..." << endl;
			symTab.connectInstructions();
			std::cout << "Instructions connection successfull. Successor and predecessor sets were succesfully formed." << endl;
			//symTab.printInstructions();

			std::cout << "Doing Liveness analysis..." << endl;
			symTab.accept(livenessAnalysis);
			std::cout << "Liveness analysis finished successfully! In and out sets were formed!" << endl;
			//symTab.printInstructions();

			try
			{
				std::cout << "Doing Resource allocation..." << endl;
				symTab.accept(resourceAllocation);
				std::cout << "Resource allocation finished successfully! Registers were successfully allocated!\n" << endl;
				symTab.printInstructions();
				symTab.getInterferenceGraph().printIGMatrix();
				// if no spill happens then the MIPS output file is created
				std::cout << "\nCreating MIPS output file..." << endl;
				symTab.accept(fileHandler);
				std::cout << "\nOutput file with the MIPS code successfully created!" << endl;
				break; // if no spills happen
			}
			catch (NoMoreSpillsPossible e) {
				std::cout << e.what() << endl;
				std::cout << "Output was not created! Number of registers="<<__REG_NUMBER__ << " was too small!" << endl;
				symTab.printInstructions();
				break;
			}
			catch (runtime_error e)
			{
				std::cout << e.what() << endl;
				continue;
			}
		}
	}
	catch (runtime_error e)
	{
		std::cout << e.what() << endl;
		return 1;
	}
	system("pause");
	return 0;
}
