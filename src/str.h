#pragma once

#include <stdlib.h>

typedef struct String {
	size_t len;
	char *buffer;
} String;

String trim_begin(String s);
String trim_end(String s);
String word_tok(String *input);
String line_tok(String *input);
