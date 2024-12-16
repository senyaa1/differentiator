#include <stdlib.h>
#include <string.h>

#include "graph.h"
#include "lexer.h"
#include "parser.h"
#include "differentiator.h"
#include "tex.h"
#include "fs.h"
#include "plot.h"

int main()
{
	// char* expression = (char*)calloc(INPUT_BUF_SZ, sizeof(char));
	// fgets(expression, INPUT_BUF_SZ, stdin);

	// const char *expression = "((2 + 5) * x * (x - 4) / (300 * x + (100 / (400 / (x - 10000 / x^2 - 5 * x)))) * ((2 + 5) * x * (x - 4) / (300 * x + (100 / (400 / (x - 10000 / x^2 - 5 * x)))) + 1337";
	const char *expression = "sin(5 * x^2) + cos(2 * x) ^ 3";
	
	lexer_t* lexer = lex(expression);
	diff_node_t* tree = parse_expression(lexer);
	// draw_tree(tree, "normal.png");

	buf_writer_t writer = tex_init(tree);
	bufcpy(&writer, "\n\nДавайте продифференцируем данную легчайшую функцию.\n");
	tex_dump_equation(&writer, tree, "f(x) = ");

	bufcpy(&writer, "\n\nПриступаем к дифференцированию.\n");
	diff_node_t* diff_tree = differentiate(tree, &writer);
	tex_dump_equation(&writer, diff_tree, "f'(x) = ");

	// draw_tree(diff_tree, "diffed.png");
	// system("imv diffed.png &");
	
	diff_node_t* optimized_tree = optimize(diff_tree, &writer);
	tex_dump_equation(&writer, diff_tree, "f'_{\\text{упрощённая}} = ");
	// draw_tree(optimized_tree, "optimized.png");

	// system("imv optimized.png &");

	bufcpy(&writer, "\n\nНесложно заметить, что графики выглядят так:\n\n");
	plot_dump(tree, "функция", "blue", "orig_plot.png");
	plot_dump(optimized_tree, "производная", "orange", "optimized_plot.png");

	bufcpy(&writer, "\\begin{minipage}{0.45\\textwidth}\n");
	bufcpy(&writer, "\\centering");
	bufcpy(&writer, "\\includegraphics[width=\\linewidth]{orig_plot.png}");
	bufcpy(&writer, "\\end{minipage}\n");
	bufcpy(&writer, "\\hfill\n");
	bufcpy(&writer, "\\begin{minipage}{0.45\\textwidth}\n");
	bufcpy(&writer, "\\centering");
	bufcpy(&writer, "\\includegraphics[width=\\linewidth]{optimized_plot.png}");
	bufcpy(&writer, "\\end{minipage}\n");

	char* tex = tex_end(&writer);
	write_file("dump.tex", tex, strlen(tex));

	system("latexmk");
	free(writer.buf);

	// free(expression);

	return 0;
}
