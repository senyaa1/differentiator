#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #include "differentiator.h"
#include "graph.h"

#include "lexer.h"
#include "parser.h"
#include "differentiator.h"
#include "tex.h"
#include "fs.h"

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
	// const char *expression = "(2 + 5) * x * (x - 4) / (300 * x + (100 / (400 / (x - 10000 / x^2 - 5 * x)))";
	const char *expression = "((2 + 5) + (2 + 5) + (2 + 5) + (2 + 5) + (2 + 5) + (2 + 5) + (2 + 5) + (2 + 5) + (2 + 5) + (2 + 5)) * x)";
	// const char *expression = "(x^2 + x) ^ 2";
	// const char *expression = "5 * x ^ 4";




	lexer_t* lexer = lex(expression);
	diff_node_t* tree = parse_expression(lexer);
	draw_tree(tree, "normal.png");

	diff_node_t* diff_tree = differentiate(tree);
	draw_tree(diff_tree, "diffed.png");
	diff_node_t* optimized_tree = optimize(diff_tree);

	draw_tree(optimized_tree, "optimized.png");

	char* tex = tex_dump(optimized_tree);

	// printf("%s\n", tex);
	
	write_file("dump.tex", tex, strlen(tex));


	system("latexmk");
	system("zathura dump.pdf");

	// system("imv normal.png & ");
	// system("imv diffed.png &");
	// system("imv optimized.png");

	return 0;
}
