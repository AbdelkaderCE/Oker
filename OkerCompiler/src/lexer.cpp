#include "lexer.h"
#include <stdexcept>
#include <cctype>
#include <sstream>

std::string Token::toString() const {
    std::ostringstream oss;
    oss << "Token(" << static_cast<int>(type) << ", \"" << value << "\", " 
        << line << ":" << column << ")";
    return oss.str();
}

Lexer::Lexer(const std::string& source) : source(source), position(0), line(1), column(1) {
    initKeywords();
}

void Lexer::initKeywords() {
    keywords["let"] = TokenType::LET;
    keywords["say"] = TokenType::SAY;
    keywords["if"] = TokenType::IF;
    keywords["else"] = TokenType::ELSE;
    keywords["end"] = TokenType::END;
    keywords["while"] = TokenType::WHILE;
    keywords["repeat"] = TokenType::REPEAT;
    keywords["makef"] = TokenType::MAKEF;
    keywords["return"] = TokenType::RETURN;
    keywords["try"] = TokenType::TRY;
    keywords["fail"] = TokenType::FAIL;
    keywords["class"] = TokenType::CLASS;
    keywords["new"] = TokenType::NEW;
    keywords["this"] = TokenType::THIS;
    keywords["true"] = TokenType::BOOLEAN;
    keywords["false"] = TokenType::BOOLEAN;
    keywords["and"] = TokenType::AND;
    keywords["or"] = TokenType::OR;
    keywords["not"] = TokenType::NOT;
    keywords["break"] = TokenType::BREAK;
    keywords["continue"] = TokenType::CONTINUE;
}

char Lexer::current() const {
    if (position >= source.length()) return '\0';
    return source[position];
}

char Lexer::peek() const {
    if (position + 1 >= source.length()) return '\0';
    return source[position + 1];
}

void Lexer::advance() {
    if (position < source.length()) {
        if (source[position] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        position++;
    }
}

void Lexer::skipWhitespace() {
    while (std::isspace(current()) && current() != '\n') {
        advance();
    }
}

Token Lexer::readString() {
    int startLine = line;
    int startCol = column;
    std::string value;

    advance(); // Skip opening quote

    while (current() != '"' && current() != '\0') {
        if (current() == '\\') {
            advance();
            switch (current()) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case 'r': value += '\r'; break;
                case '\\': value += '\\'; break;
                case '"': value += '"'; break;
                default: value += current(); break;
            }
        } else {
            value += current();
        }
        advance();
    }

    if (current() == '\0') {
        throw std::runtime_error("Unterminated string at line " + std::to_string(startLine));
    }

    advance(); // Skip closing quote
    return Token(TokenType::STRING, value, startLine, startCol);
}

Token Lexer::readNumber() {
    int startLine = line;
    int startCol = column;
    std::string value;
    bool hasDecimal = false;

    while (std::isdigit(current()) || (current() == '.' && !hasDecimal)) {
        if (current() == '.') {
            hasDecimal = true;
        }
        value += current();
        advance();
    }

    return Token(TokenType::NUMBER, value, startLine, startCol);
}

Token Lexer::readIdentifier() {
    int startLine = line;
    int startCol = column;
    std::string value;

    while (std::isalnum(current()) || current() == '_') {
        value += current();
        advance();
    }

    TokenType type = TokenType::IDENTIFIER;
    auto it = keywords.find(value);
    if (it != keywords.end()) {
        type = it->second;
    }

    return Token(type, value, startLine, startCol);
}

Token Lexer::readComment() {
    int startLine = line;
    int startCol = column;
    std::string value;

    advance(); // Skip first ~

    while (current() != '\n' && current() != '\0') {
        value += current();
        advance();
    }

    return Token(TokenType::COMMENT, value, startLine, startCol);
}

