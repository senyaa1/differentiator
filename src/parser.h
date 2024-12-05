#pragma once

#include "lexer.h"

#define OPERATIONS	\
	X(ADD)	\
	X(SUB)	\
	X(MUL)	\
	X(DIV)	\
	X(POW)	\
	X(LOG)	\
	X(SQRT)	\
	X(SIN)	\
	X(COS)	\
	X(TAN)	\
	X(CTAN)	\
	X(ARCSIN)	\
	X(ARCCOS)	\
	X(ARCTG)	\
	X(ARCCTG)	\
	X(SH	)	\
	X(CH	)	\
	X(TH	)	\
	X(CTH)		\
	X(ARCSH)	\
	X(ARCCH)	\
	X(ARCTH)	\
	X(ARCCTH)	\


#define X(NAME) NAME,
typedef enum MATH_FUNCS
{
    INVALID = 0,
    OPERATIONS
} math_func_t;
#undef X

typedef enum DIFF_NODE_TYPE : char
{
	NODE_INVALID 	= 0,
	NODE_NUMBER	= 1,
	NODE_FUNCTION	= 2,
	NODE_VARIABLE	= 3,
} diff_node_type_t;

typedef struct diff_node diff_node_t;

struct diff_node
{
	diff_node_type_t type;
	union 
	{
		math_func_t op_type;
		double number;
		char* variable;
	} value;
	diff_node_t* left;
	diff_node_t* right;
};

diff_node_t* parse_expression(lexer_t *lexer);
char* match_operation(math_func_t op);
