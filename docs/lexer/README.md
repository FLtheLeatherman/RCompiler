# Lexer 实现文档

本项目实现了一个 Rust 语言子集的词法分析器（Lexer），用于将源代码转换为 token 序列。

## Token 类型列表

### 严格关键字 (Strict Keywords)

| Token 类型 | 对应字符串 |
|-----------|-----------|
| `kAs` | `as` |
| `kBreak` | `break` |
| `kConst` | `const` |
| `kContinue` | `continue` |
| `kCrate` | `crate` |
| `kElse` | `else` |
| `kEnum` | `enum` |
| `kFalse` | `false` |
| `kFn` | `fn` |
| `kFor` | `for` |
| `kIf` | `if` |
| `kImpl` | `impl` |
| `kIn` | `in` |
| `kLet` | `let` |
| `kLoop` | `loop` |
| `kMatch` | `match` |
| `kMod` | `mod` |
| `kMove` | `move` |
| `kMut` | `mut` |
| `kRef` | `ref` |
| `kReturn` | `return` |
| `kSelf` | `self` |
| `kSelf_` | `Self` |
| `kStatic` | `static` |
| `kStruct` | `struct` |
| `kSuper` | `super` |
| `kTrait` | `trait` |
| `kTrue` | `true` |
| `kType` | `type` |
| `kUnsafe` | `unsafe` |
| `kUse` | `use` |
| `kWhere` | `where` |
| `kWhile` | `while` |
| `kDyn` | `dyn` |

### 标识符 (Identifier)

| Token 类型 | 正则表达式 |
|-----------|-----------|
| `kIdentifier` | `[a-zA-Z][a-zA-Z0-9_]*` |

### 注释 (Comments)

| Token 类型 | 正则表达式 |
|-----------|-----------|
| `kComment` | `((//([^\n])*(\n)?)|(\/\*[\s\S]*\*\/))` |

### 字面量 (Literals)

| Token 类型 | 正则表达式 |
|-----------|-----------|
| `kCharLiteral` | `'([^'\\\n\r\t]|\\'|\\"|\\x[0-7][0-9a-fA-F]|\\n|\\r|\\t|\\\\|\\0)'` |
| `kStringLiteral` | `"(([^"\\\r\t])|(\\')|(\\")|((\\x[0-7][0-9a-fA-F])|(\\n)|(\\r)|(\\t)|(\\\\)|(\\0))|(\\\n))*"([a-zA-Z][a-zA-Z0-9_]*)?` |
| `kRawStringLiteral` | `r([#]+)([^\r])*?(\1)` |
| `kCStringLiteral` | `c"(([^"\\\r\0])|(\\x[0-7][0-9a-fA-F])|(\\n)|(\\r)|(\\t)|\\\\|(\\\n))*"` |
| `kRawCStringLiteral` | `cr([#]+)([^\r\0])*?(\1)` |
| `kIntegerLiteral` | `((0b[0-1_]*[0-1][0-1_]*)|(0o[0-7_]*[0-7][0-7_]*)|(0x[0-9a-fA-F_]*[0-9a-fA-F][0-9a-fA-F_]*)|([0-9][0-9_]*))((u32)|(i32)|(usize)|(isize))?` |

### 标点符号 (Punctuations)

| Token 类型 | 对应字符 |
|-----------|---------|
| `kPlus` | `+` |
| `kMinus` | `-` |
| `kStar` | `*` |
| `kSlash` | `/` |
| `kPercent` | `%` |
| `kCaret` | `^` |
| `kNot` | `!` |
| `kAnd` | `&` |
| `kOr` | `|` |
| `kAndAnd` | `&&` |
| `kOrOr` | `||` |
| `kShl` | `<<` |
| `kShr` | `>>` |
| `kPlusEq` | `+=` |
| `kMinusEq` | `-=` |
| `kStarEq` | `*=` |
| `kSlashEq` | `/=` |
| `kPercentEq` | `%=` |
| `kCaretEq` | `^=` |
| `kAndEq` | `&=` |
| `kOrEq` | `|=` |
| `kShlEq` | `<<=` |
| `kShrEq` | `>>=` |
| `kEq` | `=` |
| `kEqEq` | `==` |
| `kNe` | `!=` |
| `kGt` | `>` |
| `kLt` | `<` |
| `kGe` | `>=` |
| `kLe` | `<=` |
| `kAt` | `@` |
| `kUnderscore` | `_` |
| `kDot` | `.` |
| `kDotDot` | `..` |
| `kDotDotDot` | `...` |
| `kDotDotEq` | `..=` |
| `kComma` | `,` |
| `kSemi` | `;` |
| `kColon` | `:` |
| `kPathSep` | `::` |
| `kRArrow` | `->` |
| `kFatArrow` | `=>` |
| `kLArrow` | `<-` |
| `kPound` | `#` |
| `kDollar` | `$` |
| `kQuestion` | `?` |
| `kTilde` | `~` |

### 分隔符 (Delimiters)

| Token 类型 | 对应字符 |
|-----------|---------|
| `kLCurly` | `{` |
| `kRCurly` | `}` |
| `kLSquare` | `[` |
| `kRSquare` | `]` |
| `kLParenthese` | `(` |
| `kRParenthese` | `)` |

### 特殊 Token

| Token 类型 | 说明 |
|-----------|------|
| `kEOF` | 文件结束标记 |

## 实现特点

1. **基于正则表达式**：使用 C++ `std::regex` 库进行模式匹配
2. **最长匹配原则**：在多个可能的匹配中选择最长的一个
3. **自动跳过空白字符**：在词法分析过程中自动忽略空格、制表符等空白字符
4. **支持 Rust 语法特性**：包括原始字符串、C 风格字符串、各种进制整数等

## 使用方法

```cpp
#include "lexer/lexer.hpp"

Lexer lexer;
std::string source_code = "let x = 42;";
auto tokens = lexer.lex(source_code);
```

词法分析器返回一个 `std::vector<std::pair<Token, std::string>>`，其中每个元素包含 token 类型和对应的字符串值。