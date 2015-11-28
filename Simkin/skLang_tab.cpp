
/*  A Bison parser, made from skLang.y
 by  GNU Bison version 1.25
  */

#define YYBISON 1  /* Identify Bison output.  */

#define YYLSP_NEEDED

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


#include "skParser.h"
#include <stdlib.h>


#define YYPARSE_PARAM parser
#define YYINTERP ((skParser*)YYPARSE_PARAM)
#define YYLEX_PARAM parser

#ifdef EXECUTE_PARSENODES
  typedef skString * ID_REFERENCE;
#else
  typedef int ID_REFERENCE;
#endif

#define SAVE(t) YYINTERP->addParseNode(t);
#define SETTOPNODE(t) YYINTERP->setTopNode(t);
#define REMOVE_STRING(s) YYINTERP->removeTempString(s);

  //#define _DEBUGPARSER
#ifdef _DEBUGPARSER
#include <stdio.h>
#define PDB(t) printf("%s\n",t);
#define PDBN(t,n) printf("%s - %s\n",t,(const char *)n);
#else
#define PDB(t) 
#define PDBN(t,n) 
#endif

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

int yylex(YYSTYPE * lvalp,void * yylloc, void* context);
void real_yyerror(Char * msg, void* context);
#define yyerror(x) real_yyerror(x, YYPARSE_PARAM)

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

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		168
#define	YYFLAG		-32768
#define	YYNTBASE	50

#define YYTRANSLATE(x) ((unsigned)(x) <= 284 ? yytranslate[x] : 68)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,    32,     2,    38,    35,     2,    43,
    44,    36,    33,    45,    34,    41,    37,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    39,    48,    30,
    29,    31,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    40,     2,    49,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    46,     2,    47,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    42
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     4,     7,     9,    13,    18,    20,    24,    25,
    27,    29,    32,    33,    35,    37,    41,    42,    44,    46,
    49,    53,    59,    67,    73,    81,    91,    98,   106,   114,
   124,   128,   131,   134,   139,   146,   149,   151,   156,   160,
   162,   167,   171,   173,   178,   183,   191,   193,   197,   201,
   203,   207,   209,   211,   213,   215,   218,   222,   226,   230,
   234,   238,   242,   247,   251,   255,   259,   264,   268,   272,
   276,   280,   284,   288,   291,   295,   299
};

static const short yyrhs[] = {    52,
     0,    51,     0,    29,    67,     0,    54,     0,    43,    44,
    66,     0,    43,    53,    44,    66,     0,     6,     0,    53,
    45,     6,     0,     0,    55,     0,    61,     0,    55,    61,
     0,     0,    57,     0,    67,     0,    57,    45,    67,     0,
     0,    59,     0,    60,     0,    59,    60,     0,    20,    67,
    66,     0,    11,    43,    67,    44,    66,     0,    11,    43,
    67,    44,    66,    12,    66,     0,    13,    43,    67,    44,
    66,     0,    18,    43,    67,    44,    46,    58,    47,     0,
    18,    43,    67,    44,    46,    58,    19,    66,    47,     0,
    21,    22,     6,    23,    67,    66,     0,    21,    22,     6,
     6,    23,    67,    66,     0,    21,     6,    29,    67,    24,
    67,    66,     0,    21,     6,    29,    67,    24,    67,    25,
    67,    66,     0,    26,    67,    48,     0,    26,    48,     0,
    27,    48,     0,    63,    29,    67,    48,     0,    63,    39,
     6,    29,    67,    48,     0,    62,    48,     0,    48,     0,
     6,    43,    56,    44,     0,    64,    41,    62,     0,     6,
     0,     6,    40,    67,    49,     0,    64,    41,    63,     0,
     6,     0,     6,    43,    56,    44,     0,     6,    40,    67,
    49,     0,     6,    43,    56,    44,    40,    67,    49,     0,
    64,     0,    64,    41,    65,     0,    46,    54,    47,     0,
    65,     0,    65,    39,     6,     0,     7,     0,     3,     0,
     4,     0,     5,     0,    10,    67,     0,    67,     9,    67,
     0,    67,     8,    67,     0,    67,    34,    67,     0,    67,
    17,    67,     0,    67,    30,    67,     0,    67,    16,    67,
     0,    67,    30,    29,    67,     0,    67,    15,    67,     0,
    67,    31,    67,     0,    67,    14,    67,     0,    67,    31,
    29,    67,     0,    67,    29,    67,     0,    67,    28,    67,
     0,    67,    33,    67,     0,    67,    37,    67,     0,    67,
    36,    67,     0,    67,    38,    67,     0,    34,    67,     0,
    67,    32,    67,     0,    67,    35,    67,     0,    43,    67,
    44,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   109,   110,   112,   122,   127,   132,   139,   145,   152,   157,
   160,   166,   172,   176,   180,   185,   189,   193,   197,   203,
   208,   212,   217,   222,   227,   232,   237,   242,   247,   252,
   257,   262,   267,   272,   277,   282,   287,   293,   299,   305,
   311,   317,   322,   326,   330,   334,   339,   344,   351,   356,
   362,   367,   374,   379,   384,   391,   396,   401,   406,   411,
   416,   421,   426,   431,   436,   441,   446,   451,   456,   461,
   466,   471,   476,   481,   486,   491,   496
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","L_CHARACTER",
"L_INTEGER","L_FLOAT","L_ID","L_STRING","L_OR","L_AND","L_NOT","L_IF","L_ELSE",
"L_WHILE","L_GE","L_GT","L_LE","L_LT","L_SWITCH","L_DEFAULT","L_CASE","L_FOR",
"L_EACH","L_IN","L_TO","L_STEP","L_RETURN","L_BREAK","L_NEQ","'='","'<'","'>'",
"'#'","'+'","'-'","'&'","'*'","'/'","'%'","':'","'['","'.'","UNARY","'('","')'",
"','","'{'","'}'","';'","']'","start","start_expr","method_defn","param_list",
"stat_list","nonnull_stat_list","expr_list","nonnull_expr_list","case_list",
"nonnull_case_list","case_stat","stat","id_list_fn","id_list_id","id_or_method",
"id_list","compound_stat","expr", NULL
};
#endif

