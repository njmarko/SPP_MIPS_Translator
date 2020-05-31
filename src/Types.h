#ifndef __TYPES__
#define __TYPES__

#include "Constants.h"

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <stack>

/**
* Supported token types.
*/
enum TokenType
{
	T_NO_TYPE,

	T_ID,			// abcd...
	T_M_ID,			// m123...
	T_R_ID,			// r123...
	T_NUM,			// 123...
	T_WHITE_SPACE,

	// reserved words
	T_MEM,			// _mem
	T_REG,			// _reg
	T_FUNC,			// _func
	T_ADD,			// add
	T_ADDI,			// addi
	T_SUB,			// sub
	T_LA,			// la
	T_LI,			// li
	T_LW,			// lw
	T_SW,			// sw
	T_BLTZ,			// bltz
	T_B,			// b
	T_NOP,			// nop
	T_MUL,			// mul
	T_XOR,			// xor
	T_NEG,			// neg
	T_BGEZ,			// bgez

	// operators
	T_COMMA,		//,
	T_L_PARENT,		//(
	T_R_PARENT,		//)
	T_COL,			//:
	T_SEMI_COL,		//;

	// utility
	T_COMMENT,
	T_END_OF_FILE,
	T_ERROR,
};


/**
 * Instruction type.
 */
enum InstructionType
{
	I_NO_TYPE = 0,
	I_ADD,
	I_ADDI,
	I_SUB,
	I_LA,
	I_LI,
	I_LW,
	I_SW,
	I_BLTZ,
	I_B,
	I_NOP,
	I_MUL,
	I_XOR,
	I_NEG,
	I_BGEZ,
};

/*
* Converts enum instruction type to the string
* that represents the whole instruction format
* @param instruction type
* @returns string representation of the whole instruction type
*/
std::string instrTypeToWholeInstrStr(InstructionType type);

/*
* Converts enum instruction type to string
* @param instruction type
* @returns string with the name of the type
*/
std::string instrTypeStr(InstructionType type);

/**
 * Reg names. Mips has 32 registers
 */
enum Regs
{
	no_assign = 0,
	t0,
	t1,
	t2,
	t3,
	t4,
	t5,
	t6,
	t7,
	t8,
	t9,
	t10,
	t11,
	t12,
	t13,
	t14,
	t15,
	t16,
	t17,
	t18,
	t19,
	t20,
	t21,
	t22,
	t23,
	t24,
	t25,
	t26,
	t27,
	t28,
	t29,
	t30,
	t31
};

#endif
