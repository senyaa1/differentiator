#include <stdio.h>
#include <math.h>

#include "parser.h"
#include "differentiator.h"
#include "tex.h"

#define D(NODE)					differentiate(NODE, writer)
#define C(NODE)					node_copy(NODE)
#define NUM(NUM)				node_create_num_d(NUM)
#define OP(OPERATION, LEFT, RIGHT)		op(#OPERATION, (LEFT), (RIGHT))
#define F(FUNCTION, LEFT, RIGHT)		f(#FUNCTION, (LEFT), (RIGHT))
#define LEFT					node->left
#define RIGHT					node->right


static diff_node_t* f(char* func, diff_node_t* left, diff_node_t* right)
{
	diff_node_t* node = node_create_func(func);
	LEFT = left;
	RIGHT = right;
	return node;
}

static diff_node_t* op(char* op, diff_node_t* left, diff_node_t* right)
{
	diff_node_t* node = node_create_op(op);
	LEFT = left;
	RIGHT = right;
	return node;
}

static diff_node_t* diff_function(diff_node_t* node, buf_writer_t* writer)
{
	math_func_t type = node->value.op_type;

	switch(type)
	{

		case ADD:
			return OP(+, D(LEFT), D(RIGHT));

		case SUB:
			return OP(-, D(LEFT), D(RIGHT));

		case MUL:
			return OP(+,	OP(*, D(LEFT), C(RIGHT)), 	
					OP(*, C(LEFT), D(RIGHT)));

		case DIV:
			return OP(/,	
					OP(-, 
						OP(*, D(LEFT), C(RIGHT)),		
						OP(*, C(LEFT), D(RIGHT))
				),
				OP(^, C(RIGHT), NUM(2))
			);

		case SIN:
			return OP(*, F(cos, C(LEFT), 0), D(LEFT));

		case COS:
			return OP(*,
					OP(*, 
						F(sin, C(LEFT), 0), 
						D(LEFT)
				),
				NUM(-1)
			);

		case TG:
			return OP(*,  
					OP(/, 
						NUM(1), 
						OP(^, F(cos, C(LEFT), 0), 
							NUM(2))),
					D(LEFT));
		case CTG:
			return OP(*,	OP(/, 
					NUM(-1), 
					OP(^, F(sin, C(LEFT), 0),
						NUM(2))),
				D(LEFT));
		case LN:
			return	OP(/,	D(LEFT), 
					C(LEFT));
		case LOG:
			return OP(/,	F(ln, C(LEFT), 0), 
					F(ln, C(RIGHT), 0)
				);
		case POW:
			if(LEFT->type == NODE_VARIABLE && RIGHT->type == NODE_NUMBER)
			{
				return OP(*,	
						C(RIGHT), 
						OP(^, C(LEFT), 
							OP(-, RIGHT, NUM(1))
						)
					);
			}

			return OP(*,	
					C(node),
					OP(+, 
						OP(*,
							D(RIGHT),
							F(ln, LEFT, 0)),
						OP(*, 
							RIGHT,
							OP(/, D(LEFT),
								C(LEFT))
						)
					)
				);
			break;
		case SQRT:
			return OP(/,D(LEFT),
					OP(*,	NUM(2), 
						C(node)
					)
				);
		default:
			return 0;
	}
}

#define NUMCMP(NODE, VAL, ACTION)						\
		if(NODE->type == NODE_NUMBER && NODE->value.number == VAL)	\
		{								\
			(*optimization_cnt)++;					\
			return ACTION;						\
		}								\


