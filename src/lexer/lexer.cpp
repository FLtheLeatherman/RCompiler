#include "lexer/lexer.hpp"
#include <iostream>

std::vector<std::pair<Token, std::string>> Lexer::lex(std::string str) {
    std::vector<std::pair<Token, std::string>> res;
    for (size_t i = 0; i < str.size(); ++i) {
        while (i < str.size() && std::isspace(str[i])) {
            i++;
        }
        if (i >= str.size()) break;
        if (str.substr(i, 2) == "//") {
            uint32_t cur = 0;
            while (str[i + cur] != '\n' && str[i + cur] != '\r') {
                cur++;
            }
            i += cur;
            continue;
        }
        if (str.substr(i, 2) == "/*") {
            uint32_t cur = 2, cnt = 1;
            while (cnt > 0) {
                if (str.substr(i + cur, 2) == "/*") {
                    cnt++;
                } else if (str.substr(i + cur, 2) == "*/") {
                    cnt--;
                }
                cur++;
            }
            i += cur + 1;
            continue;
        }
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
            if (best_match.first != Token::kComment) res.push_back(best_match);
            i += best_len - 1;
        }
    }
    res.push_back(std::make_pair(Token::kEOF, "EOF"));
    return res;
}

std::string tokenToString(Token token) {
    switch (token) {
        // strict keywords
        case Token::kAs: return "kAs";
        case Token::kBreak: return "kBreak";
        case Token::kConst: return "kConst";
        case Token::kContinue: return "kContinue";
        case Token::kCrate: return "kCrate";
        case Token::kElse: return "kElse";
        case Token::kEnum: return "kEnum";
        case Token::kFalse: return "kFalse";
        case Token::kFn: return "kFn";
        case Token::kFor: return "kFor";
        case Token::kIf: return "kIf";
        case Token::kImpl: return "kImpl";
        case Token::kIn: return "kIn";
        case Token::kLet: return "kLet";
        case Token::kLoop: return "kLoop";
        case Token::kMatch: return "kMatch";
        case Token::kMod: return "kMod";
        case Token::kMove: return "kMove";
        case Token::kMut: return "kMut";
        case Token::kRef: return "kRef";
        case Token::kReturn: return "kReturn";
        case Token::kSelf: return "kSelf";
        case Token::kSelf_: return "kSelf_";
        case Token::kStatic: return "kStatic";
        case Token::kStruct: return "kStruct";
        case Token::kSuper: return "kSuper";
        case Token::kTrait: return "kTrait";
        case Token::kTrue: return "kTrue";
        case Token::kType: return "kType";
        case Token::kUnsafe: return "kUnsafe";
        case Token::kUse: return "kUse";
        case Token::kWhere: return "kWhere";
        case Token::kWhile: return "kWhile";
        case Token::kDyn: return "kDyn";

        // identifier
        case Token::kIdentifier: return "kIdentifier";

        // comments
        case Token::kComment: return "kComment";

        // literals
        case Token::kCharLiteral: return "kCharLiteral";
        case Token::kStringLiteral: return "kStringLiteral";
        case Token::kRawStringLiteral: return "kRawStringLiteral";
        case Token::kCStringLiteral: return "kCStringLiteral";
        case Token::kRawCStringLiteral: return "kRawCStringLiteral";
        case Token::kIntegerLiteral: return "kIntegerLiteral";

        // punctuations
        case Token::kPlus: return "kPlus";
        case Token::kMinus: return "kMinus";
        case Token::kStar: return "kStar";
        case Token::kSlash: return "kSlash";
        case Token::kPercent: return "kPercent";
        case Token::kCaret: return "kCaret";
        case Token::kNot: return "kNot";
        case Token::kAnd: return "kAnd";
        case Token::kOr: return "kOr";
        case Token::kAndAnd: return "kAndAnd";
        case Token::kOrOr: return "kOrOr";
        case Token::kShl: return "kShl";
        case Token::kShr: return "kShr";
        case Token::kPlusEq: return "kPlusEq";
        case Token::kMinusEq: return "kMinusEq";
        case Token::kStarEq: return "kStarEq";
        case Token::kSlashEq: return "kSlashEq";
        case Token::kPercentEq: return "kPercentEq";
        case Token::kCaretEq: return "kCaretEq";
        case Token::kAndEq: return "kAndEq";
        case Token::kOrEq: return "kOrEq";
        case Token::kShlEq: return "kShlEq";
        case Token::kShrEq: return "kShrEq";
        case Token::kEq: return "kEq";
        case Token::kEqEq: return "kEqEq";
        case Token::kNe: return "kNe";
        case Token::kGt: return "kGt";
        case Token::kLt: return "kLt";
        case Token::kGe: return "kGe";
        case Token::kLe: return "kLe";
        case Token::kAt: return "kAt";
        case Token::kUnderscore: return "kUnderscore";
        case Token::kDot: return "kDot";
        case Token::kDotDot: return "kDotDot";
        case Token::kDotDotDot: return "kDotDotDot";
        case Token::kDotDotEq: return "kDotDotEq";
        case Token::kComma: return "kComma";
        case Token::kSemi: return "kSemi";
        case Token::kColon: return "kColon";
        case Token::kPathSep: return "kPathSep";
        case Token::kRArrow: return "kRArrow";
        case Token::kFatArrow: return "kFatArrow";
        case Token::kLArrow: return "kLArrow";
        case Token::kPound: return "kPound";
        case Token::kDollar: return "kDollar";
        case Token::kQuestion: return "kQuestion";
        case Token::kTilde: return "kTilde";

        // delimiters
        case Token::kLCurly: return "kLCurly";
        case Token::kRCurly: return "kRCurly";
        case Token::kLSquare: return "kLSquare";
        case Token::kRSquare: return "kRSquare";
        case Token::kLParenthese: return "kLParenthese";
        case Token::kRParenthese: return "kRParenthese";

        // special
        case Token::kEOF: return "kEOF";

        default: return "Unknown";
    }
}