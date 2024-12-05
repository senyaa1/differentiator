#include <stdio.h>
#include <stdlib.h>

// #include "differentiator.h"
#include "graph.h"

#include "lexer.h"
#include "parser.h"
#include "differentiator.h"

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
	const char *expression = "cos(2 * x)";	

	lexer_t* lexer = lex(expression);
	diff_node_t* tree = parse_expression(lexer);
	draw_tree(tree, "normal.png");

	diff_node_t* diff_tree = differentiate(tree);
	diff_node_t* optimized_tree = optimize(diff_tree);

	draw_tree(diff_tree, "diffed.png");
	draw_tree(optimized_tree, "optimized.png");
	system("imv normal.png & ");
	system("imv diffed.png &");
	system("imv optimized.png");

	return 0;
}
