#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"
#include "lexer.h"
#include "parser.h"
#include "differentiator.h"
#include "tex.h"
#include "fs.h"

/* TODO:
 * tex dump
 * funny tex dump
*/

const size_t INPUT_BUF_SZ = 1024;

int main()
{
	// char* expression = (char*)calloc(INPUT_BUF_SZ, sizeof(char));
	// fgets(expression, INPUT_BUF_SZ, stdin);
	const char *expression = "((2 + 5) * x * (x - 4) / (300 * x + (100 / (400 / (x - 10000 / x^2 - 5 * x)))) * ((2 + 5) * x * (x - 4) / (300 * x + (100 / (400 / (x - 10000 / x^2 - 5 * x)))) + 1337";
	
	lexer_t* lexer = lex(expression);
	diff_node_t* tree = parse_expression(lexer);
	draw_tree(tree, "normal.png");

	buf_writer_t writer = tex_init(tree);
	bufcpy(&writer, "\n\nПриветствую всех, давайте продифференцируем данную легчайшую функцию.\n");
	tex_dump_equation(&writer, tree);

	bufcpy(&writer, "\n\nПриступаем к дифференцированию.\n");
	diff_node_t* diff_tree = differentiate(tree, &writer);
	tex_dump_equation(&writer, diff_tree);
	// draw_tree(diff_tree, "diffed.png");

	diff_node_t* optimized_tree = optimize(diff_tree, &writer);
	tex_dump_equation(&writer, diff_tree);
	// draw_tree(diff_tree, "diffed.png");
	// draw_tree(optimized_tree, "optimized.png");
	

	char* tex = tex_end(&writer);
	write_file("dump.tex", tex, strlen(tex));

	system("latexmk");
	system("zathura dump.pdf");

	free(writer.buf);
	// free(expression);
	return 0;
}
