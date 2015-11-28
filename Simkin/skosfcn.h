#include <stdlib.h>
extern "C" int read(int handle, void *buf, unsigned len);
void yyerror(Char * msg);
int yylex ( YYSTYPE * lvalp );
extern "C" void *alloca(size_t size);
