#include <iostream>

class ASTVisitor;

class ASTNode;

class Crate;

/****Items****/
class Item;

class Function;
class Struct;
class Enumeration;
class ConstantItem;
class Trait;
class Implementation;

class FunctionParameters;
class SelfParam;
class ShorthandSelf;
class TypedSelf;
class FunctionParam;
class FunctionReturnType;

class StructStruct;
class StructFields;
class StructField;

class EnumVariants;
class EnumVariant;

class AssociatedItem;

class InherentImpl;
class TraitImpl;
/****Items****/

/****Statements & expressions****/
class Statement;
class LetStatement;
class ExpressionStatement;
class Statements;
class Expression;
class ExpressionWithoutBlock;
class ExpressionWithBlock;
class LiteralExpression;
class PathExpression;
class OperatorExpression;
class AssignmentExpression;
class CompoundAssignmentExpression;
class BinaryExpression;
class TypeCastExpression;
class GroupedExpression;
class ArrayExpression;
class ArrayElements;
class IndexExpression;
class StructExpression;
class StructExprFields;
class StructExprField;
class CallParams;
class StructExpression;
class CallExpression;
class MethodCallExpression;
class FieldExpression;
class ContinueExpression;
class BreakExpression;
class ReturnExpression;
class BlockExpression;
class LoopExpression;
class InfiniteLoopExpression;
class PredicateLoopExpression;
class IfExpression;
class Condition;
class Statements;

// 字面量表达式
class CharLiteral;
class StringLiteral;
class RawStringLiteral;
class CStringLiteral;
class RawCStringLiteral;
class IntegerLiteral;
class BoolLiteral;

// 运算符表达式
class UnaryExpression;
class BorrowExpression;
class DereferenceExpression;
/****Statements & expressions****/

/****Patterns****/
class PatternNoTopAlt;
class IdentifierPattern;
class ReferencePattern;
/****Patterns****/

/****Types****/
class Type; // also TypeNoBounds
class ReferenceType;
class ArrayType;
class UnitType;
/****Types****/

/****Paths****/
class PathInExpression;
class PathIdentSegment; // also PathExprSegment, TypePath, TypePathSegment
/****Paths****/