#include "lexer.hpp"

std::vector<std::pair<Token, std::string>> Lexer::lex(std::string str) {
    std::vector<std::pair<Token, std::string>> res;
    for (size_t i = 0; i < str.size(); ++i) {
        while (i < str.size() && std::isspace(str[i])) {
            i++;
        }
        if (i >= str.size()) break;
        size_t best_len = 0;
        std::pair<Token, std::string> best_match;
        for (auto [token, reg]: patterns) {
            std::smatch match;
            auto sub = str.substr(i);
            if (std::regex_search(sub, match, reg) && match.position() == 0) {
                auto match_str = match.str();
                if (match_str.size() > best_len) {
                    best_len = match_str.size();
                    best_match = make_pair(token, match_str);
                }
            }
        }
        if (best_len > 0) {
            res.push_back(best_match);
            i += best_len - 1;
        }
    }
    res.push_back(std::make_pair(Token::kEOF, "EOF"));
    return res;
}