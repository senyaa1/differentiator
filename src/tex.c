#include <stdlib.h>
#include <stdio.h>

#include "parser.h"
#include "tex.h"

static diff_node_t* tex_dump_recursive(buf_writer_t* writer, diff_node_t* node)
{
	#define TEX_OP(OP_NAME, OP)					\
		case OP_NAME:						\
			tex_dump_recursive(writer, node->left);		\
			bufcpy(writer, OP);				\
			tex_dump_recursive(writer, node->right);	\
			break;						\

	#define TEX_FUNC(FUNC_NAME, FUNC)				\
		case FUNC_NAME:						\
			bufcpy(writer, FUNC "(");			\
			tex_dump_recursive(writer, node->left);	\
			bufcpy(writer, ")");				\
			break;						\

	if(!node) return 0;

	char* num_str = 0;
	switch(node->type)
	{
		case NODE_VARIABLE:
			bufcpy(writer, node->value.variable);
			break;
		case NODE_NUMBER:
			asprintf(&num_str, "%g", node->value.number);
			bufcpy(writer, num_str);
			break;
		case NODE_FUNCTION:
			switch(node->value.op_type)
			{
				TEX_OP(ADD, " + ")
				TEX_OP(SUB, " - ")
				TEX_OP(MUL, " \\cdot ")
				TEX_FUNC(SIN, "\\sin")
				TEX_FUNC(COS, "\\cos")
				TEX_FUNC(TG, "\\tg")
				TEX_FUNC(CTG, "\\ctg")
				TEX_FUNC(LN, "\\ln")

				case DIV:
					bufcpy(writer, "\\frac{");
					tex_dump_recursive(writer, node->left);
					bufcpy(writer, "}{");
					tex_dump_recursive(writer, node->right);
					bufcpy(writer, "}");
					break;
				case POW:
					bufcpy(writer, "{");
					tex_dump_recursive(writer, node->left);
					bufcpy(writer, "}^{");
					tex_dump_recursive(writer, node->right);
					bufcpy(writer, "}");
					break;
				case LOG:
					bufcpy(writer, "\\log_(");
					tex_dump_recursive(writer, node->left);
					bufcpy(writer, "}(");
					tex_dump_recursive(writer, node->right);
					bufcpy(writer, "}");
					break;
				case SQRT:
					bufcpy(writer, "\\sqrt{");
					tex_dump_recursive(writer, node->left);
					bufcpy(writer, "}");
					break;
			}
			break;
		default:
			return 0;
	}
	
	return 0;
}

void tex_dump_equation(buf_writer_t* writer, diff_node_t* node, char* preamble)
{
	bufcpy(writer, "\n\\begin{equation*}\n");

	if(preamble) bufcpy(writer, preamble);

	tex_dump_recursive(writer, node);
	bufcpy(writer, "\n\\end{equation*}\n");
}

buf_writer_t tex_init(diff_node_t* tree)
{
	buf_writer_t writer = { writer.buf = (char*)calloc(DEFAULT_TEX_ALLOC, sizeof(char)), .buf_len = DEFAULT_TEX_ALLOC};

	bufcpy(&writer, "\\documentclass[12pt,a4paper]{extreport}\n");
	bufcpy(&writer, "\\input{style}\n");
	bufcpy(&writer, "\\title{<<Самостоятельная работа по подготовке к ВПР 5-го класса>>}\n");
	bufcpy(&writer, "\\begin{document}\n");
	bufcpy(&writer, "\\maketitle\n");
	bufcpy(&writer, "\\pagebreak\n");

	return writer;

}

char* tex_end(buf_writer_t* writer)
{
	bufcpy(writer, "\\end{document}\n");
	writer->buf = (char*)realloc(writer->buf, (writer->cursor + 1)* sizeof(char));
	writer->buf[writer->cursor] = '\x00';
	return writer->buf;
}
