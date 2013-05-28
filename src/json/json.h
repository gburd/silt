#ifndef THORSANVIL_JSON_PARSER_H
#define THORSANVIL_JSON_PARSER_H

#ifndef IN_LEXER
#include <FlexLexer.h>
#endif

int yylex(int*, FlexLexer& lexer);

#ifdef DEBUG_LOG
#include <iostream>
#define LOG(x)      std::cout << x << "\n"
#else
#define LOG(x)      0 /*Empty Statement that will be optimized out*/
#endif

#endif
