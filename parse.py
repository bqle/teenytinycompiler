import sys
from lex import *

# Parser object keeps track of current token and checks if the code matches the grammar.
class Parser:
    def __init__(self, lexer, emitter):
        self.lexer = lexer
        self.emitter = emitter

        self.symbols = set()
        self.labelsDeclared = set() 
        self.labelsGotoed = set()

        self.curToken = None
        self.peekToken = None
        self.nextToken()
        self.nextToken() # Call this twice to initialize current and peek.

    # Return true if the current token matches.
    def checkToken(self, kind):
        return kind == self.curToken.kind 

    # Return true if the next token matches.
    def checkPeek(self, kind):
        return kind == self.peekToken.kind

    # Try to match current token. If not, error. Advances the current token.
    def match(self, kind):
        if not self.checkToken(kind):
            self.abort("Expected " + kind.name + ", got " + self.curToken.kind.name)
        self.nextToken()


    # Advances the current token.
    def nextToken(self):
        self.curToken = self.peekToken
        self.peekToken = self.lexer.getToken()

    def abort(self, message):
        sys.exit("Error. " + message)


    # Production rules.

    # program ::= {statement}
    def program(self):
        print("PROGRAM")
        self.emitter.headerLine("#include <stdio.h>")
        self.emitter.headerLine("int main(void){")

        self.emitter.increaseLvl()
        while self.checkToken(TokenType.NEWLINE):
            self.nextToken()

        while not self.checkToken(TokenType.EOF):
            self.statement()

        for label in self.labelsGotoed:
            if label not in self.labelsDeclared:
                self.abort("Attempting to GOTO to undeclared label: " + label)
        self.emitter.emitLine("return 0;")
        self.emitter.decreaseLvl()

        self.emitter.emitLine("}")

    def statement(self):
        if self.checkToken(TokenType.PRINT):
            print("STATEMENT-PRINT")
            self.nextToken()

            if self.checkToken(TokenType.STRING):
                self.emitter.emitLine("printf(\"" + self.curToken.text + "\\n\");", True)
                self.nextToken()
            else:
                self.emitter.emit("printf(\"%" + ".2f\\n\", (float)(", True)
                self.expression()
                self.emitter.emitLine("));")
        elif self.checkToken(TokenType.IF):
            print("STATEMENT-IF")
            self.nextToken()
            self.emitter.emit("if(", True)
            self.comparison()
            self.emitter.emitLine(") {")
            self.match(TokenType.THEN)
            self.nl()

            self.emitter.increaseLvl()
            while not self.checkToken(TokenType.ENDIF):
                self.statement()

            self.match(TokenType.ENDIF)
            self.emitter.decreaseLvl()
            self.emitter.emitLine("}", True)

        elif self.checkToken(TokenType.WHILE):
            print("STATEMENT-WHILE")
            self.nextToken()
            self.emitter.emit("while(", True)
            self.comparison()

            self.match(TokenType.REPEAT)
            self.nl()
            self.emitter.emitLine("){")

            self.emitter.increaseLvl()
            while not self.checkToken(TokenType.ENDWHILE):
                self.statement()
                
            self.emitter.decreaseLvl()
            self.emitter.emitLine("}", True)
            self.match(TokenType.ENDWHILE)
        elif self.checkToken(TokenType.LABEL):
            print("STATEMENT-LABEL")
            self.nextToken()

            if self.curToken.text in self.labelsDeclared:
                self.abort("Label already exists: " + self.curToken.text)
            self.labelsDeclared.add(self.curToken.text)
            self.emitter.emitLine(self.curToken.text + ":")
            self.match(TokenType.IDENT)
        elif self.checkToken(TokenType.GOTO):
            print("STATEMENT-GOTO")
            self.nextToken()
            self.labelsGotoed.add(self.curToken.text)
            self.emitter.emitLine("goto " + self.curToken.text + ";")
            self.match(TokenType.IDENT) 
        elif self.checkToken(TokenType.LET):
            print("STATEMENT-LET")
            self.nextToken()

            if self.curToken.text not in self.symbols:
                self.emitter.headerLine("float " + self.curToken.text + ";")
                self.symbols.add(self.curToken.text)

            self.emitter.emit(self.curToken.text + " = ", True)
            self.match(TokenType.IDENT)
            self.match(TokenType.EQ)
            self.expression()
            self.emitter.emitLine(";")
        elif self.checkToken(TokenType.INPUT):
            print("STATEMENT-INPUT")
            self.nextToken()

            if self.curToken.text not in self.symbols:
                self.symbols.add(self.curToken.text)
                self.emitter.headerLine("float " + self.curToken.text + ";")

            self.emitter.emitLine("if(0 == scanf(\"%" + "f\", &" + self.curToken.text + ")) {", True)
            self.emitter.increaseLvl()
            self.emitter.emitLine(self.curToken.text + " = 0;", True)
            self.emitter.emitLine("scanf(\"%*s\");", True)
            
            self.emitter.decreaseLvl()
            self.emitter.emitLine("}", True)
            self.match(TokenType.IDENT)
        else:
            self.abort("Invalid statement at " + self.curToken.text + " (" + self.curToken.kind.name + ")") 

        self.nl()

    def nl(self):
        print("NEWLINE")
		
        self.match(TokenType.NEWLINE)
        while self.checkToken(TokenType.NEWLINE):
            self.nextToken()

    def isComparisonOperator(self):
        return self.checkToken(TokenType.GT) or self.checkToken(TokenType.GTEQ) or self.checkToken(TokenType.LT) or self.checkToken(TokenType.LTEQ) or self.checkToken(TokenType.EQEQ) or self.checkToken(TokenType.NOTEQ)

    def comparison(self):
        print("COMPARISON")

        self.expression()
        if self.isComparisonOperator():
            self.emitter.emit(self.curToken.text)
            self.nextToken()
            self.expression()
        else:
            self.abort("Expected comparison operator at: " + self.curToken.text)

        while self.isComparisonOperator():
            self.emitter.emit(self.curToken.text)
            self.nextToken()
            self.expression()

    def expression(self):
        print("EXPRESSION")
        self.term()

        while self.checkToken(TokenType.PLUS) or self.checkToken(TokenType.MINUS):
            self.emitter.emit(self.curToken.text)
            self.nextToken()
            self.term()
    
    def term(self):
        print("TERM")
        self.unary()

        while self.checkToken(TokenType.ASTERISK) or self.checkToken(TokenType.SLASH):
            self.emitter.emit(self.curToken.text)
            self.nextToken()
            self.unary()

    def unary(self):
        print("UNARY")

        # Optional unary +/-
        if self.checkToken(TokenType.PLUS) or self.checkToken(TokenType.MINUS):
            self.emitter.emit(self.curToken.text)
            self.nextToken()        
        self.primary()

    def primary(self):
        print("PRIMARY (" + self.curToken.text + ")")
        if self.checkToken(TokenType.NUMBER): 
            self.emitter.emit(self.curToken.text)
            self.nextToken()
        elif self.checkToken(TokenType.IDENT):
            if self.curToken.text not in self.symbols:
                self.abort("Referencing variable before assignment: " + self.curToken.text)

            self.emitter.emit(self.curToken.text)
            self.nextToken()
        elif self.checkToken(TokenType.BRACK_L):
            self.emitter.emit(self.curToken.text)
            self.nextToken()
            self.expression()
            if self.checkToken(TokenType.BRACK_R):
                self.emitter.emit(self.curToken.text)
                self.nextToken()
            else:
                self.abort("Mismatched closing bracket")
        else:
            self.abort("Unexpected token at " + self.curToken.text)
