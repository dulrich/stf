#ifndef STF_PARSER_H
#define STF_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct token {
	char* start;
	int len;
} token;


typedef enum cclass {
	cc_none = 0,
	cc_spac = 1 << 0,
	cc_numb = 1 << 1,
	cc_word = 1 << 2,
	cc_oper = 1 << 3,
	cc_sstr = 1 << 4,
	cc_dstr = 1 << 5,
} cclass;


cclass cclass_of(char c);
token* scan(char* s);

#endif // STF_PARSER_H

