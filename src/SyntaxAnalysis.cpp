#include "SyntaxAnalysis.h"

SyntaxAnalysis::SyntaxAnalysis(LexicalAnalysis & lex, SymbolTable& _symTab): 
	tokIter(lexAnalysis.getTokenList().begin()),err(false), lexAnalysis(lex),symTab(_symTab),
	linePos(0)
{
}

bool SyntaxAnalysis::Do()
{
	currTok = getNextToken();
	Q();
	return !err; //TODO: For now it just return the bool value
}

void SyntaxAnalysis::Q()
{
	S();
	eat(TokenType::T_SEMI_COL);
	L();
}

void SyntaxAnalysis::S()
{
	std::string s_id;
	std::string s_val;
	Variable v;
	switch (currTok.getType())
	{
	case TokenType::T_MEM: // _mem mid num
		eat(TokenType::T_MEM);
		s_id= currTok.getValue();
		eat(TokenType::T_M_ID);
		s_val = currTok.getValue();
		eat(TokenType::T_NUM);
		symTab.addMemVariable(s_id,Variable::MEM_VAR,s_val);
		break;
	case TokenType::T_REG: // _reg rid
		eat(TokenType::T_REG);
		s_id = currTok.getValue();
		eat(TokenType::T_R_ID);
		symTab.addRegVariable(s_id, Variable::REG_VAR);
		break;
	case TokenType::T_FUNC: // _func id
		eat(TokenType::T_FUNC);
		s_id = currTok.getValue();
		eat(TokenType::T_ID);
		symTab.addFunction(s_id);
		break;
	case TokenType::T_ID: // id: E
		s_id = currTok.getValue();
		eat(TokenType::T_ID);
		eat(TokenType::T_COL);
		symTab.addLabel(s_id);
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
	std::list<std::string> params; // name of source variables
	std::list<std::string> dst; // name of destination variables
	switch (currTok.getType())
	{
	case TokenType::T_XOR: // xor rid, rid, rid
		eat(TokenType::T_XOR);
		params.emplace_back(currTok.getValue());
		eat(TokenType::T_R_ID);
		eat(TokenType::T_COMMA);
		params.emplace_back(currTok.getValue());
		eat(TokenType::T_R_ID);
		eat(TokenType::T_COMMA);
		params.emplace_back(currTok.getValue());
		eat(TokenType::T_R_ID);
		symTab.makeInstruction(params, InstructionType::I_XOR);
		break;
	case TokenType::T_MUL: // mul rid, rid, rid
		eat(TokenType::T_MUL);
		params.emplace_back(currTok.getValue());
		eat(TokenType::T_R_ID);
		eat(TokenType::T_COMMA);
		params.emplace_back(currTok.getValue());
		eat(TokenType::T_R_ID);
		eat(TokenType::T_COMMA);
		params.emplace_back(currTok.getValue());
		eat(TokenType::T_R_ID);
		symTab.makeInstruction(params, InstructionType::I_MUL);
		break;
	case TokenType::T_ADD: // add rid, rid, rid
		eat(TokenType::T_ADD);
		params.emplace_back(currTok.getValue());
		eat(TokenType::T_R_ID);
		eat(TokenType::T_COMMA);
		params.emplace_back(currTok.getValue());
		eat(TokenType::T_R_ID);
		eat(TokenType::T_COMMA);
		params.emplace_back(currTok.getValue());
		eat(TokenType::T_R_ID);
		symTab.makeInstruction(params,InstructionType::I_ADD);
		break;
	case TokenType::T_ADDI: // addi rid, rid, num
		eat(TokenType::T_ADDI);
		params.emplace_back(currTok.getValue());
		eat(TokenType::T_R_ID);
		eat(TokenType::T_COMMA);
		params.emplace_back(currTok.getValue());
		eat(TokenType::T_R_ID);
		eat(TokenType::T_COMMA);
		params.emplace_back(currTok.getValue());
		eat(TokenType::T_NUM);
		symTab.makeInstruction(params, InstructionType::I_ADDI);
		break;
	case TokenType::T_SUB: // sub rid, rid, rid
		eat(TokenType::T_SUB);
		params.emplace_back(currTok.getValue());
		eat(TokenType::T_R_ID);
		eat(TokenType::T_COMMA);
		params.emplace_back(currTok.getValue());
		eat(TokenType::T_R_ID);
		eat(TokenType::T_COMMA);
		params.emplace_back(currTok.getValue());
		eat(TokenType::T_R_ID);
		symTab.makeInstruction(params, InstructionType::I_SUB);
		break;
	case TokenType::T_LA: // la rid, mid
		eat(TokenType::T_LA);
		params.emplace_back(currTok.getValue());
		eat(TokenType::T_R_ID);
		eat(TokenType::T_COMMA);
		params.emplace_back(currTok.getValue());
		eat(TokenType::T_M_ID);
		symTab.makeInstruction(params, InstructionType::I_LA);
		break;
	case TokenType::T_LW: // lw rid, num(rid)
		eat(TokenType::T_LW);
		params.emplace_back(currTok.getValue());
		eat(TokenType::T_R_ID);
		eat(TokenType::T_COMMA);
		params.emplace_back(currTok.getValue());
		eat(TokenType::T_NUM);
		eat(TokenType::T_L_PARENT);
		params.emplace_back(currTok.getValue());
		eat(TokenType::T_R_ID);
		eat(TokenType::T_R_PARENT);
		symTab.makeInstruction(params, InstructionType::I_LW);
		break;
	case TokenType::T_LI: // li rid, num
		eat(TokenType::T_LI);
		params.emplace_back(currTok.getValue());
		eat(TokenType::T_R_ID);
		eat(TokenType::T_COMMA);
		params.emplace_back(currTok.getValue());
		eat(TokenType::T_NUM);
		symTab.makeInstruction(params, InstructionType::I_LI);
		break;
	case TokenType::T_SW: // sw rid, num(rid)
		eat(TokenType::T_SW);
		params.emplace_back(currTok.getValue());
		eat(TokenType::T_R_ID);
		eat(TokenType::T_COMMA);
		params.emplace_back(currTok.getValue());
		eat(TokenType::T_NUM);
		eat(TokenType::T_L_PARENT);
		params.emplace_back(currTok.getValue());
		eat(TokenType::T_R_ID);
		eat(TokenType::T_R_PARENT);
		symTab.makeInstruction(params, InstructionType::I_SW);
		break;
	case TokenType::T_B: // b id
		eat(TokenType::T_B);
		params.emplace_back(currTok.getValue());
		eat(TokenType::T_ID);
		symTab.makeInstruction(params, InstructionType::I_B);
		break;
	case TokenType::T_BLTZ: // bltz rid, id
		eat(TokenType::T_BLTZ);
		params.emplace_back(currTok.getValue());
		eat(TokenType::T_R_ID);
		eat(TokenType::T_COMMA);
		params.emplace_back(currTok.getValue());
		eat(TokenType::T_ID);
		symTab.makeInstruction(params, InstructionType::I_BLTZ);
		break;
	case TokenType::T_NOP: // nop
		eat(TokenType::T_NOP);
		symTab.makeInstruction(params, InstructionType::I_NOP);
		break;
	default:
		throw std::runtime_error("Syntax error! Unexpected token: " + currTok.getValue());
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
			throw std::runtime_error("Syntax error! Unexpected token: " + currTok.getValue());
		}
	}
}