static const short yyr1[] = {     0,
    50,    50,    51,    52,    52,    52,    53,    53,    54,    54,
    55,    55,    56,    56,    57,    57,    58,    58,    59,    59,
    60,    61,    61,    61,    61,    61,    61,    61,    61,    61,
    61,    61,    61,    61,    61,    61,    61,    62,    62,    63,
    63,    63,    64,    64,    64,    64,    65,    65,    66,    67,
    67,    67,    67,    67,    67,    67,    67,    67,    67,    67,
    67,    67,    67,    67,    67,    67,    67,    67,    67,    67,
    67,    67,    67,    67,    67,    67,    67
};

static const short yyr2[] = {     0,
     1,     1,     2,     1,     3,     4,     1,     3,     0,     1,
     1,     2,     0,     1,     1,     3,     0,     1,     1,     2,
     3,     5,     7,     5,     7,     9,     6,     7,     7,     9,
     3,     2,     2,     4,     6,     2,     1,     4,     3,     1,
     4,     3,     1,     4,     4,     7,     1,     3,     3,     1,
     3,     1,     1,     1,     1,     2,     3,     3,     3,     3,
     3,     3,     4,     3,     3,     3,     4,     3,     3,     3,
     3,     3,     3,     2,     3,     3,     3
};

static const short yydefact[] = {     9,
    40,     0,     0,     0,     0,     0,     0,     0,     0,    37,
     2,     1,     4,    10,    11,     0,     0,     0,     0,    13,
     0,     0,     0,     0,     0,    53,    54,    55,    43,    52,
     0,     0,     0,    32,    47,    50,     0,    33,     3,     7,
     0,     0,    12,    36,     0,     0,     0,     0,     0,    14,
    15,     0,     0,     0,     0,     0,     0,    13,    56,    74,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    31,     9,     5,     0,     0,     0,     0,    39,    42,    41,
    38,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    77,    48,    51,    58,    57,    66,    64,    62,    60,    69,
    68,     0,    61,     0,    65,    75,    70,    59,    76,    72,
    71,    73,     0,     6,     8,    34,     0,     0,    16,    22,
    24,    17,     0,     0,     0,    45,    44,    63,    67,    49,
     0,     0,     0,     0,     0,    18,    19,     0,     0,    27,
    35,    46,    23,     0,     0,    25,    20,     0,    29,    28,
    21,     0,     0,    26,    30,     0,     0,     0
};

static const short yydefgoto[] = {   166,
    11,    12,    42,    13,    14,    49,    50,   145,   146,   147,
    15,    16,    17,    35,    36,    83,    51
};

static const short yypact[] = {   113,
   -31,   -36,   -23,   -20,     0,    75,   -16,   348,    -3,-32768,
-32768,-32768,-32768,    29,-32768,   -10,   -21,   -13,   348,   348,
   348,   348,   348,    17,    42,-32768,-32768,-32768,   -29,-32768,
   348,   348,   348,-32768,    12,    15,   242,-32768,   585,-32768,
    11,   -14,-32768,-32768,   348,    70,    78,   149,    46,    48,
   585,   430,   461,   492,   348,    -2,   348,   348,-32768,-32768,
   523,    85,    98,   348,   348,   348,   348,   348,   348,   348,
   348,   107,   327,   348,   348,   348,   348,   348,   348,   348,
-32768,    29,-32768,    11,    99,   277,    86,-32768,-32768,    66,
    -4,   348,    11,    11,    74,   554,    93,   348,   180,    81,
-32768,-32768,-32768,   610,   610,    64,    64,    64,    64,    64,
   585,   348,    64,   348,    64,   110,     7,     7,     7,-32768,
-32768,-32768,    82,-32768,-32768,-32768,   348,   348,   585,   118,
-32768,   112,   348,   348,   397,-32768,    95,   585,   585,-32768,
   312,   211,    11,   348,   -18,   112,-32768,   364,   397,-32768,
-32768,-32768,-32768,   397,    11,-32768,-32768,   348,-32768,-32768,
-32768,    90,   397,-32768,-32768,   133,   151,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,    67,-32768,    96,-32768,-32768,-32768,     9,
   139,   115,   120,     5,    97,   -60,    -6
};


