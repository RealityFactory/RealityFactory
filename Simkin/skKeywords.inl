/* C code produced by gperf version 3.0.1 */
/* Command-line: gperf -a -c -t -p -S1 -C -E -H keyword_hash skKeywords.gperf  */
/* Computed positions: -k'1-2' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

#line 1 "skKeywords.gperf"

#include "skString.h"
#line 4 "skKeywords.gperf"
struct keyword { const char * name; int value; };
/* maximum key range = 46, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
keyword_hash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 25, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 20, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48,  5,  0, 10,
      30,  5,  5, 20,  0, 30, 48, 48,  0, 48,
       0, 10, 48, 48,  5,  0,  0, 48, 48,  0,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48
    };
  return len + asso_values[(unsigned char)str[1]] + asso_values[(unsigned char)str[0]];
}

#ifdef __GNUC__
__inline
#endif
const struct keyword *
in_word_set (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 21,
      MIN_WORD_LENGTH = 2,
      MAX_WORD_LENGTH = 7,
      MIN_HASH_VALUE = 2,
      MAX_HASH_VALUE = 47
    };

  static const struct keyword wordlist[] =
    {
#line 17 "skKeywords.gperf"
      {"lt",	L_LT},
#line 25 "skKeywords.gperf"
      {"step",	L_STEP},
#line 10 "skKeywords.gperf"
      {"while",	L_WHILE},
#line 19 "skKeywords.gperf"
      {"switch",L_SWITCH},
#line 22 "skKeywords.gperf"
      {"le",	L_LE},
#line 14 "skKeywords.gperf"
      {"and",	L_AND},
#line 12 "skKeywords.gperf"
      {"else",	L_ELSE},
#line 26 "skKeywords.gperf"
      {"break",	L_BREAK},
#line 24 "skKeywords.gperf"
      {"to",	L_TO},
#line 15 "skKeywords.gperf"
      {"not",	L_NOT},
#line 7 "skKeywords.gperf"
      {"each",	L_EACH},
#line 16 "skKeywords.gperf"
      {"return",L_RETURN},
#line 13 "skKeywords.gperf"
      {"or",	L_OR},
#line 9 "skKeywords.gperf"
      {"for",	L_FOR},
#line 20 "skKeywords.gperf"
      {"case",	L_CASE},
#line 18 "skKeywords.gperf"
      {"gt",	L_GT},
#line 23 "skKeywords.gperf"
      {"ge",	L_GE},
#line 8 "skKeywords.gperf"
      {"in",	L_IN},
#line 11 "skKeywords.gperf"
      {"if",	L_IF},
#line 21 "skKeywords.gperf"
      {"default",L_DEFAULT},
#line 6 "skKeywords.gperf"
      {"!=",	L_NEQ}
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
              case 5:
                resword = &wordlist[4];
                goto compare;
              case 6:
                resword = &wordlist[5];
                goto compare;
              case 7:
                resword = &wordlist[6];
                goto compare;
              case 8:
                resword = &wordlist[7];
                goto compare;
              case 10:
                resword = &wordlist[8];
                goto compare;
              case 11:
                resword = &wordlist[9];
                goto compare;
              case 12:
                resword = &wordlist[10];
                goto compare;
              case 14:
                resword = &wordlist[11];
                goto compare;
              case 15:
                resword = &wordlist[12];
                goto compare;
              case 16:
                resword = &wordlist[13];
                goto compare;
              case 17:
                resword = &wordlist[14];
                goto compare;
              case 20:
                resword = &wordlist[15];
                goto compare;
              case 25:
                resword = &wordlist[16];
                goto compare;
              case 30:
                resword = &wordlist[17];
                goto compare;
              case 35:
                resword = &wordlist[18];
                goto compare;
              case 40:
                resword = &wordlist[19];
                goto compare;
              case 45:
                resword = &wordlist[20];
                goto compare;
            }
          return 0;
        compare:
          {
            register const char *s = resword->name;

            if (*str == *s && !strncmp (str + 1, s + 1, len - 1) && s[len] == '\0')
              return resword;
          }
        }
    }
  return 0;
}
