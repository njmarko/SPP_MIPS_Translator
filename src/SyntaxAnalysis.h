#pragma once

#include "Token.h"
#include "LexicalAnalysis.h"
#include "IR.h"
#include <string>

/*
* Class that will perform syntax analysis of the MAVN code
*/
class SyntaxAnalysis {
public:
	/*
	* Constructor that takes areference to lexical analysis module
	* @param reference to a lexer that will be used to get tokens
	*/
	SyntaxAnalysis(LexicalAnalysis& lex);

	/*
	* Method that will do the syntax analysis
	* @returns true if syntax analysis was successfull
	*/
	bool Do();
private:

	/*
	* Starting nonterminal symbol Q
	*/
	void Q();

	/*
	* Nonterminal symbol S
	*/
	void S();

	/*
	* Nonterminal symbol L
	*/
	void L();

	/*
	* Nonterminal symbol E
	*/
	void E();

	/*
	* Token that is currently processed
	*/
	Token currTok;

	/*
	* A reference to the lexer that will be used to get tokens
	*/
	LexicalAnalysis& lexAnalysis;

	/*
	* Iterator that goes trought the output of the lexical analysis
	*/
	TokenList::iterator tokIter;

	/*
	* Indicator for syntax errors
	*/
	bool err;

	/*
	* Gets the next token from the token iterator
	*/
	Token getNextToken();

	/*
	* If the current token is type t, consume it
	* if it's not, report syntax error
	* @param type of token that current token is expected to be
	*/
	void eat(TokenType t);

	void printSyntaxError(Token t);
};