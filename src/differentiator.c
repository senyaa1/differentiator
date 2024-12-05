#include <stdio.h>

#include "parser.h"
#include "differentiator.h"

#define D(NODE)	differentiate(NODE)
#define C(NODE)	node_copy(NODE)

#define NODE_OP(NODE, OP, LEFT, RIGHT)				\
	diff_node_t* NODE = node_create_op((diff_node_t*)(OP));	\
	NODE->left = (LEFT);					\
	NODE->right = (RIGHT);					\


#define NODE_FUNC(NODE, FUNC, LEFT, RIGHT)			\
	diff_node_t* NODE = node_create_func((diff_node_t*)(FUNC));		\
	NODE->left = (LEFT);					\
	NODE->right = (RIGHT);					\
								\

static diff_node_t* diff_function(diff_node_t* node)
{
	math_func_t type = node->value.op_type;

	if(type == ADD)
	{
		NODE_OP(add, "+", D(node->left), D(node->right))
		return add;
	}

	if(type == SUB)
	{

		NODE_OP(sub, "-", D(node->left), D(node->right));
		return sub;
	}

	if(type == MUL)
	{
		NODE_OP(mul1, "*", D(node->left), C(node->right));
		NODE_OP(mul2, "*", C(node->left), D(node->right));
		NODE_OP(final, "+", mul1, mul2);
		return final;
	}

	if(type == DIV)
	{

		NODE_OP(div_mul1, "*", D(node->left), C(node->right));
		NODE_OP(div_mul2, "*", C(node->left), D(node->right));
		NODE_OP(div_sub, "-", div_mul1, div_mul2);
		NODE_OP(div_sqr, "*", C(node->right), C(node->right));
		NODE_OP(div_final, "/", div_sub, div_sqr);
		return div_final;
	}

	if(type == SIN)
	{
		NODE_FUNC(cos, "cos", C(node->left), 0);
		NODE_OP(cos_final, "*", cos, D(node->left));
		return cos_final;
	}

	if(type == COS)
	{

		NODE_FUNC(sin, "sin", C(node->left), 0);
		NODE_OP(sin_final, "*", sin, D(node->left));
		return sin_final;
	}

	return 0;
}

diff_node_t* differentiate(diff_node_t* node)
{
	switch(node->type)
	{
		case NODE_NUMBER:	
			return node_create_num("0");
		case NODE_FUNCTION:
			return diff_function(node);
		case NODE_VARIABLE:
			return node_create_num("1");
		default:
			return 0;
	}
}

#undef D
#undef NODE_FUNC
#undef NODE_OP
