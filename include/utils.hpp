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
class Expression;
class ExpressionWithoutBlock;
class ExpressionWithBlock;
class BlockExpression;
/****Statements & expressions****/

/****Patterns****/
class PatternNoTopAlt;
/****Patterns****/

/****Types****/
class Type;
/****Types****/