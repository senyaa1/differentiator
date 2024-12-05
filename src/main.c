#include <stdio.h>
#include <stdlib.h>

// #include "differentiator.h"
#include "graph.h"

#include "lexer.h"
#include "parser.h"

/* TODO:

 * recursively differentitate
 * tex dump
 * optimize tree
 * funny tex dump

*/

const size_t INPUT_BUF_SZ = 1024;

int main()
{
	// char* expression = (char*)calloc(INPUT_BUF_SZ, sizeof(char));
	// fgets(expression, INPUT_BUF_SZ, stdin);
	const char *expression = "sin(sin(1) + 2) * log(2 + 10 * abc + 1) + 3^2 + arcth(5)";	// TODO: fix log
	lexer_t* lexer = lex(expression);
	diff_node_t* diff_tree = parse_expression(lexer);

	// diff_node_t* tree = build_from_prefix(prefix_input);
	//
	// diff_node_t* diff_tree = differentiate(tree);

	draw_tree(diff_tree, "amogus.png");
	system("imv amogus.png");

	return 0;
}