static diff_node_t* optimize_recursive(diff_node_t* node, size_t* optimization_cnt)
{
	#define PERFORM_OP(TYPE, OPERATION)		case TYPE:								\
								(*optimization_cnt)++;						\
								return NUM(LEFT->value.number OPERATION RIGHT->value.number);	\

	if(!node) return 0;

	if(node->type != NODE_FUNCTION)
		return node;

	// Trivial (*0, +0, etc)
	switch(node->value.op_type)
	{
		case ADD:
		case SUB:
			NUMCMP(RIGHT, 0, C(LEFT));
			NUMCMP(LEFT, 0, C(RIGHT));
			break;
		case POW:
			NUMCMP(RIGHT, 1, C(LEFT));
			break;
		case MUL:
			NUMCMP(RIGHT, 1, C(LEFT));
			NUMCMP(LEFT, 1, C(RIGHT));

			NUMCMP(RIGHT, 0, NUM(0));
			NUMCMP(LEFT, 0, NUM(0));
			break;
		case DIV:
			NUMCMP(LEFT, 1, C(RIGHT));
			break;
		default:
			break;
	}

	// Calculate constants (e.g. 2 + 2)
	if((LEFT && RIGHT) && LEFT->type == NODE_NUMBER && RIGHT->type == NODE_NUMBER)
	{					
		switch(node->value.op_type)
		{
			PERFORM_OP(ADD, +)
			PERFORM_OP(SUB, -)
			PERFORM_OP(MUL, *)
			PERFORM_OP(DIV, /)
			case POW:
				(*optimization_cnt)++;
				return NUM(pow(LEFT->value.number, RIGHT->value.number));
			default:
				break;
		}
	}

	// Optimize children of the same operations e.g. (mul -> mul)
	if((LEFT && RIGHT) && (
			(LEFT->type == NODE_FUNCTION && node->value.op_type == LEFT->value.op_type && RIGHT->type == NODE_NUMBER)	|| 
			(RIGHT->type == NODE_FUNCTION && node->value.op_type == RIGHT->value.op_type && LEFT->type == NODE_NUMBER))
	      )
	{
		diff_node_t* first_const = 0, *second_const = 0, *func = 0;

		#define TRY_GET_NUM(NODE, CHILD1, CHILD2)			\
		if(NODE->CHILD1 && NODE->CHILD1->type == NODE_NUMBER)		\
		{								\
			second_const = NODE->CHILD1;				\
			func = NODE->CHILD2;					\
		}								\

		if(RIGHT->type == NODE_NUMBER)
		{
			first_const = RIGHT;
			TRY_GET_NUM(LEFT, left, right)
			TRY_GET_NUM(LEFT, right, left)
		}
		else
		{
			first_const = LEFT;
			TRY_GET_NUM(RIGHT, left, right)
			TRY_GET_NUM(RIGHT, right, left)
		}
		#undef TRY_GET_NUM

		if(second_const)
		{
			#define OPT_OP(OPERATION_NAME, OPERATION)										\
				case OPERATION_NAME:												\
					(*optimization_cnt)++;											\
					return OP(OPERATION, C(func), NUM(first_const->value.number OPERATION second_const->value.number));	\

			switch(node->value.op_type)
			{
				OPT_OP(ADD, +)
				OPT_OP(SUB, -)
				OPT_OP(DIV, /)
				OPT_OP(MUL, *)
				default:
					break;
			}
		}
	}


	RIGHT = C(optimize_recursive(RIGHT, optimization_cnt));
	LEFT = C(optimize_recursive(LEFT, optimization_cnt));

	return node;
	#undef PERFORM_OP
}

diff_node_t* differentiate(diff_node_t* node, buf_writer_t* writer)
{
	switch(node->type)
	{
		case NODE_NUMBER:	
			return NUM(0);
		case NODE_FUNCTION:
			return diff_function(node, writer);
		case NODE_VARIABLE:
			return NUM(1);
		default:
			return 0;
	}
}

diff_node_t* optimize(diff_node_t* tree, buf_writer_t* writer)
{
	bufcpy(writer, "Давайте немного упростим данное выражение.\n");

	size_t optimization_cnt = 0;
	do 
	{
		optimization_cnt = 0;
		tree = optimize_recursive(tree, &optimization_cnt);
	} while (optimization_cnt > 0);
  
	return tree;
}

