#pragma once

#include "Token.h"
#include "LexicalAnalysis.h"
#include "IR.h"
#include <string>
#include "SymbolTable.h"
#include <list>

/**
* Class that will perform syntax analysis of the MAVN code
*/
class SyntaxAnalysis:public Visitor {
public:
	/**
	* Constructor that takes areference to lexical analysis module
	* @param lex reference to a lexer that will be used to get tokens
	*/
	SyntaxAnalysis(LexicalAnalysis& lex);

	/**
	* Visitor class has pure virtual function visit that has to be implemented.
	* Function visit is called by the class that this Visitor visits
	*/
	void visit(SymbolTable& symTab)override;
	/**
	* Method that will do the syntax analysis
	* @returns true if syntax analysis was successfull
	*/
	bool Do(SymbolTable & symTab);
private:

	/**
	* Position of the line in the code. Usefull for error reporting
	*/
	int linePos;

	/**
	* Starting nonterminal symbol Q
	*/
	void Q(SymbolTable & symTab);

	/**
	* Nonterminal symbol S
	* Variables,labels and functions will be created here
	*/
	void S(SymbolTable & symTab);

	/**
	* Nonterminal symbol L
	*/
	void L(SymbolTable & symTab);

	/**
	* Nonterminal symbol E
	* Instructions will be created here
	*/
	void E(SymbolTable & symTab);

	/**
	* Token that is currently processed
	*/
	Token currTok;

	/**
	* A reference to the lexer that will be used to get tokens
	*/
	LexicalAnalysis& lexAnalysis;

	/**
	* Iterator that goes trought the output of the lexical analysis
	*/
	TokenList::iterator tokIter;

	/**
	* Indicator for syntax errors
	*/
	bool err;

	/**
	* Gets the next token from the token iterator
	*/
	Token getNextToken();

	/**
	* If the current token is type t, consume it
	* if it's not, report syntax error
	* @param t type of token that current token is expected to be
	*/
	void eat(TokenType t);



};