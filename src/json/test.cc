#include "json.tab.hh"
#include <iostream>

int main()
{
    try
    {
        yyFlexLexer         lexer(&std::cin, &std::cout);
        yy::JsonParser      parser(lexer);

        std::cout << (parser.parse() == 0 ? "OK" : "FAIL") << "\n";
    }
    catch(std::exception const& e)
    {
        std::cout << "Exception: " << e.what() << "\n";
    }
}
