#include "parser.h"
#include "differentiator.h"

diff_node_t* differentiate(diff_node_t* diff_node)
{
	switch(diff_node->type)
	{
		case NODE_NUMBER:	
			return node_create_num("0");
		case NODE_FUNCTION:
			switch(diff_node->value.op_type)
			{
				case ADD:
					diff_node_t* add = node_create_op("+");
					add->left = differentiate(diff_node->left);
					add->right = differentiate(diff_node->right);
					return add;
				case SUB:
					diff_node_t* add = node_create_op("+");
					add->left = differentiate(diff_node->left);
					add->right = differentiate(diff_node->right);
					return add;
			}
			break;
		default:
			return 0;
	}
}
