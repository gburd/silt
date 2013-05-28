%skeleton "lalr1.cc"
%require "2.1a"
%defines
%define "parser_class_name" "JsonParser"

%{

#include "json.h"
#include <stdexcept>

%}

%parse-param {FlexLexer& lexer}
%lex-param   {FlexLexer& lexer}

%token  JSON_STRING
%token  JSON_NUMBER
%token  JSON_TRUE
%token  JSON_FALSE
%token  JSON_NULL


%%

JsonObject              :   JsonMap                                 {LOG("JsonObject: JsonMap");}
                        |   JsonArray                               {LOG("JsonObject: JsonArray");}

JsonMap                 :   '{' JsonMapValueListOpt '}'             {LOG("JsonMap: { JsonMapValueListOpt }");}
JsonMapValueListOpt     :                                           {LOG("JsonMapValueListOpt: EMPTY");}
                        |   JsonMapValueList                        {LOG("JsonMapValueListOpt: JsonMapValueList");}
JsonMapValueList        :   JsonMapValue                            {LOG("JsonMapValueList: JsonMapValue");}
                        |   JsonMapValueList ',' JsonMapValue       {LOG("JsonMapValueList: JsonMapValueList , JsonMapValue");}
JsonMapValue            :   JSON_STRING ':' JsonValue               {LOG("JsonMapValue: JSON_STRING : JsonValue");}

JsonArray               :   '[' JsonArrayValueListOpt ']'           {LOG("JsonArray: [ JsonArrayValueListOpt ]");}
JsonArrayValueListOpt   :                                           {LOG("JsonArrayValueListOpt: EMPTY");}
                        |   JsonArrayValueList                      {LOG("JsonArrayValueListOpt: JsonArrayValueList");}
JsonArrayValueList      :   JsonValue                               {LOG("JsonArrayValueList: JsonValue");}
                        |   JsonArrayValueList ',' JsonValue        {LOG("JsonArrayValueList: JsonArrayValueList , JsonValue");}

JsonValue               :   JsonMap                                 {LOG("JsonValue: JsonMap");}
                        |   JsonArray                               {LOG("JsonValue: JsonArray");}
                        |   JSON_STRING                             {LOG("JsonValue: JSON_STRING");}
                        |   JSON_NUMBER                             {LOG("JsonValue: JSON_NUMBER");}
                        |   JSON_TRUE                               {LOG("JsonValue: JSON_TRUE");}
                        |   JSON_FALSE                              {LOG("JsonValue: JSON_FALSE");}
                        |   JSON_NULL                               {LOG("JsonValue: JSON_NULL");}


%%

int yylex(int*, FlexLexer& lexer)
{
    return lexer.yylex();
}

void yy::JsonParser::error(yy::location const&, std::string const& msg)
{
    throw std::runtime_error(msg);
}