#define	YYLAST		648


static const short yytable[] = {    37,
   155,    39,    40,    97,    18,    24,    21,    45,    19,   -43,
    57,    20,    48,    58,    52,    53,    54,    46,    18,    22,
    98,    25,    23,   124,    59,    60,    61,    47,   156,    84,
    85,    38,   130,   131,     1,   128,   -44,    44,    86,     2,
    41,     3,    78,    79,    80,    55,     4,    56,    96,     5,
    99,    18,    62,    63,     6,     7,    82,   104,   105,   106,
   107,   108,   109,   110,   111,   113,   115,   116,   117,   118,
   119,   120,   121,   122,   150,    87,    10,    26,    27,    28,
    29,    30,   153,     1,    31,   129,    18,   159,   160,    91,
    29,   135,    92,   161,   162,    74,    75,    76,    77,    78,
    79,    80,   165,   103,   125,   138,   -45,   139,    32,    26,
    27,    28,    29,    30,   127,   134,    31,    33,     1,   132,
   141,   142,    34,     2,   137,     3,   148,   149,   140,   143,
     4,   144,   167,     5,   128,   112,   164,   154,     6,     7,
    32,     8,    75,    76,    77,    78,    79,    80,   123,    33,
   168,   163,    43,   100,   157,     9,    64,    65,   102,     0,
    10,    88,    66,    67,    68,    69,    89,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    70,    71,    72,    73,
    74,    75,    76,    77,    78,    79,    80,    64,    65,     0,
     0,     0,     0,    66,    67,    68,    69,    90,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    70,    71,    72,
    73,    74,    75,    76,    77,    78,    79,    80,    64,    65,
     0,     0,     0,     0,    66,    67,    68,    69,   136,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    70,    71,
    72,    73,    74,    75,    76,    77,    78,    79,    80,    64,
    65,     0,     0,     0,     0,    66,    67,    68,    69,   152,
     0,     0,     0,     0,     0,     0,     0,     0,     0,    70,
    71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
     0,     0,     0,     0,    64,    65,     0,     0,     0,    81,
    66,    67,    68,    69,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    70,    71,    72,    73,    74,    75,
    76,    77,    78,    79,    80,     0,     0,     0,     0,    64,
    65,     0,     0,     0,   126,    66,    67,    68,    69,    26,
    27,    28,    29,    30,     0,     0,    31,     0,     0,    70,
    71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
    26,    27,    28,    29,    30,   114,     0,    31,     0,   151,
    32,     0,     0,     0,     0,     0,     0,     0,     0,    33,
     0,    64,    65,     0,     0,     0,     0,    66,    67,    68,
    69,    32,     0,     0,     0,     0,     0,     0,   158,     0,
    33,    70,    71,    72,    73,    74,    75,    76,    77,    78,
    79,    80,     0,     0,    64,    65,     0,     0,     0,    82,
    66,    67,    68,    69,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    70,    71,    72,    73,    74,    75,
    76,    77,    78,    79,    80,     0,     0,    64,    65,     0,
     0,     0,    82,    66,    67,    68,    69,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    70,    71,    72,
    73,    74,    75,    76,    77,    78,    79,    80,    64,    65,
     0,     0,     0,    93,    66,    67,    68,    69,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    70,    71,
    72,    73,    74,    75,    76,    77,    78,    79,    80,    64,
    65,     0,     0,     0,    94,    66,    67,    68,    69,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,    70,
    71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
    64,    65,     0,     0,     0,    95,    66,    67,    68,    69,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
    80,    64,    65,     0,     0,     0,   101,    66,    67,    68,
    69,     0,     0,     0,     0,     0,     0,   133,     0,     0,
     0,    70,    71,    72,    73,    74,    75,    76,    77,    78,
    79,    80,    64,    65,     0,     0,     0,     0,    66,    67,
    68,    69,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    70,    71,    72,    73,    74,    75,    76,    77,
    78,    79,    80,    66,    67,    68,    69,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    70,     0,    72,
    73,    74,    75,    76,    77,    78,    79,    80
};

