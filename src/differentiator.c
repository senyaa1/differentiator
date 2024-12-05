#include <stdio.h>

#include "parser.h"
#include "differentiator.h"

#define D(NODE)	differentiate(NODE)
#define C(NODE)	node_copy(NODE)

static diff_node_t* f(char* func, diff_node_t* left, diff_node_t* right)
{
	diff_node_t* node = node_create_func(func);
	node->left = left;
	node->right = right;

	return node;
}

static diff_node_t* op(char* op, diff_node_t* left, diff_node_t* right)
{
	diff_node_t* node = node_create_op(op);
	node->left = left;
	node->right = right;

	return node;
}

static diff_node_t* diff_function(diff_node_t* node)
{
	math_func_t type = node->value.op_type;

	if(type == ADD)
	{
		return op("+", D(node->left), D(node->right));
	}

	if(type == SUB)
	{
		return op("-", D(node->left), D(node->right));
	}

	if(type == MUL)
	{
		return op("+",	op("*", D(node->left), C(node->right)), 	
				op("*", C(node->left), D(node->right)));
	}

	if(type == DIV)
	{
		return op("/",	
				op("-", 
					op("*", D(node->left), C(node->right)),		
					op("*", C(node->left), D(node->right))
				),
			op("^", C(node->right), node_create_num("2"))
		);
	}
	if(type == SIN)
	{
		return op("*", f("cos", C(node->left), 0), D(node->left));
	}

	if(type == COS)
	{
		
		return op("*",
				op("*", 
					f("sin", C(node->left), 0), D(node->left)
				),
			node_create_num("-1")
	    );
	}

	if(type == TG)
	{
		return op("*",  
				op("/", 
					node_create_num("1"), 
					op("^", f("cos", C(node->left), 0), node_create_num("2"))),
				D(node->left));
	}

	if(type == CTG)
	{
		return op("*",  
				op("/", 
					node_create_num("-1"), 
					op("^", f("sin", C(node->left), 0), node_create_num("2"))),
				D(node->left));
	}

	// if(type == POW)
	// {
	// 	return op(
	// }
	//


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

diff_node_t* optimize(diff_node_t* node)
{
	if(node->type != NODE_FUNCTION)
	{
		return node;
	}
	
	switch(node->value.op_type)
	{
		case POW:
			if(node->right->type == NODE_NUMBER && node->right->value.number == 1)
			{
				return C(node->left);
			}
			break;
		case MUL:
			if(node->right->type == NODE_NUMBER && node->right->value.number == 1)
				return C(node->left);

			if(node->left->type == NODE_NUMBER && node->left->value.number == 1)
				return C(node->right);
			break;
		default:
			break;
	}
	
	return node;
}

#undef D
#undef NODE_FUNC
#undef NODE_OP
