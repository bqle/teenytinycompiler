#include "parse.h"
#include "lex.h"
#include "emit.h"
#include <bits/stdc++.h>

using namespace std;

int main(int argc, char** argv) {
    if (argc != 2) {
        perror("Needs source file as arg");
        exit(1);           
    }
    const char*fn = argv[1];
    ifstream file {string(fn)};
    string source ((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

    Lexer lexer {source};
    Emitter emitter("out.c");
    Parser parser (lexer, emitter);

    parser.program();
}