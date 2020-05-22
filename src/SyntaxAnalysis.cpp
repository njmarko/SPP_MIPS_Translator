#include "SyntaxAnalysis.h"

SyntaxAnalysis::SyntaxAnalysis(LexicalAnalysis & lex): tokIter(lexAnalysis.getTokenList().begin()),err(false), lexAnalysis(lex)
{
}

bool SyntaxAnalysis::Do()
{
	currTok = getNextToken();
	Q();
	return !err;
}

void SyntaxAnalysis::Q()
{
	S();
	eat(TokenType::T_SEMI_COL);
	L();
}

void SyntaxAnalysis::S()
{
	switch (currTok.getType())
	{
	case TokenType::T_MEM: // _mem mid num
		eat(TokenType::T_MEM);
		eat(TokenType::T_M_ID);
		eat(TokenType::T_NUM);
		break;
	case TokenType::T_REG: // _reg rid
		eat(TokenType::T_REG);
		eat(TokenType::T_R_ID);
		break;
	case TokenType::T_FUNC: // _func id
		eat(TokenType::T_FUNC);
		eat(TokenType::T_ID);
		break;
	case TokenType::T_ID: // id: E
		eat(TokenType::T_ID);
		eat(TokenType::T_COL);
		E();
		break;
	default: // E 
		E();
		break;
	}
}

void SyntaxAnalysis::L()
{
	switch (currTok.getType())
	{
	case TokenType::T_END_OF_FILE: // there is no need to eat eof token
		//eat(TokenType::T_END_OF_FILE);
		break;
	default:
		Q();
		break;
	}
}

void SyntaxAnalysis::E()
{
	switch (currTok.getType())
	{
	case TokenType::T_ADD: // add rid, rid, rid
		eat(TokenType::T_ADD);
		eat(TokenType::T_R_ID);
		eat(TokenType::T_COMMA);
		eat(TokenType::T_R_ID);
		eat(TokenType::T_COMMA);
		eat(TokenType::T_R_ID);
		break;
	case TokenType::T_ADDI: // addi rid, rid, num
		eat(TokenType::T_ADDI);
		eat(TokenType::T_R_ID);
		eat(TokenType::T_COMMA);
		eat(TokenType::T_R_ID);
		eat(TokenType::T_COMMA);
		eat(TokenType::T_NUM);
		break;
	case TokenType::T_SUB: // sub rid, rid, rid
		eat(TokenType::T_SUB);
		eat(TokenType::T_R_ID);
		eat(TokenType::T_COMMA);
		eat(TokenType::T_R_ID);
		eat(TokenType::T_COMMA);
		eat(TokenType::T_R_ID);
		break;
	case TokenType::T_LA: // la rid, mid
		eat(TokenType::T_LA);
		eat(TokenType::T_R_ID);
		eat(TokenType::T_COMMA);
		eat(TokenType::T_M_ID);
		break;
	case TokenType::T_LW: // lw rid, num(rid)
		eat(TokenType::T_LW);
		eat(TokenType::T_R_ID);
		eat(TokenType::T_COMMA);
		eat(TokenType::T_NUM);
		eat(TokenType::T_L_PARENT);
		eat(TokenType::T_R_ID);
		eat(TokenType::T_R_PARENT);
		break;
	case TokenType::T_LI: // li rid, num
		eat(TokenType::T_LI);
		eat(TokenType::T_R_ID);
		eat(TokenType::T_COMMA);
		eat(TokenType::T_NUM);
		break;
	case TokenType::T_SW: // sw rid, num(rid)
		eat(TokenType::T_SW);
		eat(TokenType::T_R_ID);
		eat(TokenType::T_COMMA);
		eat(TokenType::T_NUM);
		eat(TokenType::T_L_PARENT);
		eat(TokenType::T_R_ID);
		eat(TokenType::T_R_PARENT);
		break;
	case TokenType::T_B: // b id
		eat(TokenType::T_B);
		eat(TokenType::T_ID);
		break;
	case TokenType::T_BLTZ: // bltz rid, id
		eat(TokenType::T_BLTZ);
		eat(TokenType::T_R_ID);
		eat(TokenType::T_COMMA);
		eat(TokenType::T_ID);
		break;
	case TokenType::T_NOP: // nop
		eat(TokenType::T_NOP);
		break;
	default:
		printSyntaxError(currTok);
		err = true;
		break;
	}
}

Token SyntaxAnalysis::getNextToken()
{
	if (lexAnalysis.getTokenList().end() == tokIter) {
		throw std::runtime_error("You reached the end of file");
	}
	return *tokIter++;
}

void SyntaxAnalysis::eat(TokenType t)
{
	if (!err) {
		if (t == currTok.getType())
		{
			//std::cout << currTok.getValue() << std::endl;
			currTok = getNextToken();
		}
		else
		{
			printSyntaxError(currTok);
			err = true;
		}
	}
}

void SyntaxAnalysis::printSyntaxError(Token t)
{
	std::cout << "Syntax error! Unexpected token: " << t.getValue() << std::endl;
}
