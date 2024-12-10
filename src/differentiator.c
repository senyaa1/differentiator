#include <stdio.h>
#include <math.h>

#include "parser.h"
#include "differentiator.h"
#include "tex.h"

#define D(NODE)	differentiate(NODE, writer)
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

static diff_node_t* diff_function(diff_node_t* node, buf_writer_t* writer)
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
			op("^", C(node->right), node_create_num_d(2))
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
			node_create_num_d(-1)
	    );
	}

	if(type == TG)
	{
		return op("*",  
				op("/", 
					node_create_num_d(1), 
					op("^", f("cos", C(node->left), 0), node_create_num_d(2))),
				D(node->left));
	}

	if(type == CTG)
	{
		return op("*",  
				op("/", 
					node_create_num_d(-1), 
					op("^", f("sin", C(node->left), 0), node_create_num_d(2))),
				D(node->left));
	}

	if(type == LN)
	{
		return	op("/", 
				D(node->left), 
				C(node->left));
	}

	if(type == POW)
	{
		if(node->left->type == NODE_VARIABLE)
		{
			return op("*",	
					C(node->right), 
					op("^", C(node->left), 
						op("-", node->right, node_create_num_d(1))
					)
				);
		}
		return op("*",	
				C(node), 
				D(op("*", node->right, f("ln", node->left, 0))));
				
	}


	return 0;
}

static diff_node_t* optimize_recursive(diff_node_t* node, size_t* optimization_cnt)
{
	if(!node) return 0;

	if(node->type != NODE_FUNCTION)
	{
		return node;
	}
	
	switch(node->value.op_type)
	{
		case ADD:
		case SUB:
			if(node->right->type == NODE_NUMBER && node->right->value.number == 0)
			{
				(*optimization_cnt)++;
				return C(node->left);
			}
			if(node->left->type == NODE_NUMBER && node->left->value.number == 0)
			{
				(*optimization_cnt)++;
				return C(node->right);
			}
			break;
			
		case POW:
			if(node->right->type == NODE_NUMBER && node->right->value.number == 1)
			{
				(*optimization_cnt)++;
				return C(node->left);
			}
			break;
		case MUL:
			if(node->right->type == NODE_NUMBER && node->right->value.number == 1)
			{
				(*optimization_cnt)++;
				return C(node->left);
			}

			if(node->left->type == NODE_NUMBER && node->left->value.number == 1)
			{
				(*optimization_cnt)++;
				return C(node->right);
			}

			if(node->right->type == NODE_NUMBER && node->right->value.number == 0)
			{
				(*optimization_cnt)++;
				return node_create_num_d(0);
			}

			if(node->left->type == NODE_NUMBER && node->left->value.number == 0)
			{
				(*optimization_cnt)++;
				return node_create_num_d(0);
			}

			break;
		case DIV:
			if(node->left->type == NODE_NUMBER && node->left->value.number == 1)
			{
				(*optimization_cnt)++;
				return C(node->right);
			}
			break;
		default:
			break;
	}

	if((node->left && node->right) && node->left->type == NODE_NUMBER && node->right->type == NODE_NUMBER)
	{
		switch(node->value.op_type)
		{
			case ADD:
				(*optimization_cnt)++;
				return node_create_num_d(node->left->value.number + node->right->value.number);
			case SUB:
				(*optimization_cnt)++;
				return node_create_num_d(node->left->value.number - node->right->value.number);
			case MUL:
				(*optimization_cnt)++;
				return node_create_num_d(node->left->value.number * node->right->value.number);
			case DIV:
				(*optimization_cnt)++;
				return node_create_num_d(node->left->value.number / node->right->value.number);
			case POW:
				(*optimization_cnt)++;
				return node_create_num_d(pow(node->left->value.number, node->right->value.number));
			default:
				break;
		}

	}

	node->right = C(optimize_recursive(node->right, optimization_cnt));
	node->left = C(optimize_recursive(node->left, optimization_cnt));

	return node;
}

diff_node_t* differentiate(diff_node_t* node, buf_writer_t* writer)
{
	switch(node->type)
	{
		case NODE_NUMBER:	
			return node_create_num_d(0);
		case NODE_FUNCTION:
			return diff_function(node, writer);
		case NODE_VARIABLE:
			return node_create_num_d(1);
		default:
			return 0;
	}
}

diff_node_t* optimize(diff_node_t* tree, buf_writer_t* writer)
{
	bufcpy(writer, "Товарищи, приступаем к следующей части нашего семинара. Давайте немного упростим данное выражение.\n");

	size_t optimization_cnt = 0;
	do 
	{
		optimization_cnt = 0;
		tree = optimize_recursive(tree, &optimization_cnt);
	} while (optimization_cnt > 0);
  
	return tree;
}

