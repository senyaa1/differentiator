#pragma once

#include "parser.h"

static const size_t DEFAULT_TEX_ALLOC = 256;

typedef struct buf_writer
{
	char* buf;
	size_t buf_len;
	size_t cursor;
} buf_writer_t;


char* tex_dump(diff_node_t* tree);
