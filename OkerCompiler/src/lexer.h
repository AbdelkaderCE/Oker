#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

enum class TokenType {
    // Literals
    NUMBER,
    STRING,
    BOOLEAN,
    IDENTIFIER,

    // Keywords
    LET,
    SAY,
    IF,
    ELSE,
    END,
    WHILE,
    REPEAT,
    MAKEF,
    RETURN,
    TRY,
    FAIL,
    CLASS,
    NEW,
    THIS,
    BREAK,
    CONTINUE,

    // Operators
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    MODULO,
    ASSIGN,
    EQUAL,
    NOT_EQUAL,
    LESS_THAN,
    LESS_EQUAL,
    GREATER_THAN,
    GREATER_EQUAL,
    AND,
    OR,
    NOT,

    // Punctuation
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    LBRACKET, // New
    RBRACKET, // New
    COMMA,
    DOT,
    COLON,
    SEMICOLON,

    // Special
    NEWLINE,
    EOF_TOKEN,
    COMMENT,

    // Error
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;

    Token(TokenType t, const std::string& v, int l, int c) 
        : type(t), value(v), line(l), column(c) {}

    std::string toString() const;
};

class Lexer {
private:
    std::string source;
    size_t position;
    int line;
    int column;
    std::unordered_map<std::string, TokenType> keywords;

    void initKeywords();
    char current() const;
    char peek() const;
    void advance();
    void skipWhitespace();
    Token readString();
    Token readNumber();
    Token readIdentifier();
    Token readComment();
    Token readMultiLineComment();

public:
    explicit Lexer(const std::string& source);
    std::vector<Token> tokenize();
};

#endif
