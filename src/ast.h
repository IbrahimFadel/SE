#ifndef AST_H
#define AST_H

#include <string.h>
#include <memory>
#include <vector>
#include <map>
#include <llvm/IR/Value.h>

#include "lexer.h"

using std::unique_ptr;

class Function_Declaration;
class Code_Block;
class Variable_Declaration;
class Expression;

#include <llvm/IR/Value.h>

class Node
{
private:
public:
    virtual llvm::Value *code_gen(llvm::Module *mod) = 0;
};

class Expression : public Node
{
private:
public:
    virtual llvm::Value *code_gen(llvm::Module *mod) = 0;
};

class Number_Expression : public Expression
{
private:
    double value;

public:
    Number_Expression(double value) : value(value){};
    llvm::Value *code_gen(llvm::Module *mod);
};

class Variable_Reference_Expression : public Expression
{
private:
    std::string name;

public:
    Variable_Reference_Expression(std::string name) : name(name){};
    llvm::Value *code_gen(llvm::Module *mod);
};

class Binary_Operation_Expression : public Expression
{
private:
    Token_Type op;
    unique_ptr<Expression> lhs;
    unique_ptr<Expression> rhs;

public:
    Binary_Operation_Expression(Token_Type op, unique_ptr<Expression> lhs, unique_ptr<Expression> rhs) : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)){};
    llvm::Value *code_gen(llvm::Module *mod);
};

class Unary_Prefix_Operation_Expression : public Expression
{
private:
    Token_Type op;
    unique_ptr<Expression> value;

public:
    Unary_Prefix_Operation_Expression(Token_Type op, unique_ptr<Expression> value) : op(op), value(std::move(value)){};
    llvm::Value *code_gen(llvm::Module *mod);
};

class Function_Declaration : public Node
{
private:
    std::string name;
    std::map<std::string, std::string> params;
    std::string return_type;
    std::unique_ptr<Code_Block> then;

    std::map<std::string, llvm::Value *> variables;

public:
    Function_Declaration(std::string name, std::map<std::string, std::string> params, std::string return_type, std::unique_ptr<Code_Block> then) : name(name), params(params), return_type(return_type), then(std::move(then)){};
    llvm::Value *code_gen(llvm::Module *mod);

    void set_variable(std::string name, llvm::Value *v);
    llvm::Value *get_variable(std::string name);
    std::string get_name();
    std::map<std::string, std::string> get_params();
    std::string get_return_type();
};

class Code_Block : public Node
{
private:
    std::vector<unique_ptr<Node>> nodes;

public:
    Code_Block(std::vector<unique_ptr<Node>> nodes) : nodes(std::move(nodes)){};
    llvm::Value *code_gen(llvm::Module *mod);
};

class Variable_Declaration : public Node
{
private:
    std::string name;
    std::string type;
    unique_ptr<Expression> value;
    // std::map<std::string, unique_ptr<Expression>> properties;
    // std::vector<std::string> property_names;
    // std::vector<unique_ptr<Expression>> property_values;
    bool is_struct = false;

public:
    Variable_Declaration(std::string name, std::string type, unique_ptr<Expression> value) : name(name), type(type), value(std::move(value)){};
    Variable_Declaration(std::string name, std::string type, unique_ptr<Expression> value, bool is_struct) : name(name), type(type), value(std::move(value)), is_struct(is_struct){};
    // Variable_Declaration(std::string name, std::string type, std::map<std::string, unique_ptr<Expression>> properties) : name(name), type(type), properties(std::move(properties))
    // {
    // is_struct = true;
    // };
    // Variable_Declaration(std::string name, std::string type) : name(name), type(type)
    // {
    // is_struct = true;
    // undefined = true;
    // };
    llvm::Value *code_gen(llvm::Module *mod);
};

class Struct_Type_Expression : public Expression
{
private:
    std::string name;
    std::map<std::string, std::string> properties;

public:
    Struct_Type_Expression(std::string name, std::map<std::string, std::string> properties) : name(name), properties(properties){};
    llvm::Value *code_gen(llvm::Module *mod);
};

class Struct_Value_Expression : public Expression
{
private:
    std::map<std::string, unique_ptr<Expression>> properties;

public:
    Struct_Value_Expression(std::map<std::string, unique_ptr<Expression>> properties) : properties(std::move(properties)){};
    llvm::Value *code_gen(llvm::Module *mod);
    std::map<std::string, unique_ptr<Expression>> get_properties();
};

class If_Statement : public Expression
{
private:
    std::vector<unique_ptr<Expression>> conditions;
    std::vector<Token_Type> condition_separators;
    unique_ptr<Code_Block> then;

public:
    If_Statement(std::vector<unique_ptr<Expression>> conditions, std::vector<Token_Type> condition_separators, unique_ptr<Code_Block> then) : conditions(std::move(conditions)), condition_separators(condition_separators), then(std::move(then)){};
    llvm::Value *code_gen(llvm::Module *mod);
};

class Return_Statement : public Expression
{
private:
    unique_ptr<Expression> value;

public:
    Return_Statement(unique_ptr<Expression> value) : value(std::move(value)){};
    llvm::Value *code_gen(llvm::Module *mod);
};

class Function_Call_Expression : public Expression
{
private:
    std::string name;
    std::vector<unique_ptr<Expression>> params;

public:
    Function_Call_Expression(std::string name, std::vector<unique_ptr<Expression>> params) : name(name), params(std::move(params)){};
    llvm::Value *code_gen(llvm::Module *mod);
};

class Import_Statement : public Expression
{
private:
    std::string path;

public:
    Import_Statement(std::string path) : path(path){};
    llvm::Value *code_gen();
    std::string get_path();
    llvm::Value *code_gen(llvm::Module *mod);
};

typedef std::vector<unique_ptr<Node>> Nodes;

#endif