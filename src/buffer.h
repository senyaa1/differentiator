#pragma once

#include <stdlib.h>

typedef struct buf_writer
{
	char* buf;
	size_t buf_len;
	size_t cursor;
} buf_writer_t;

void bufcpy(buf_writer_t* writer, const char* string);
