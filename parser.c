#include "macros.h"
#include "parser.h"

///////////////////
//    GRAMMAR    //
///////////////////
//
// e := t
//   | t '+' e
//   | t '-' e
//
// t := f
//   | f '*' t
//   | f '/' t
//   | '(' e ')'
//
//////////////////




cclass cclass_of(char c) {
	if(c == '\0') return cc_none;
	
	if(c >= '0' && c <= '9') return cc_numb;
	
	if( c == '_'
		|| (c >= 'a' && c <= 'z')
		|| (c >= 'A' && c <= 'Z')
	) return cc_word;
	
	if(IN(c, ' ', '\t', '\r', '\n')) return cc_spac;
	
	if(c == '\'') return cc_sstr;
	
	if(c == '"') return cc_dstr;
	
	return cc_oper;	 
}


#define make_token() do { \
	printf("adding token at [%i:%c]\n", offset, s[offset]); \
	tokens++; \
	if(tokens == max_tokens) { \
		max_tokens *= 2; \
		ts = realloc(ts, sizeof(*ts)*max_tokens); \
	} \
	ts[tokens].len = 1; \
	ts[tokens].start = &s[offset]; \
} while(0)

#define token_error() printf("ERROR: unexpected token '%c' at offset %d [state: %d->%d]\n", s[offset], offset, cc0, cc1); exit(1);


token* scan(char* s) {
	size_t max_tokens = 10;
	size_t tokens = -1;
	token* ts = calloc(10, sizeof(*ts)*max_tokens);
	
	int offset = -1;
	offset++;
	cclass cc0 = cc_none;
	cclass cc1 = cc_none;
	while(s[offset] != '\0') {
		cc1 = cclass_of(s[offset]);
		if(cc1 & cc_none) {
			printf("cc_none at offset %i\n", offset);
			break;
		}
		
		switch(cc0) {
			case cc_none:
				switch(cc1) {
					case cc_spac:
						break;
					default:
						make_token();
				}
				break;
			case cc_spac:
				switch(cc1) {
					case cc_spac:
						break;
					default:
						make_token();
				}
				break;
			case cc_numb:
				switch(cc1) {
					case cc_spac:
						break;
					case cc_numb:
						ts[tokens].len++;
						break;
					case cc_word:
						if(IN(s[offset], 'e', 'E')) {
							ts[tokens].len++;
							cc1 = cc_numb;
						} else {
							token_error();
						}
						break;
					case cc_oper:
						if(s[offset] == '.') {
							ts[tokens].len++;
							cc1 = cc_numb;
						} else if((s[offset] == '-') && (IN(s[offset-1], 'e', 'E'))) {
							ts[tokens].len++;
							cc1 = cc_numb;
						} else {
							make_token();
						}
						break;
					case cc_sstr:
					case cc_dstr:
						token_error();
						break;
					default:
						make_token();
				}
				break;
			case cc_word:
				switch(cc1) {
					case cc_spac:
						break;
					case cc_numb:
					case cc_word:
						ts[tokens].len++;
						break;
					case cc_sstr:
					case cc_dstr:
						token_error();
						break;
					case cc_oper:
					default:
						make_token();
				}
				break;
			case cc_oper:
				switch(cc1) {
					case cc_spac:
						break;
					case cc_numb:
						if(s[offset-1] == '-') {
							ts[tokens].len++;
						} else {
							make_token();
						}
						break;
					case cc_oper:
						switch(ts[tokens].len) {
							case 1:
								switch(s[offset-1]) {
									// = ==
									// * *=
									// % %=
									// ! !=
									// ^ ^=
									case '=':
									case '*':
									case '%':
									case '!':
									case '^':
										if(s[offset] == '=') {
											ts[tokens].len++;
										} else {
											make_token();
										}
										break;
									// + += ++
									case '+':
										if(IN(s[offset], '=', '+')) {
											ts[tokens].len++;
										} else {
											make_token();
										}
										break;
									// - -= -- ->
									case '-':
										if(IN(s[offset], '=', '-', '>')) {
											ts[tokens].len++;
										} else {
											make_token();
										}
										break;
									// / /= //
									case '/':
										if(s[offset] == '=') {
											ts[tokens].len++;
										} else {
											make_token();
										}
										break;
									// > >= >> >>=
									case '>':
										if(IN(s[offset], '=', '>')) {
											ts[tokens].len++;
										} else {
											make_token();
										}
										break;
									// < <= <=> << <<=
									case '<':
										if(IN(s[offset], '=', '<')) {
											ts[tokens].len++;
										} else {
											make_token();
										}
										break;
									// & && &=
									case '&':
										if(IN(s[offset], '=', '&')) {
											ts[tokens].len++;
										} else {
											make_token();
										}
										break;
									// | || |=
									case '|':
										if(IN(s[offset], '=', '|')) {
											ts[tokens].len++;
										} else {
											make_token();
										}
										break;
									case '~':
									case '.':
									case ':':
									case '?':
									case '(':
									case ')':
									case '[':
									case ']':
									case '{':
									case '}':
										make_token();
										break;
									default:
										token_error();
								}
								break;
							case 2:
								// <<= <=> >>=
								switch(s[offset-2]) {
									case '<':
										if(s[offset-1] == '<' && s[offset] == '=') ts[tokens].len++;
										else if(s[offset-1] == '=' && s[offset] == '>') ts[tokens].len++;
										else make_token();
										break;
									case '>':
										if(s[offset-1] == '>' && s[offset] == '=') ts[tokens].len++;
										else make_token();
										break;
									default:
										make_token();
								}
								break;
							default:
								make_token();
						}
						break;
					case cc_word:
					case cc_sstr:
					case cc_dstr:
					default:
						make_token();
				}
				break;
			case cc_sstr:
				if(ts[tokens].len == 1) {
					ts[tokens].len++;
					cc1 = cc_sstr;
				} else {
					if((s[offset-1] == '\'') && (s[offset-2] != '\\')) {
						switch(cc1) {
							case cc_spac:
								break;
							case cc_oper:
							case cc_sstr:
								make_token();
								break;
							default:
								token_error();
						}
					} else {
						ts[tokens].len++;
						cc1 = cc_sstr;
					}
				}
				break;
			case cc_dstr:
				if(ts[tokens].len == 1) {
					ts[tokens].len++;
					cc1 = cc_dstr;
				} else {
					if((s[offset-1] == '"') && (s[offset-2] != '\\')) {
						switch(cc1) {
							case cc_spac:
								break;
							case cc_oper:
							case cc_dstr:
								make_token();
								break;
							default:
								token_error();
						}
					} else {
						ts[tokens].len++;
						cc1 = cc_dstr;
					}
				}
				break;
		}
		
		cc0 = cc1;
		offset++;
	}
	ts[tokens+1].len = 0;
	ts[tokens+1].start = NULL;
	
	return ts;
}


