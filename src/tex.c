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

static void bufcpy(buf_writer_t* writer, const char* string)
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


static diff_node_t* tex_dump_recursive(diff_node_t* node, buf_writer_t* writer)
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
					tex_dump_recursive(node->left, writer);
					bufcpy(writer, " + ");
					tex_dump_recursive(node->right, writer);
					break;
				case SUB:
					tex_dump_recursive(node->left, writer);
					bufcpy(writer, " - ");
					tex_dump_recursive(node->right, writer);
					break;
				case MUL:
					tex_dump_recursive(node->left, writer);
					bufcpy(writer, " \\cdot ");
					tex_dump_recursive(node->right, writer);
					break;
				case DIV:
					bufcpy(writer, "\\frac{");
					tex_dump_recursive(node->left, writer);
					bufcpy(writer, "}{");
					tex_dump_recursive(node->right, writer);
					bufcpy(writer, "}");
					break;
				case POW:
					bufcpy(writer, "{");
					tex_dump_recursive(node->left, writer);
					bufcpy(writer, "}^{");
					tex_dump_recursive(node->right, writer);
					bufcpy(writer, "}");
					break;
			}
			break;
		default:
			return 0;
	}
	
	return 0;
}


char* tex_dump(diff_node_t* tree)
{
	buf_writer_t writer = { writer.buf = (char*)calloc(DEFAULT_TEX_ALLOC, sizeof(char)), .buf_len = DEFAULT_TEX_ALLOC};

	bufcpy(&writer, "\\documentclass{article}\n");
	bufcpy(&writer, "\\usepackage{amsmath,breqn}\n");
	bufcpy(&writer, "\\begin{document}\n");
	bufcpy(&writer, "\\begin{dmath*}\n");
	tex_dump_recursive(tree, &writer);
	bufcpy(&writer, "\n\\end{dmath*}\n");
	bufcpy(&writer, "\\end{document}\n");
	
	writer.buf = (char*)realloc(writer.buf, (writer.cursor + 1)* sizeof(char));
	writer.buf[writer.cursor] = '\x00';
	return writer.buf;
}
