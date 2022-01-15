#ifndef SRC_SCANNER_HPP
#define SRC_SCANNER_HPP

class Token {
  public:
    enum TokenType {                                        
        // Single-character tokens.                         
        TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,                
        TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,                
        TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,    
        TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,

        // One or two character tokens.                     
        TOKEN_BANG, TOKEN_BANG_EQUAL,                       
        TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,                     
        TOKEN_GREATER, TOKEN_GREATER_EQUAL,                 
        TOKEN_LESS, TOKEN_LESS_EQUAL,                       

        // Literals.                                        
        TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,       

        // Keywords.                                        
        TOKEN_AND, TOKEN_CLASS, TOKEN_ELSE, TOKEN_FALSE,    
        TOKEN_FUN, TOKEN_FOR, TOKEN_IF, TOKEN_NIL, TOKEN_OR,
        TOKEN_PRINT, TOKEN_RETURN, TOKEN_SUPER, TOKEN_THIS, 
        TOKEN_TRUE, TOKEN_VAR, TOKEN_WHILE,                 

        TOKEN_ERROR,                                        
        TOKEN_EOF                                           
    };
    TokenType type;
    const char* start;
    int length;
    int line;

  Token(TokenType type, const char *start, int length, int line) :
    type(type), start(start), length(length), line(line) {};
  Token() {};
};

class Scanner {
  public:
    Scanner() : start(nullptr), current(nullptr), line(0) {};
    Scanner(const char *source) : start(source), current(source), line(1) {};
    Token scanToken(void);
  private:
    const char* start;
    const char* current;
    int line;

    inline Token errorToken(const char*) const;
    inline Token makeToken(Token::TokenType) const;
    Token identifier(void);
    Token number(void);
    Token string(void);

    inline char advance(void);
    inline char peek(void) const;
    inline char peekNext(void) const;

    inline bool isAtEnd(void) const;
    Token::TokenType identifierType(void) const;
    bool match(char expected);
    void skipWhitespace(void);
};

#endif /* SRC_SCANNER_HPP */
