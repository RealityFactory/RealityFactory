typedef union{
  Char	character; 
  int	integer; 
  float	floating;
  skStatListNode * statListNode;
  skStatNode * statNode;
  skExprListNode * exprListNode;
  skExprNode * exprNode;
  skCaseListNode * caseListNode;
  skCaseNode * caseNode;
  skIdListNode * idListNode;
  skIdNode * idNode;
  ID_REFERENCE idReference;
  skString * string;
  const Char  * keyword;
  skMethodDefNode * methodDef;
  skParseNode * parseNode;
} YYSTYPE;

#ifndef YYLTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YYLTYPE yyltype
#endif

#define	L_CHARACTER	258
#define	L_INTEGER	259
#define	L_FLOAT	260
#define	L_ID	261
#define	L_STRING	262
#define	L_OR	263
#define	L_AND	264
#define	L_NOT	265
#define	L_IF	266
#define	L_ELSE	267
#define	L_WHILE	268
#define	L_GE	269
#define	L_GT	270
#define	L_LE	271
#define	L_LT	272
#define	L_SWITCH	273
#define	L_DEFAULT	274
#define	L_CASE	275
#define	L_FOR	276
#define	L_EACH	277
#define	L_IN	278
#define	L_TO	279
#define	L_STEP	280
#define	L_RETURN	281
#define	L_BREAK	282
#define	L_NEQ	283
#define	UNARY	284

