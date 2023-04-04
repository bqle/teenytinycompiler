#pragma once
#include <bits/stdc++.h>

#include "lex.h"
#include "emit.h"

using namespace std;

extern map<string, TokenType> tok_type_str;

struct Parser {
    Lexer lexer;
    Emitter emitter;

    set<string> symbols;
    set<string> labels_declared;
    set<string> labels_gotoed;

    Token cur_token;
    Token peek_token;
    

    Parser(const Lexer& lexer, const Emitter& emitter);

    bool check_token(TokenType kind);

    bool check_peek(TokenType kind);

    void match(TokenType kind);

    void next_token();

    void abort(const string &);

    // Production rules
    void program();

    void statement();

    void nl();

    bool is_comparison_operator();

    void comparison();

    void expression();

    void term();

    void unary();

    void primary();

};