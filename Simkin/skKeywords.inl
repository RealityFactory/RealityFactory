/* C code produced by gperf version 2.7.2 */
/* Command-line: gperf -a -c -t -p -S1 -C -E -H keyword_hash skKeywords.gperf  */
#include "skString.h"
struct keyword { const Char * name; int value; };
/* maximum key range = 51, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
keyword_hash (const Char * str, unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 30, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53,  0, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53,  5, 53, 10,
      25,  0, 13, 25,  5, 10, 53, 53, 15, 53,
       0,  0, 15, 53,  0,  0, 20, 53, 53,  0,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53
    };
  return len + asso_values[(unsigned char)str[len - 1]] + asso_values[(unsigned char)str[0]];
}

#ifdef __GNUC__
__inline
#endif
const struct keyword *
in_word_set (const Char * str, unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 20,
      MIN_WORD_LENGTH = 2,
      MAX_WORD_LENGTH = 7,
      MIN_HASH_VALUE = 2,
      MAX_HASH_VALUE = 52
    };

  static const struct keyword wordlist[] =
    {
      {skSTR("or"),	L_OR},
      {skSTR("else"),	L_ELSE},
      {skSTR("while"),	L_WHILE},
      {skSTR("return"),L_RETURN},
      {skSTR("each"),	L_EACH},
      {skSTR("switch"),L_SWITCH},
      {skSTR("in"),	L_IN},
      {skSTR("case"),	L_CASE},
      {skSTR("for"),	L_FOR},
      {skSTR("le"),	L_LE},
      {skSTR("step"),	L_STEP},
      {skSTR("to"),	L_TO},
      {skSTR("not"),	L_NOT},
      {skSTR("if"),	L_IF},
      {skSTR("ge"),	L_GE},
      {skSTR("!="),	L_NEQ},
      {skSTR("and"),	L_AND},
      {skSTR("lt"),	L_LT},
      {skSTR("gt"),	L_GT},
      {skSTR("default"),L_DEFAULT}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = keyword_hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= MIN_HASH_VALUE)
        {
          register const struct keyword *resword;

          switch (key - 2)
            {
              case 0:
                resword = &wordlist[0];
                goto compare;
              case 2:
                resword = &wordlist[1];
                goto compare;
              case 3:
                resword = &wordlist[2];
                goto compare;
              case 4:
                resword = &wordlist[3];
                goto compare;
              case 7:
                resword = &wordlist[4];
                goto compare;
              case 9:
                resword = &wordlist[5];
                goto compare;
              case 10:
                resword = &wordlist[6];
                goto compare;
              case 12:
                resword = &wordlist[7];
                goto compare;
              case 14:
                resword = &wordlist[8];
                goto compare;
              case 15:
                resword = &wordlist[9];
                goto compare;
              case 17:
                resword = &wordlist[10];
                goto compare;
              case 20:
                resword = &wordlist[11];
                goto compare;
              case 21:
                resword = &wordlist[12];
                goto compare;
              case 23:
                resword = &wordlist[13];
                goto compare;
              case 25:
                resword = &wordlist[14];
                goto compare;
              case 30:
                resword = &wordlist[15];
                goto compare;
              case 31:
                resword = &wordlist[16];
                goto compare;
              case 35:
                resword = &wordlist[17];
                goto compare;
              case 45:
                resword = &wordlist[18];
                goto compare;
              case 50:
                resword = &wordlist[19];
                goto compare;
            }
          return 0;
        compare:
          {
            register const Char *s = resword->name;

            if (*str == *s && !STRNCMP (str + 1, s + 1, len - 1) && s[len] == '\0')
              return resword;
          }
        }
    }
  return 0;
}