Token Lexer::readMultiLineComment() {
    int startLine = line;
    int startCol = column;
    std::string value;

    advance(); // Skip first ~
    advance(); // Skip second ~

    while (position < source.length() - 1) {
        if (current() == '~' && peek() == '~') {
            advance(); // Skip first ~
            advance(); // Skip second ~
            break;
        }
        value += current();
        advance();
    }

    return Token(TokenType::COMMENT, value, startLine, startCol);
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (position < source.length()) {
        skipWhitespace();

        if (position >= source.length()) break;

        char c = current();
        int startLine = line;
        int startCol = column;

        switch (c) {
            case '\n':
                tokens.emplace_back(TokenType::NEWLINE, "\n", startLine, startCol);
                advance();
                break;

            case '"':
                tokens.push_back(readString());
                break;

            case '~':
                if (peek() == '~') {
                    tokens.push_back(readMultiLineComment());
                } else {
                    tokens.push_back(readComment());
                }
                break;

            case '+':
                tokens.emplace_back(TokenType::PLUS, "+", startLine, startCol);
                advance();
                break;

            case '-':
                tokens.emplace_back(TokenType::MINUS, "-", startLine, startCol);
                advance();
                break;

            case '*':
                tokens.emplace_back(TokenType::MULTIPLY, "*", startLine, startCol);
                advance();
                break;

            case '/':
                tokens.emplace_back(TokenType::DIVIDE, "/", startLine, startCol);
                advance();
                break;

            case '%':
                tokens.emplace_back(TokenType::MODULO, "%", startLine, startCol);
                advance();
                break;

            case '=':
                if (peek() == '=') {
                    tokens.emplace_back(TokenType::EQUAL, "==", startLine, startCol);
                    advance();
                    advance();
                } else {
                    tokens.emplace_back(TokenType::ASSIGN, "=", startLine, startCol);
                    advance();
                }
                break;

            case '!':
                if (peek() == '=') {
                    tokens.emplace_back(TokenType::NOT_EQUAL, "!=", startLine, startCol);
                    advance();
                    advance();
                } else {
                    tokens.emplace_back(TokenType::NOT, "!", startLine, startCol);
                    advance();
                }
                break;

            case '<':
                if (peek() == '=') {
                    tokens.emplace_back(TokenType::LESS_EQUAL, "<=", startLine, startCol);
                    advance();
                    advance();
                } else {
                    tokens.emplace_back(TokenType::LESS_THAN, "<", startLine, startCol);
                    advance();
                }
                break;

            case '>':
                if (peek() == '=') {
                    tokens.emplace_back(TokenType::GREATER_EQUAL, ">=", startLine, startCol);
                    advance();
                    advance();
                } else {
                    tokens.emplace_back(TokenType::GREATER_THAN, ">", startLine, startCol);
                    advance();
                }
                break;

            case '(':
                tokens.emplace_back(TokenType::LPAREN, "(", startLine, startCol);
                advance();
                break;

            case ')':
                tokens.emplace_back(TokenType::RPAREN, ")", startLine, startCol);
                advance();
                break;

            case '{':
                tokens.emplace_back(TokenType::LBRACE, "{", startLine, startCol);
                advance();
                break;

            case '}':
                tokens.emplace_back(TokenType::RBRACE, "}", startLine, startCol);
                advance();
                break;

            case '[': // New case
                tokens.emplace_back(TokenType::LBRACKET, "[", startLine, startCol);
                advance();
                break;

            case ']': // New case
                tokens.emplace_back(TokenType::RBRACKET, "]", startLine, startCol);
                advance();
                break;

            case ',':
                tokens.emplace_back(TokenType::COMMA, ",", startLine, startCol);
                advance();
                break;

            case '.':
                if (std::isdigit(peek())) {
                    tokens.push_back(readNumber());
                } else {
                    tokens.emplace_back(TokenType::DOT, ".", startLine, startCol);
                    advance();
                }
                break;

            case ':':
                tokens.emplace_back(TokenType::COLON, ":", startLine, startCol);
                advance();
                break;

            case ';':
                tokens.emplace_back(TokenType::SEMICOLON, ";", startLine, startCol);
                advance();
                break;

            default:
                if (std::isdigit(c)) {
                    tokens.push_back(readNumber());
                } else if (std::isalpha(c) || c == '_') {
                    tokens.push_back(readIdentifier());
                } else {
                    tokens.emplace_back(TokenType::UNKNOWN, std::string(1, c), startLine, startCol);
                    advance();
                }
                break;
        }
    }

    tokens.emplace_back(TokenType::EOF_TOKEN, "", line, column);
    return tokens;
}
