#include <bits/stdc++.h>

#include "parse.h"
#include "lex.h"

using namespace std;

Parser::Parser(const Lexer& lexer, const Emitter& emitter): 
    lexer(lexer), emitter(emitter), symbols(), labels_declared(), labels_gotoed(), 
    cur_token("", END_OF_FILE), peek_token("", END_OF_FILE) {
    ;
    next_token();
    next_token();
}

bool Parser::check_token(TokenType kind) {
    return kind == cur_token.kind;
}

bool Parser::check_peek(TokenType kind) {
    return kind == peek_token.kind;
}

void Parser::match(TokenType kind) {
    if (cur_token.kind == kind) {
        next_token();
    } else {
        abort("Expected " + to_string(kind) +", got" + to_string(cur_token.kind));
    }
}

void Parser::next_token() {
    cur_token = peek_token;
    peek_token = lexer.get_token();
}

void Parser::abort(const string& msg) {
    perror(msg.c_str());
    exit(1);
}

void Parser::program() {
    cout<<"Program"<<endl;
    while (check_token(TokenType::NEWLINE)) {
        next_token();
    }

    while (!check_token(TokenType::END_OF_FILE)) {
        statement();
    }

    for (const string& label : labels_gotoed) {
        if (labels_gotoed.find(label) == labels_gotoed.end()) {
            abort("Attempting GOTO to undeclared");
        }
    }
}

void Parser::statement() {
    if (check_token(PRINT)) {
        cout<<"Statement-print"<<endl;
        next_token();

        if (check_token(STRING)) {
            next_token();
        } else {
            expression();
        }
    } else if (check_token(IF)) {
        next_token();
        comparison();
        match(THEN);
        nl();

        while (!check_token(ENDIF)) {
            statement();
        }

        match(ENDIF);
    } else if (check_token(WHILE)) {
        next_token();
        comparison();
        match(REPEAT);
        nl();
        
        while (!check_token(ENDWHILE)) {
            statement();
        }

        match(ENDWHILE);
    } else if (check_token(LABEL)) {
        next_token();

        if (labels_declared.find(cur_token.text) != labels_declared.end()) {
            abort("Label already exist: " + cur_token.text);
        }
        labels_declared.insert(cur_token.text);
        match(IDENT);
    } else if (check_token(GOTO)) {
        next_token();
        labels_gotoed.insert(cur_token.text);
        match(IDENT);
    } else if (check_token(LET)) {
        next_token();

        if (symbols.find(cur_token.text) == symbols.end()) {
            symbols.insert(cur_token.text);
        }
        match(IDENT);
        match(EQ);
        expression();
    } else if (check_token(INPUT)) {
        next_token();
        if (symbols.find(cur_token.text) == symbols.end()) {
            symbols.insert(cur_token.text);
        }

        match(IDENT);
    } else {
        abort("Invalid statement at " + cur_token.text);
    }
    nl();
}

void Parser::nl() {
    match(NEWLINE);
    while (check_token(NEWLINE)) {
        next_token();
    }
}

bool Parser::is_comparison_operator() {
    return check_token(GT) || check_token(GTEQ) || check_token(LT) 
        || check_token(LTEQ) || check_token(EQEQ) || check_token(NOTEQ);
}

void Parser::comparison() {
    cout<<"Comparison"<<endl;
    expression();
    if (is_comparison_operator()) {
        next_token();
        expression();
    } else {
        abort("Expected comparison operator at: " + cur_token.text);
    }

    while (is_comparison_operator()) {
        next_token();
        expression();
    }
}

void Parser::expression() {
    cout<<"Expression"<<endl;
    term();

    while (check_token(PLUS) || check_token(MINUS)) {
        next_token();
        term();
    }
}

void Parser::term() {
    cout<<"Term"<<endl;
    unary();
    while (check_token(ASTERISK)||check_token(SLASH)) {
        next_token();
        unary();
    }
}

void Parser::unary() {
    cout<<"Unary"<<endl;
    if (check_token(PLUS) || check_token(MINUS)) {
        next_token();
    }

    primary();
}

void Parser::primary() {
    cout<< "PRIMARY " + cur_token.text<<endl;
    if (check_token(NUMBER)) {
        next_token();
    } else if (check_token(IDENT)) {
        if (symbols.find(cur_token.text) == symbols.end()) {
            abort("Referencing variable before assignment: "+ cur_token.text);
        }
        next_token();
    } else if (check_token(BRACK_L)) {
        next_token();
        expression();
        if (check_token(BRACK_R)) {
            next_token();
        } else {
            abort("Mismatched closing bracket");
        }
    } else {
        abort("Unexpected token at " + cur_token.text);
    }
}