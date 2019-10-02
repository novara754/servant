#pragma once

#include <stdbool.h>
#include <stdlib.h>

typedef struct String {
	size_t len;
	const char *buffer;
} String;

String string_nul(const char *s);
String trim_begin(String s);
String trim_end(String s);
String word_tok(String *input);
String line_tok(String *input);
bool string_equal(String s1, String s2);
