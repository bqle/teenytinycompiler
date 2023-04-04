#pragma once

#include <bits/stdc++.h>

using namespace std;

enum TokenType {
    END_OF_FILE = -1,
	NEWLINE = 0,
	NUMBER = 1,
	IDENT = 2,
	STRING = 3,
    // Keywords
	LABEL = 101,
	GOTO = 102,
	PRINT = 103,
	INPUT = 104,
	LET = 105,
	IF = 106,
	THEN = 107,
	ENDIF = 108,
	WHILE = 109,
	REPEAT = 110,
	ENDWHILE = 111,
	// Operators
	EQ = 201,
	PLUS = 202,
	MINUS = 203,
	ASTERISK = 204,
	SLASH = 205,
	EQEQ = 206,
	NOTEQ = 207,
	LT = 208,
	LTEQ = 209,
	GT = 210,
	GTEQ = 211,
	BRACK_L = 212,
	BRACK_R = 213
};

extern map<string, TokenType> tok_type_str;

struct Token {
    string text;
    TokenType kind;

    Token(const string& token_text, TokenType tt); 

    static inline optional<TokenType> check_if_keyword(const string& token_text);
};

struct Lexer {
    string source;
    char cur_char;
    int cur_pos;

    Lexer(const string & source);

    void next_char();

    char peek();

    void abort(const string& msg);

    void skip_whitespace();

    void skip_comment();

    Token get_token();
};