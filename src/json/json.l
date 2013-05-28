%option c++
%option noyywrap

%{
#define  IN_LEXER
#include "json.h"
#include "json.tab.hh"
%}

DIGIT           [0-9]
DIGIT1          [1-9]
INTNUM          {DIGIT1}{DIGIT}*
FRACT           "."{DIGIT}+
FLOAT           ({INTNUM}|0){FRACT}?
EXP             [eE][+-]?{DIGIT}+
NUMBER          -?{FLOAT}{EXP}?

UNICODE         \\u[A-Fa-f0-9]{4}
ESCAPECHAR      \\["\\/bfnrt]
CHAR            [^"\\]|{ESCAPECHAR}|{UNICODE}
STRING          \"{CHAR}*\"

WHITESPACE      [ \t\n]


%%

\{              {LOG("LEX({)");     return '{';}
\}              {LOG("LEX(})");     return '}';}
\[              {LOG("LEX([)");     return '[';}
\]              {LOG("LEX(])");     return ']';}
,               {LOG("LEX(,)");     return ',';}
:               {LOG("LEX(:)");     return ':';}
true            {LOG("LEX(true)");  return yy::JsonParser::token::JSON_TRUE;}
false           {LOG("LEX(false)"); return yy::JsonParser::token::JSON_FALSE;}
null            {LOG("LEX(null)");  return yy::JsonParser::token::JSON_NULL;}
{STRING}        {LOG("LEX(String)");return yy::JsonParser::token::JSON_STRING;}
{NUMBER}        {LOG("LEX(Number)");return yy::JsonParser::token::JSON_NUMBER;}

{WHITESPACE}    {/*IGNORE*/}

%%
