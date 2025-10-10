#include "parser.hpp"

Token Parser::peek() {
    if (pos < tokens.size()) return tokens[pos].first;
    else return Token::kEOF;
}
void Parser::consume() {
    pos++;
}

std::unique_ptr<Crate> Parser::parseCrate() {

}