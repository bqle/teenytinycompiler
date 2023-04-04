#include "lex.h"

Token::Token(const string& token_text, TokenType tt): text(token_text), kind(tt)
{} 

map<string, TokenType> tok_type_str = {
    {"END_OF_FILE", END_OF_FILE},
    {"NEWLINE", NEWLINE},
	{"NUMBER", NUMBER},
	{"IDENT", IDENT},
	{"STRING", STRING},
	{"LABEL", LABEL},
	{"GOTO", GOTO},
	{"PRINT", PRINT},
	{"INPUT", INPUT},
	{"LET", LET},
	{"IF", IF},
	{"THEN", THEN},
	{"ENDIF", ENDIF},
	{"WHILE", WHILE},
	{"REPEAT", REPEAT},
	{"ENDWHILE", ENDWHILE},
	// Operators
	{"EQ", EQ},
	{"PLUS", PLUS},
	{"MINUS", MINUS},
	{"ASTERISK", ASTERISK},
	{"SLASH", SLASH},
	{"EQEQ", EQEQ},
	{"NOTEQ", NOTEQ},
	{"LT", LT},
	{"LTEQ", LTEQ},
	{"GT", GT},
	{"GTEQ", GTEQ},
	{"BRACK_L", BRACK_L},
	{"BRACK_R", BRACK_R}
};

optional<TokenType> Token::check_if_keyword(const string& token_text) {
    return tok_type_str.find(token_text) != tok_type_str.end() ? optional(tok_type_str[token_text]) : nullopt; 
}

Lexer::Lexer(const string & source): source(source+"\n"), cur_char('\0'), cur_pos(-1) {
    next_char();
}

void Lexer::next_char() {
    cur_pos++;
    if (cur_pos >= source.size()) {
        cur_char = '\0';
    } else {
        cur_char = source[cur_pos];
    }
}

char Lexer::peek() {
    if (cur_pos +1 >= source.size()) {
        return '\0';
    } else {
        return source[cur_pos+1];
    }
}

void Lexer::abort(const string& msg) {
    perror(msg.c_str());
    exit(1);
}

void Lexer::skip_whitespace() {
    while (cur_char == ' ' || cur_char == '\r' || cur_char == '\t') {
        next_char();
    }
}

void Lexer::skip_comment() {
    if (cur_char == '#') {
        while (cur_char != '\n') {
            next_char();
        }
    }
}

Token Lexer::get_token() {
    skip_whitespace();
    skip_comment();
    Token token = Token(string(1,'\0'), END_OF_FILE);
    if (cur_char == '+') {
       token = Token(string(1, cur_char), PLUS); 
    } else if (cur_char == '-') {
        token = Token(string(1, cur_char), MINUS);
    } else if (cur_char == '*') {
        token = Token(string(1, cur_char), ASTERISK);
    } else if (cur_char == '/') {
        token = Token(string(1, cur_char), SLASH);
    } else if (cur_char == '(') {
        token = Token(string(1, cur_char), BRACK_L);
    } else if (cur_char == ')') {
        token = Token(string(1, cur_char), BRACK_R);
    } else if (cur_char == '\0') {
        token = Token(string(1, cur_char), END_OF_FILE);
    } else if (cur_char == '\n') {
        token = Token(string(1, cur_char), NEWLINE);
    } else if (cur_char == '=') {
        if (peek() == '=') {
            next_char();
            token = Token("==", EQEQ);
        } else {
            token = Token("=", EQ);
        }
    } else if (cur_char == '<') {
        if (peek() == '=') {
            next_char();
            token = Token("<=", LTEQ);
        } else {
            token = Token("<", LT);
        }
    } else if (cur_char == '>') {
        if (peek() == '=') {
            next_char();
            token = Token(">=", GTEQ);
        }
    } else if (cur_char == '!') {
        if (peek() != '=')  {
            abort("Unexpected token !");
        } else {
            next_char();
            token = Token("!=", NOTEQ); 
        }
    } else if (cur_char == '\"') {
        next_char();
        int start_pos = cur_pos;

        while (cur_char != '\"') {
            if (cur_char == '\r' || cur_char == '\n' || cur_char == '\t' || cur_char == '\\' || cur_char == '%'){
                abort("Illegal character in string");
            }
            next_char();
        }

        string tok_text = source.substr(start_pos, cur_pos - start_pos);
        token = Token(tok_text, STRING);
    } else if (isdigit(cur_char)) {
        int start_pos = cur_pos;

        while (isdigit(peek())) {
            next_char();
        }
        if (peek() == '.') {
            next_char();
            if (!isdigit(peek())) {
                abort("Illegal character in number.");
            }
            while (isdigit(peek())) {
                next_char();
            }
        }

        string tok_text = source.substr(start_pos, cur_pos + 1 - start_pos);
        token = Token(tok_text, NUMBER);
    } else if (isalpha(cur_char)) {
        int start_pos = cur_pos;
        while (isalnum(peek())) {
            next_char();
        }

        string tok_text = source.substr(start_pos, cur_pos + 1 - start_pos);
        optional<TokenType> type = Token::check_if_keyword(tok_text); 
        if (type) {
            token = Token(tok_text, *type);
        } else {
            token = Token(tok_text, IDENT);
        }
    } else {
        abort("Unknown token " + to_string(cur_char));
    }
    next_char();
    return token;
}