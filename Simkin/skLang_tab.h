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

#define	L_CHARACTER	257
#define	L_INTEGER	258
#define	L_FLOAT	259
#define	L_ID	260
#define	L_STRING	261
#define	L_OR	262
#define	L_AND	263
#define	L_NOT	264
#define	L_IF	265
#define	L_ELSE	266
#define	L_WHILE	267
#define	L_GE	268
#define	L_GT	269
#define	L_LE	270
#define	L_LT	271
#define	L_SWITCH	272
#define	L_DEFAULT	273
#define	L_CASE	274
#define	L_FOR	275
#define	L_EACH	276
#define	L_IN	277
#define	L_TO	278
#define	L_STEP	279
#define	L_RETURN	280
#define	L_NEQ	281
#define	UNARY	282

