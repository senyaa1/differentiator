#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "parser.h"
#include "tex.h"

static void ensure_allocated(buf_writer_t* writer, size_t n)
{
	if(writer->cursor + n >= writer->buf_len - 1)
	{
		writer->buf_len *= 2;
		writer->buf = (char*)realloc(writer->buf, writer->buf_len * sizeof(char));
	}
}

void bufcpy(buf_writer_t* writer, const char* string)
{
	size_t len = strlen(string);
	ensure_allocated(writer, len);

	memcpy(writer->buf + writer->cursor, string, len);
	writer->cursor += len;
}

static void bufncpy(buf_writer_t* writer, const char* string, size_t len)
{
	ensure_allocated(writer, len);
	memcpy(writer->buf + writer->cursor, string, len);
	writer->cursor += len;
}


static diff_node_t* tex_dump_recursive(buf_writer_t* writer, diff_node_t* node)
{
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
				case ADD:
					tex_dump_recursive(writer, node->left);
					bufcpy(writer, " + ");
					tex_dump_recursive(writer, node->right);
					break;
				case SUB:
					tex_dump_recursive(writer, node->left);
					bufcpy(writer, " - ");
					tex_dump_recursive(writer, node->right);
					break;
				case MUL:
					tex_dump_recursive(writer, node->left);
					bufcpy(writer, " \\cdot ");
					tex_dump_recursive(writer, node->right);
					break;
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
				case SIN:
					bufcpy(writer, "\\sin(");
					tex_dump_recursive(writer, node->left);
					bufcpy(writer, ")");
					break;
				case COS:
					bufcpy(writer, "\\cos(");
					tex_dump_recursive(writer, node->left);
					bufcpy(writer, ")");
					break;
				case TG:
					bufcpy(writer, "\\tg(");
					tex_dump_recursive(writer, node->left);
					bufcpy(writer, ")");
					break;
				case CTG:
					bufcpy(writer, "\\ctg(");
					tex_dump_recursive(writer, node->left);
					bufcpy(writer, ")");
					break;
				case LN:
					bufcpy(writer, "\\ln(");
					tex_dump_recursive(writer, node->left);
					bufcpy(writer, ")");
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