static const short yycheck[] = {     6,
    19,     8,     6,     6,     0,     6,    43,    29,    40,    41,
    40,    43,    19,    43,    21,    22,    23,    39,    14,    43,
    23,    22,    43,    84,    31,    32,    33,    41,    47,    44,
    45,    48,    93,    94,     6,    40,    41,    48,    45,    11,
    44,    13,    36,    37,    38,    29,    18,     6,    55,    21,
    57,    47,    41,    39,    26,    27,    46,    64,    65,    66,
    67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
    77,    78,    79,    80,   135,     6,    48,     3,     4,     5,
     6,     7,   143,     6,    10,    92,    82,   148,   149,    44,
     6,    98,    45,   154,   155,    32,    33,    34,    35,    36,
    37,    38,   163,     6,     6,   112,    41,   114,    34,     3,
     4,     5,     6,     7,    29,    23,    10,    43,     6,    46,
   127,   128,    48,    11,    44,    13,   133,   134,    47,    12,
    18,    20,     0,    21,    40,    29,    47,   144,    26,    27,
    34,    29,    33,    34,    35,    36,    37,    38,    82,    43,
     0,   158,    14,    58,   146,    43,     8,     9,    62,    -1,
    48,    47,    14,    15,    16,    17,    47,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    28,    29,    30,    31,
    32,    33,    34,    35,    36,    37,    38,     8,     9,    -1,
    -1,    -1,    -1,    14,    15,    16,    17,    49,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    28,    29,    30,
    31,    32,    33,    34,    35,    36,    37,    38,     8,     9,
    -1,    -1,    -1,    -1,    14,    15,    16,    17,    49,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    28,    29,
    30,    31,    32,    33,    34,    35,    36,    37,    38,     8,
     9,    -1,    -1,    -1,    -1,    14,    15,    16,    17,    49,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    28,
    29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    -1,    -1,    -1,    -1,     8,     9,    -1,    -1,    -1,    48,
    14,    15,    16,    17,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    28,    29,    30,    31,    32,    33,
    34,    35,    36,    37,    38,    -1,    -1,    -1,    -1,     8,
     9,    -1,    -1,    -1,    48,    14,    15,    16,    17,     3,
     4,     5,     6,     7,    -1,    -1,    10,    -1,    -1,    28,
    29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
     3,     4,     5,     6,     7,    29,    -1,    10,    -1,    48,
    34,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,
    -1,     8,     9,    -1,    -1,    -1,    -1,    14,    15,    16,
    17,    34,    -1,    -1,    -1,    -1,    -1,    -1,    25,    -1,
    43,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    -1,    -1,     8,     9,    -1,    -1,    -1,    46,
    14,    15,    16,    17,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    28,    29,    30,    31,    32,    33,
    34,    35,    36,    37,    38,    -1,    -1,     8,     9,    -1,
    -1,    -1,    46,    14,    15,    16,    17,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    28,    29,    30,
    31,    32,    33,    34,    35,    36,    37,    38,     8,     9,
    -1,    -1,    -1,    44,    14,    15,    16,    17,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    28,    29,
    30,    31,    32,    33,    34,    35,    36,    37,    38,     8,
     9,    -1,    -1,    -1,    44,    14,    15,    16,    17,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    28,
    29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
     8,     9,    -1,    -1,    -1,    44,    14,    15,    16,    17,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
    38,     8,     9,    -1,    -1,    -1,    44,    14,    15,    16,
    17,    -1,    -1,    -1,    -1,    -1,    -1,    24,    -1,    -1,
    -1,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,     8,     9,    -1,    -1,    -1,    -1,    14,    15,
    16,    17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    14,    15,    16,    17,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    28,    -1,    30,
    31,    32,    33,    34,    35,    36,    37,    38
};
#define YYPURE 1

/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */

