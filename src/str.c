#include <ctype.h>
#include <string.h>
#include "str.h"

String string_nul(const char *s) {
	return (String){
		.len = strlen(s),
		.buffer = s,
	};
}

String trim_begin(String s) {
	while (s.len && isspace(*s.buffer)) {
		s.buffer++;
		s.len--;
	}
	return s;
}

String trim_end(String s) {
	while (s.len && isspace(s.buffer[s.len - 1])) {
		s.len--;
	}
	return s;
}

String word_tok(String *input) {
	*input = trim_begin(*input);
	String word;
	word.buffer = input->buffer;
	word.len = 0;
	while (word.len < input->len && input->buffer[word.len] != ' ') {
		word.len++;
	}
	input->buffer += word.len;
	input->len -= word.len;
	return word;
}

String line_tok(String *input) {
	size_t i = 0;
	while (i < input->len && input->buffer[i] != '\n') {
		i++;
	}

	String line = {
		.buffer = input->buffer,
		.len = i,
	};

	if (i == input->len) {
		input->buffer += input->len;
		input->len = 0;
	} else {
		input->buffer += i + 1;
		input->len -= i + 1;
	}

	return line;
}

bool string_equal(String s1, String s2) {
	if (s1.len != s2.len) return false;
	return memcmp((char*)s1.buffer, (char*)s2.buffer, s1.len) == 0;
}
