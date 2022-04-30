#include "AST.h"
#include "Parser.h"
#include "PrettyPrinter.h"
#include <cmath>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <memory>
#include <string>
#include "Logger.h"

using namespace std;

int main(int argc, char* argv[]) {
    string input = string { std::istreambuf_iterator<char>{ std::cin }, std::istreambuf_iterator<char>{} };
    Parser parser;

    // Logger::level = LogLevel::DEBUG;
    
    unique_ptr<ast> t = nullptr;
    try {
        t = parser.parse(input);
    } catch (runtime_error& e) {
        printf("error: %s\n", e.what());
        return 1;
    }
    PrettyPrinter pprint0;
    if (t) {
        t->accept(pprint0);
        printf("%s\n", pprint0.ctx.s.c_str());
    }
    return 0;
}