/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror (skSTR("syntax error: cannot back up")); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#ifdef __SYMBIAN32__
#define YYINITDEPTH 100
#else
#define YYINITDEPTH 200
#endif
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     Char *to;
     Char *from;
     unsigned int count;
{
  register Char *f = from;
  register Char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (Char *to, Char *from, unsigned int count)
{
  register Char *t = to;
  register Char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif



/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror(skSTR("parser stack overflow"));
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((Char *)yyss, (Char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((Char *)yyvs, (Char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((Char *)yyls, (Char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
{ yyval.parseNode=yyvsp[0].parseNode; SETTOPNODE(yyvsp[0].parseNode);;
    break;}
case 2:
{ yyval.parseNode=yyvsp[0].parseNode; SETTOPNODE(yyvsp[0].parseNode);;
    break;}
case 3:
{
#ifdef EXECUTE_PARSENODES
  yyval.parseNode=yyvsp[0].exprNode; 
#else
  yyval.parseNode=skNEW(skCompiledExprNode(yyvsp[0].exprNode)); 
#endif
  SETTOPNODE(yyval.parseNode);
;
    break;}
case 4:
{ 
  PDB("Method_defn");
  yyval.parseNode=skNEW(skMethodDefNode(yylsp[0].first_line,yyvsp[0].statListNode)); 
  SAVE(yyval.parseNode);  
;
    break;}
case 5:
{
  PDB("Method_defn");
  yyval.parseNode=skNEW(skMethodDefNode(yylsp[-2].first_line,0,yyvsp[0].statListNode)); 
  SAVE(yyval.parseNode);  
;
    break;}
case 6:
{
  PDB("Method_defn");
  yyval.parseNode=skNEW(skMethodDefNode(yylsp[-3].first_line,yyvsp[-2].idListNode,yyvsp[0].statListNode)); 
  SAVE(yyval.parseNode);  
;
    break;}
case 7:
{ 
  yyval.idListNode=skNEW(skIdListNode(yylsp[0].first_line)); 
  skIdNode * id=skNEW(skIdNode(yylsp[0].first_line,yyvsp[0].idReference,0,0));
  SAVE(yyval.idListNode); 
  yyval.idListNode->addId(id); 
;
    break;}
case 8:
{ 
  skIdNode * id=skNEW(skIdNode(yylsp[-2].first_line,yyvsp[0].idReference,0,0));
  SAVE(yyval.idListNode); 
  yyvsp[-2].idListNode->addId(id); 
  yyval.idListNode=yyvsp[-2].idListNode; 
;
    break;}
case 9:
{
  yyval.statListNode=skNEW(skStatListNode(yylsp[1].first_line)); 
  PDB("STATLIST");
  SAVE(yyval.statListNode);  
;
    break;}
case 10:
{
  yyval.statListNode=yyvsp[0].statListNode;
;
    break;}
case 11:
{
  PDB("STATLIST");
  yyval.statListNode=skNEW(skStatListNode(yylsp[0].first_line)); 
  SAVE(yyval.statListNode);  
  yyval.statListNode->addStat(yyvsp[0].statNode); 
;
    break;}
case 12:
{	
  PDB("STATLIST");
  yyvsp[-1].statListNode->addStat(yyvsp[0].statNode); 
  yyval.statListNode=yyvsp[-1].statListNode; 
;
    break;}
case 13:
{ 
  yyval.exprListNode=skNEW(skExprListNode(yylsp[1].first_line)); 
  SAVE(yyval.exprListNode); 
;
    break;}
case 14:
{ 
  yyval.exprListNode=yyvsp[0].exprListNode; 
;
    break;}
case 15:
{ 
  yyval.exprListNode=skNEW(skExprListNode(yylsp[0].first_line));
  SAVE(yyval.exprListNode);
  yyval.exprListNode->addExpr(yyvsp[0].exprNode);
;
    break;}
case 16:
{ 
  yyvsp[-2].exprListNode->addExpr(yyvsp[0].exprNode);yyval.exprListNode=yyvsp[-2].exprListNode; 
;
    break;}
case 17:
{ 
  yyval.caseListNode=skNEW(skCaseListNode(yylsp[1].first_line)); 
  SAVE(yyval.caseListNode); 
;
    break;}
case 18:
{ 
  yyval.caseListNode=yyvsp[0].caseListNode; 
;
    break;}
case 19:
{ 
  PDB("Case");
  yyval.caseListNode=skNEW(skCaseListNode(yylsp[0].first_line));
  SAVE(yyval.caseListNode);
  yyval.caseListNode->addCase(yyvsp[0].caseNode);
;
    break;}
case 20:
{ 
  yyvsp[-1].caseListNode->addCase(yyvsp[0].caseNode);
  yyval.caseListNode=yyvsp[-1].caseListNode; 
;
    break;}
case 21:
{ 
  yyval.caseNode=skNEW(skCaseNode(yylsp[-2].first_line,yyvsp[-1].exprNode,yyvsp[0].statListNode)); 
  SAVE(yyval.caseNode); 
;
    break;}
case 22:
{ 	
  PDB("IF");
  yyval.statNode=skNEW(skIfNode(yylsp[-4].first_line,yyvsp[-2].exprNode,yyvsp[0].statListNode,0)); 
  SAVE(yyval.statNode);
;
    break;}
case 23:
{
  PDB("IFELSE");
  yyval.statNode=skNEW(skIfNode(yylsp[-6].first_line,yyvsp[-4].exprNode,yyvsp[-2].statListNode,yyvsp[0].statListNode)); 
  SAVE(yyval.statNode);
;
    break;}
case 24:
{
  PDB("WHILE");
  yyval.statNode=skNEW(skWhileNode(yylsp[-4].first_line,yyvsp[-2].exprNode,yyvsp[0].statListNode)); 
  SAVE(yyval.statNode);
;
    break;}
case 25:
{
  PDB("SWITCH");
  yyval.statNode=skNEW(skSwitchNode(yylsp[-6].first_line,yyvsp[-4].exprNode,yyvsp[-1].caseListNode,0)); 
  SAVE(yyval.statNode);
;
    break;}
case 26:
{
  PDB("SWITCH");
  yyval.statNode=skNEW(skSwitchNode(yylsp[-8].first_line,yyvsp[-6].exprNode,yyvsp[-3].caseListNode,yyvsp[-1].statListNode)); 
  SAVE(yyval.statNode);
;
    break;}
case 27:
{
  PDB("FOREACH");
  yyval.statNode=skNEW(skForEachNode(yylsp[-5].first_line,yyvsp[-3].idReference,yyvsp[-1].exprNode,yyvsp[0].statListNode)); 
  SAVE(yyval.statNode);
;
    break;}
case 28:
{ 	
  PDB("FOREACH_QUALIFIED");
  yyval.statNode=skNEW(skForEachNode(yylsp[-6].first_line,yyvsp[-4].idReference,yyvsp[-3].idReference,yyvsp[-1].exprNode,yyvsp[0].statListNode)); 
  SAVE(yyval.statNode);
;
    break;}
case 29:
{
  PDB("FOR");
  yyval.statNode=skNEW(skForNode(yylsp[-6].first_line,yyvsp[-5].idReference,yyvsp[-3].exprNode,yyvsp[-1].exprNode,yyvsp[0].statListNode)); 
  SAVE(yyval.statNode);
;
    break;}
case 30:
{
  PDB("FOR_STEP");
  yyval.statNode=skNEW(skForNode(yylsp[-8].first_line,yyvsp[-7].idReference,yyvsp[-5].exprNode,yyvsp[-3].exprNode,yyvsp[-1].exprNode,yyvsp[0].statListNode)); 
  SAVE(yyval.statNode);
;
    break;}
case 31:
{   
  PDBN("RETURN",*yyvsp[-2].keyword);
  yyval.statNode=skNEW(skReturnNode(yylsp[-2].first_line,yyvsp[-1].exprNode)); 
  SAVE(yyval.statNode);
;
    break;}
case 32:
{   
  PDB("RETURN");
  yyval.statNode=skNEW(skReturnNode(yylsp[-1].first_line)); 
  SAVE(yyval.statNode);
;
    break;}
case 33:
{   
  PDB("BREAK");
  yyval.statNode=skNEW(skBreakNode(yylsp[-1].first_line)); 
  SAVE(yyval.statNode);
;
    break;}
case 34:
{   
  PDB("ASSIGN");
  yyval.statNode=skNEW(skAssignNode(yylsp[-3].first_line,yyvsp[-3].idListNode,yyvsp[-1].exprNode)); 
  SAVE(yyval.statNode);
;
    break;}
case 35:
{ 
  PDB("ASSIGN");
  yyval.statNode=skNEW(skAssignNode(yylsp[-5].first_line,yyvsp[-5].idListNode,yyvsp[-3].idReference,yyvsp[-1].exprNode)); 
  SAVE(yyval.statNode); 
;
    break;}
case 36:
{ 
  PDB("METHODCALL");
  yyval.statNode=skNEW(skMethodStatNode(yylsp[-1].first_line,yyvsp[-1].idListNode));
  SAVE(yyval.statNode); 
;
    break;}
case 37:
{	
  PDB("NULL");
  yyval.statNode=skNEW(skStatNode(yylsp[0].first_line));
  SAVE(yyval.statNode);
;
    break;}
case 38:
{ 
  yyval.idListNode=skNEW(skIdListNode(yylsp[-3].first_line)); 
  SAVE(yyval.idListNode); 
  skIdNode * id=skNEW(skIdNode(yylsp[-3].first_line,yyvsp[-3].idReference,0,yyvsp[-1].exprListNode));
  yyval.idListNode->addId(id); 
;
    break;}
case 39:
{ 
  yyvsp[0].idListNode->prependId(yyvsp[-2].idNode); 
  yyval.idListNode=yyvsp[0].idListNode; 
;
    break;}
case 40:
{ 
  yyval.idListNode=skNEW(skIdListNode(yylsp[0].first_line)); 
  SAVE(yyval.idListNode); 
  skIdNode * id=skNEW(skIdNode(yylsp[0].first_line,yyvsp[0].idReference,0,0));
  yyval.idListNode->addId(id); 
;
    break;}
case 41:
{ 
  yyval.idListNode=skNEW(skIdListNode(yylsp[-3].first_line)); 
  SAVE(yyval.idListNode); 
  skIdNode * id=skNEW(skIdNode(yylsp[-3].first_line,yyvsp[-3].idReference,yyvsp[-1].exprNode,0));
  yyval.idListNode->addId(id); 
;
    break;}
case 42:
{ 
  yyvsp[0].idListNode->prependId(yyvsp[-2].idNode); 
  yyval.idListNode=yyvsp[0].idListNode; 
;
    break;}
case 43:
{ 
  yyval.idNode=skNEW(skIdNode(yylsp[0].first_line,yyvsp[0].idReference,0,0)); 
  SAVE(yyval.idNode); 
;
    break;}
case 44:
{ 
  yyval.idNode=skNEW(skIdNode(yylsp[-3].first_line,yyvsp[-3].idReference,0,yyvsp[-1].exprListNode)); 
  SAVE(yyval.idNode); 
;
    break;}
case 45:
{ 
  yyval.idNode=skNEW(skIdNode(yylsp[-3].first_line,yyvsp[-3].idReference,yyvsp[-1].exprNode,0)); 
  SAVE(yyval.idNode); 
;
    break;}
case 46:
{ 
  yyval.idNode=skNEW(skIdNode(yylsp[-6].first_line,yyvsp[-6].idReference,yyvsp[-1].exprNode,yyvsp[-4].exprListNode)); 
  SAVE(yyval.idNode); 
;
    break;}
case 47:
{ 
  yyval.idListNode=skNEW(skIdListNode(yylsp[0].first_line)); 
  SAVE(yyval.idListNode); 
  yyval.idListNode->addId(yyvsp[0].idNode); 
;
    break;}
case 48:
{ 
  yyvsp[0].idListNode->prependId(yyvsp[-2].idNode); 
  yyval.idListNode=yyvsp[0].idListNode; 
;
    break;}
case 49:
{	
  PDB("COMPOUNDSTAT");
  yyval.statListNode=yyvsp[-1].statListNode; 
;
    break;}
case 50:
{   
  PDB("ID_EXPR");
  yyval.exprNode=yyvsp[0].idListNode; 
;
    break;}
case 51:
{   
  PDB("ID_EXPR");
  yyvsp[-2].idListNode->setAttribute(yyvsp[0].idReference);
  yyval.exprNode=yyvsp[-2].idListNode;
;
    break;}
case 52:
{ 
  PDBN("STRING_EXPR",*yyvsp[0].string);
  yyval.exprNode=skNEW(skLiteralNode(yylsp[0].first_line,yyvsp[0].string)); 
  // nb don't delete the string as its now owned by the skLiteralNode
  REMOVE_STRING(yyvsp[0].string);
  SAVE(yyval.exprNode);
;
    break;}
case 53:
{
  PDB("CHARACTER_EXPR");
  yyval.exprNode=skNEW(skLiteralNode(yylsp[0].first_line,(Char)yyvsp[0].character)); 
  SAVE(yyval.exprNode);
;
    break;}
case 54:
{   
  PDB("INTEGER_EXPR");
  yyval.exprNode=skNEW(skLiteralNode(yylsp[0].first_line,yyvsp[0].integer)); 
  SAVE(yyval.exprNode);
;
    break;}
case 55:
{
  PDB("FLOAT_EXPR");
#ifdef USE_FLOATING_POINT
  yyval.exprNode=skNEW(skLiteralNode(yylsp[0].first_line,yyvsp[0].floating)); 
  SAVE(yyval.exprNode);
#endif
;
    break;}
case 56:
{   
  PDB("NOT_EXPR");
  yyval.exprNode=skNEW(skOpNode(yylsp[-1].first_line,s_Not,yyvsp[0].exprNode,0)); 
  SAVE(yyval.exprNode);
;
    break;}
case 57:
{   
  PDB("AND_EXPR");
  yyval.exprNode=skNEW(skOpNode(yylsp[-2].first_line,s_And,yyvsp[-2].exprNode,yyvsp[0].exprNode)); 
  SAVE(yyval.exprNode);
;
    break;}
case 58:
{   
  PDB("OR_EXPR");
  yyval.exprNode=skNEW(skOpNode(yylsp[-2].first_line,s_Or,yyvsp[-2].exprNode,yyvsp[0].exprNode)); 
  SAVE(yyval.exprNode);
;
    break;}
case 59:
{
  PDB("SUBTRACT_EXPR");
  yyval.exprNode=skNEW(skOpNode(yylsp[-2].first_line,s_Subtract,yyvsp[-2].exprNode,yyvsp[0].exprNode));
  SAVE(yyval.exprNode);
;
    break;}
case 60:
{   
  PDB("LESS_EXPR");
  yyval.exprNode=skNEW(skOpNode(yylsp[-2].first_line,s_Less,yyvsp[-2].exprNode,yyvsp[0].exprNode)); 
  SAVE(yyval.exprNode);
;
    break;}
case 61:
{   
  PDB("LESS_EXPR");
  yyval.exprNode=skNEW(skOpNode(yylsp[-2].first_line,s_Less,yyvsp[-2].exprNode,yyvsp[0].exprNode)); 
  SAVE(yyval.exprNode);
;
    break;}
case 62:
{   
  PDB("LESSEQUAL_EXPR");
  yyval.exprNode=skNEW(skOpNode(yylsp[-2].first_line,s_LessEqual,yyvsp[-2].exprNode,yyvsp[0].exprNode)); 
  SAVE(yyval.exprNode);
;
    break;}
case 63:
{   
  PDB("LESSEQUAL_EXPR");
  yyval.exprNode=skNEW(skOpNode(yylsp[-3].first_line,s_LessEqual,yyvsp[-3].exprNode,yyvsp[0].exprNode)); 
  SAVE(yyval.exprNode);
;
    break;}
case 64:
{   
  PDB("MORE_EXPR");
  yyval.exprNode=skNEW(skOpNode(yylsp[-2].first_line,s_More,yyvsp[-2].exprNode,yyvsp[0].exprNode)); 
  SAVE(yyval.exprNode);
;
    break;}
case 65:
{   
  PDB("MORE_EXPR");
  yyval.exprNode=skNEW(skOpNode(yylsp[-2].first_line,s_More,yyvsp[-2].exprNode,yyvsp[0].exprNode)); 
  SAVE(yyval.exprNode);
;
    break;}
case 66:
{   
  PDB("MOREEQUAL_EXPR");
  yyval.exprNode=skNEW(skOpNode(yylsp[-2].first_line,s_MoreEqual,yyvsp[-2].exprNode,yyvsp[0].exprNode)); 
  SAVE(yyval.exprNode);
;
    break;}
case 67:
{ 
  PDB("MOREEQUAL_EXPR");
  yyval.exprNode=skNEW(skOpNode(yylsp[-3].first_line,s_MoreEqual,yyvsp[-3].exprNode,yyvsp[0].exprNode)); 
  SAVE(yyval.exprNode);
;
    break;}
case 68:
{   
  PDB("EQUALS_EXPR");
  yyval.exprNode=skNEW(skOpNode(yylsp[-2].first_line,s_Equals,yyvsp[-2].exprNode,yyvsp[0].exprNode)); 
  SAVE(yyval.exprNode);
;
    break;}
case 69:
{   
  PDB("NOTEQUALS_EXPR");
  yyval.exprNode=skNEW(skOpNode(yylsp[-2].first_line,s_NotEquals,yyvsp[-2].exprNode,yyvsp[0].exprNode)); 
  SAVE(yyval.exprNode);
;
    break;}
case 70:
{   
  PDB("PLUS_EXPR");
  yyval.exprNode=skNEW(skOpNode(yylsp[-2].first_line,s_Plus,yyvsp[-2].exprNode,yyvsp[0].exprNode));  
  SAVE(yyval.exprNode);
;
    break;}
case 71:
{   
  PDB("DIVIDE_EXPR");
  yyval.exprNode=skNEW(skOpNode(yylsp[-2].first_line,s_Divide,yyvsp[-2].exprNode,yyvsp[0].exprNode)); 
  SAVE(yyval.exprNode);
;
    break;}
case 72:
{   
  PDB("MULT_EXPR");
  yyval.exprNode=skNEW(skOpNode(yylsp[-2].first_line,s_Mult,yyvsp[-2].exprNode,yyvsp[0].exprNode)); 
  SAVE(yyval.exprNode);
;
    break;}
case 73:
{   
  PDB("MOD_EXPR");
  yyval.exprNode=skNEW(skOpNode(yylsp[-2].first_line,s_Mod,yyvsp[-2].exprNode,yyvsp[0].exprNode)); 
  SAVE(yyval.exprNode);
;
    break;}
case 74:
{   
  PDB("MINUS_EXPR");
  yyval.exprNode=skNEW(skOpNode(yylsp[-1].first_line,s_Minus,yyvsp[0].exprNode,0)); 
  SAVE(yyval.exprNode);
;
    break;}
case 75:
{   
  PDB("CONCAT_EXPR");
  yyval.exprNode=skNEW(skOpNode(yylsp[-2].first_line,s_Concat,yyvsp[-2].exprNode,yyvsp[0].exprNode)); 
  SAVE(yyval.exprNode);
;
    break;}
case 76:
{   
  PDB("CONCAT_EXPR");
  yyval.exprNode=skNEW(skOpNode(yylsp[-2].first_line,s_Concat,yyvsp[-2].exprNode,yyvsp[0].exprNode)); 
  SAVE(yyval.exprNode);
;
    break;}
case 77:
{   
  PDB("SUB_EXPR");
  yyval.exprNode=yyvsp[-1].exprNode; 
;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */


  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  Char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(Char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (Char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      STRCPY(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(Char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			STRCAT(msg, count == 0 ? skSTR(", expecting `") : skSTR(" or `"));
			STRCAT(msg, yytname[x]);
			STRCAT(msg, skSTR("'"));
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror (skSTR("parse error; also virtual memory exceeded"));
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror(skSTR("parse error"));
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
