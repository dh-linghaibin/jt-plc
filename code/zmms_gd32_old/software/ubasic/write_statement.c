#include <string.h>
#include <ctype.h>
#include <stdlib.h>
/*static const struct keyword_token keywords[] = { { "let", TOKENIZER_LET },
		{"write", TOKENIZER_WRITE },
		{ "print", TOKENIZER_PRINT },
		{ "if",TOKENIZER_IF },
		{ "then", TOKENIZER_THEN },
		{ "else", TOKENIZER_ELSE },
		{ "for", TOKENIZER_FOR },
		{ "to", TOKENIZER_TO },
		{ "next",TOKENIZER_NEXT },
		{ "goto", TOKENIZER_GOTO },
		{ "gosub",TOKENIZER_GOSUB },
		{ "return", TOKENIZER_RETURN },
		{ "call",TOKENIZER_CALL },
		{ "end", TOKENIZER_END },
		{ NULL,TOKENIZER_ERROR } };
*/


void put_my_statement(char* src){
	char *delim = " ";
	char *p=strtok(src, delim);


	while (p != NULL){
//		printf ("%s\n",p);
		p = strtok (NULL, delim);
	    //printf ("%s\n",p);
	}
}


