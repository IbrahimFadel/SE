#include "parser.h"

std::vector<std::unique_ptr<Node>> parse_tokens(std::vector<std::shared_ptr<Token>> tokens)
{
    tok_pointer = 0;
    toks = tokens;
    cur_tok = toks[tok_pointer];

    bin_op_precedence["<"] = 10;
    bin_op_precedence["+"] = 20;
    bin_op_precedence["-"] = 20;
    bin_op_precedence["*"] = 40;

    std::vector<std::unique_ptr<Node>> nodes;
    bool ate_semicolon = false;
    while (cur_tok->type != Token_Types::tok_eof)
    {
        std::unique_ptr<Node> node = std::make_unique<Node>();
        switch (cur_tok->type)
        {
        case Token_Types::tok_fn:
        {
            auto fn = parse_fn_declaration();
            ate_semicolon = false;
            node->type = Node_Types::FunctionDeclarationNode;
            node->function_node = std::move(fn);
            break;
        }
        case Token_Types::tok_import:
        {
            auto import = parse_import();
            ate_semicolon = true;
            node->type = Node_Types::ImportNode;
            node->expression_node = std::move(import);
            break;
        }
        case Token_Types::tok_i64:
        {
            auto var = parse_variable_declaration();
            ate_semicolon = true;
            node->type = Node_Types::VariableDeclarationNode;
            node->expression_node = std::move(var);
            break;
        }
        case Token_Types::tok_i32:
        {
            auto var = parse_variable_declaration();
            ate_semicolon = true;
            node->type = Node_Types::VariableDeclarationNode;
            node->expression_node = std::move(var);
            break;
        }
        case Token_Types::tok_i16:
        {
            auto var = parse_variable_declaration();
            ate_semicolon = true;
            node->type = Node_Types::VariableDeclarationNode;
            node->expression_node = std::move(var);
            break;
        }
        case Token_Types::tok_i8:
        {
            auto var = parse_variable_declaration();
            ate_semicolon = true;
            node->type = Node_Types::VariableDeclarationNode;
            node->expression_node = std::move(var);
            break;
        }
        case Token_Types::tok_object:
        {
            auto object = parse_object();
            ate_semicolon = true;
            node->type = Node_Types::ObjectNode;
            node->expression_node = std::move(object);
            break;
        }
        default:
            break;
        }
        nodes.push_back(std::move(node));
        if (!ate_semicolon)
        {
            get_next_token();
        }
    }

    return nodes;
}

std::unique_ptr<Variable_Node> parse_variable_declaration()
{
    Variable_Types type = token_type_to_variable_type(cur_tok->type);

    get_next_token();

    std::string name = cur_tok->value;

    get_next_token();

    get_next_token();

    auto val = parse_expression(true, type);
    if (!val)
    {
        error("Expected expression");
        return nullptr;
    }

    std::unique_ptr<Variable_Node> var_node = std::make_unique<Variable_Node>(name, type, std::move(val));
    return var_node;
}

std::unique_ptr<Function_Node> parse_fn_declaration()
{
    get_next_token();
    auto proto = parse_prototype();
    if (!proto)
        return nullptr;

    auto expressions = parse_fn_body();

    return std::make_unique<Function_Node>(std::move(proto), std::move(expressions), proto->get_arg_types());
}

std::unique_ptr<Prototype_Node> parse_prototype()
{
    if (cur_tok->type != Token_Types::tok_identifier)
        return error_p("Expected function name in prototype");

    std::string fn_name = cur_tok->value;

    get_next_token();

    if (cur_tok->type != Token_Types::tok_open_paren)
        return error_p("Expected '(' in prototype");

    get_next_token();
    std::vector<Variable_Types> arg_types;
    std::vector<std::string> arg_names;

    int param_counter = 0;
    while (cur_tok->type != Token_Types::tok_close_paren)
    {
        if (param_counter == 0)
        {
            arg_types.push_back(token_type_to_variable_type(cur_tok->type));
        }
        else if (param_counter == 1)
        {
            arg_names.push_back(cur_tok->value);
        }
        else if (param_counter == 2)
        {
            if (cur_tok->type == Token_Types::tok_comma)
            {
                param_counter = -1;
            }
        }

        get_next_token();
        param_counter++;
    }

    if (cur_tok->type != Token_Types::tok_close_paren)
        return error_p("Expected ')' in prototype");

    get_next_token();

    if (cur_tok->type != Token_Types::tok_arrow)
        return error_p("Expected '->' to indicate return type in prototype");

    get_next_token();

    Variable_Types return_type = token_type_to_variable_type(cur_tok->type);

    get_next_token();

    if (cur_tok->type != Token_Types::tok_open_curly_bracket)
    {
        char *err_msg;
        sprintf(err_msg, "Expected '{' on line %d position %d", cur_tok->row, cur_tok->col);
        return error_p(err_msg);
    }

    get_next_token();

    return std::make_unique<Prototype_Node>(fn_name, arg_types, arg_names, return_type);
}

std::vector<std::unique_ptr<Node>> parse_fn_body()
{
    std::vector<std::unique_ptr<Node>> nodes;
    bool ate_semicolon = false;
    while (cur_tok->type != Token_Types::tok_close_curly_bracket)
    {
        std::unique_ptr<Node> node = std::make_unique<Node>();
        switch (cur_tok->type)
        {
        case Token_Types::tok_if:
        {
            auto if_node = parse_if();
            ate_semicolon = true;
            node->type = Node_Types::IfNode;
            node->expression_node = std::move(if_node);
            break;
        }
        case Token_Types::tok_for:
        {
            auto for_node = parse_for();
            ate_semicolon = true;
            node->type = Node_Types::ForNode;
            node->expression_node = std::move(for_node);
            break;
        }
        case Token_Types::tok_i64:
        {
            auto var = parse_variable_declaration();
            ate_semicolon = true;
            node->type = Node_Types::VariableDeclarationNode;
            node->expression_node = std::move(var);
            break;
        }
        case Token_Types::tok_i32:
        {
            auto var = parse_variable_declaration();
            ate_semicolon = true;
            node->type = Node_Types::VariableDeclarationNode;
            node->expression_node = std::move(var);
            break;
        }
        case Token_Types::tok_i16:
        {
            auto var = parse_variable_declaration();
            ate_semicolon = true;
            node->type = Node_Types::VariableDeclarationNode;
            node->expression_node = std::move(var);
            break;
        }
        case Token_Types::tok_i8:
        {
            auto var = parse_variable_declaration();
            ate_semicolon = true;
            node->type = Node_Types::VariableDeclarationNode;
            node->expression_node = std::move(var);
            break;
        }
        case Token_Types::tok_float:
        {
            auto var = parse_variable_declaration();
            ate_semicolon = true;
            node->type = Node_Types::VariableDeclarationNode;
            node->expression_node = std::move(var);
            break;
        }
        case Token_Types::tok_double:
        {
            auto var = parse_variable_declaration();
            ate_semicolon = true;
            node->type = Node_Types::VariableDeclarationNode;
            node->expression_node = std::move(var);
            break;
        }
        case Token_Types::tok_bool:
        {
            auto var = parse_variable_declaration();
            ate_semicolon = true;
            node->type = Node_Types::VariableDeclarationNode;
            node->expression_node = std::move(var);
            break;
        }
        case Token_Types::tok_return:
        {
            auto ret = parse_return_statement();
            ate_semicolon = true;
            node->type = Node_Types::ReturnNode;
            node->return_node = std::move(ret);
            break;
        }
        case Token_Types::tok_toi64:
        {
            auto ret = parse_typecast_expression();
            ate_semicolon = true;
            node->type = Node_Types::TypeCastNode;
            node->expression_node = std::move(ret);
            break;
        }
        case Token_Types::tok_toi32:
        {
            auto ret = parse_typecast_expression();
            ate_semicolon = true;
            node->type = Node_Types::TypeCastNode;
            node->expression_node = std::move(ret);
            break;
        }
        case Token_Types::tok_toi16:
        {
            auto ret = parse_typecast_expression();
            ate_semicolon = true;
            node->type = Node_Types::TypeCastNode;
            node->expression_node = std::move(ret);
            break;
        }
        case Token_Types::tok_toi8:
        {
            auto ret = parse_typecast_expression();
            ate_semicolon = true;
            node->type = Node_Types::TypeCastNode;
            node->expression_node = std::move(ret);
            break;
        }
        case Token_Types::tok_object:
        {
            auto object = parse_object();
            ate_semicolon = true;
            node->type = Node_Types::ObjectNode;
            node->expression_node = std::move(object);
            break;
        }
        case Token_Types::tok_identifier:
        {
            auto tok_val = cur_tok->value;
            auto id = parse_identifier_expression();
            node->type = id->node_type;
            switch (id->node_type)
            {
            case Node_Types::CallExpressionNode:
                ate_semicolon = false;
                break;
            case Node_Types::AssignmentNode:
                ate_semicolon = true;
                break;
            case Node_Types::VariableDeclarationNode:
                ate_semicolon = true;
                break;
            default:
                ate_semicolon = true;
                break;
            }

            node->expression_node = std::move(id);
            break;
        }
        case Token_Types::tok_string:
        {
            auto var = parse_variable_declaration();
            ate_semicolon = true;
            node->type = Node_Types::VariableDeclarationNode;
            node->expression_node = std::move(var);
            break;
        }
        default:
            break;
        }
        nodes.push_back(std::move(node));
        if (!ate_semicolon)
        {
            get_next_token();
        }
    }

    return nodes;
}

std::unique_ptr<Expression_Node> parse_expression(bool needs_semicolon, Variable_Types type)
{
    auto lhs = parse_primary(type, needs_semicolon);
    if (!lhs)
    {
        error("Error parsing primary");
        return nullptr;
    };
    auto bin_op_rhs = parse_bin_op_rhs(0, std::move(lhs), type);
    if (needs_semicolon)
        get_next_token();
    return bin_op_rhs;
}

std::unique_ptr<Expression_Node> parse_primary(Variable_Types type, bool needs_semicolon)
{
    switch (cur_tok->type)
    {
    case Token_Types::tok_identifier:
        return parse_identifier_expression(needs_semicolon);
    case Token_Types::tok_number:
        return parse_number_expression(type);
    case Token_Types::tok_string_lit:
        return parse_string_expression();
    case Token_Types::tok_open_curly_bracket:
        return parse_object_expression();
    case Token_Types::tok_toi64:
        return parse_typecast_expression();
    case Token_Types::tok_toi32:
        return parse_typecast_expression();
    case Token_Types::tok_toi16:
        return parse_typecast_expression();
    case Token_Types::tok_toi8:
        return parse_typecast_expression();
    default:
        break;
    }
}

std::unique_ptr<Expression_Node> parse_string_expression()
{
    auto with_quotes = cur_tok->value;
    auto value = with_quotes.substr(1, with_quotes.size() - 2);
    get_next_token();
    return std::make_unique<String_Expression>(value);
}

std::unique_ptr<Expression_Node> parse_identifier_expression(bool needs_semicolon)
{
    //? object_type_name and object_type are only used if it ends up being an object variable declaration
    std::string object_type_name = toks[tok_pointer - 1]->value;
    Token_Types object_type = toks[tok_pointer - 1]->type;
    std::string id_name = cur_tok->value;

    get_next_token();

    if (cur_tok->type == Token_Types::tok_eq)
    {
        auto prev = toks[tok_pointer - 2]->type;

        if (prev != Token_Types::tok_i64 || prev != Token_Types::tok_i32 || prev != Token_Types::tok_i16 || prev != Token_Types::tok_i8 || prev != Token_Types::tok_float || prev != Token_Types::tok_double || prev != Token_Types::tok_string)
        {
            get_next_token();
            auto expr = parse_expression(needs_semicolon);
            if (expr->node_type == Node_Types::ObjectExpressionNode)
            {
                auto ty = token_type_to_variable_type(object_type);
                auto var = std::make_unique<Variable_Node>(id_name, ty, object_type_name, std::move(expr));
                var->node_type = Node_Types::VariableDeclarationNode;
                return std::move(var);
            }
            auto assignment_node = std::make_unique<Assignment_Node>(id_name, std::move(expr));
            assignment_node->node_type = Node_Types::AssignmentNode;
            return assignment_node;
        }
    }

    if (cur_tok->type != Token_Types::tok_open_paren)
        return std::make_unique<Variable_Expression_Node>(id_name);

    get_next_token();

    std::vector<std::unique_ptr<Expression_Node>> args;
    if (cur_tok->type != Token_Types::tok_close_paren)
    {
        while (true)
        {
            if (auto arg = parse_expression(false))
            {
                args.push_back(std::move(arg));
            }
            else
            {
                error("Error parsing function call parameters");
                return nullptr;
            }

            if (cur_tok->type == Token_Types::tok_close_paren)
                break;
            if (cur_tok->type != Token_Types::tok_comma)
                return error("Expected ')' or ',' in argument list");

            get_next_token();
        }
    }

    get_next_token();
    auto call_node = std::make_unique<Call_Expression_Node>(id_name, std::move(args));
    call_node->node_type = Node_Types::CallExpressionNode;
    return call_node;
}

std::unique_ptr<Expression_Node> parse_number_expression(Variable_Types type)
{
    auto number_expression = std::make_unique<Number_Expression_Node>(std::stod(cur_tok->value), type);
    get_next_token();
    return std::move(number_expression);
}

std::unique_ptr<Expression_Node> parse_bin_op_rhs(int expr_prec, std::unique_ptr<Expression_Node> lhs, Variable_Types type)
{
    while (true)
    {
        int tok_prec = get_tok_precedence();
        if (tok_prec < expr_prec)
        {
            return lhs;
        }

        std::string bin_op = cur_tok->value;

        get_next_token();

        auto rhs = parse_primary(type);
        if (!rhs)
        {
            error("Error parsing right hand side");
            return nullptr;
        }

        int next_prec = get_tok_precedence();
        if (tok_prec < next_prec)
        {
            rhs = parse_bin_op_rhs(tok_prec + 1, std::move(rhs));
            if (!rhs)
                return nullptr;
        }

        lhs = std::make_unique<Binary_Expression_Node>(bin_op, std::move(lhs), std::move(rhs));
    }
}

std::unique_ptr<Return_Node> parse_return_statement()
{
    get_next_token();
    auto expr = parse_expression(true, Variable_Types::type_i32);

    if (expr == 0)
    {
        error("Error parsing return expression");
        return nullptr;
    }

    return std::make_unique<Return_Node>(std::move(expr));
}

std::unique_ptr<Expression_Node> parse_typecast_expression()
{
    auto type = token_type_to_variable_type(cur_tok->type);
    get_next_token();
    get_next_token();

    auto expr = parse_expression(false);

    get_next_token();

    auto node = std::make_unique<Type_Cast_Node>(std::move(expr), type);

    return node;
}

std::unique_ptr<Expression_Node> parse_if()
{
    get_next_token(); //? eat 'if'
    get_next_token(); //? eat '('

    std::vector<std::unique_ptr<Condition_Expression>> conditions;
    std::vector<Token_Types> condition_seperators;

    while (cur_tok->type != Token_Types::tok_close_paren)
    {
        auto lhs = parse_expression(false);

        auto op = cur_tok->type;

        get_next_token(); //? eat operator

        auto rhs = parse_expression(false);

        auto condition = std::make_unique<Condition_Expression>(std::move(lhs), op, std::move(rhs));
        conditions.push_back(std::move(condition));

        if (cur_tok->type == Token_Types::tok_and || cur_tok->type == Token_Types::tok_or)
        {
            condition_seperators.push_back(cur_tok->type);
            get_next_token();
        }
    }

    get_next_token(); //? eat ')'
    get_next_token(); //? eat '{'

    auto then = parse_fn_body();

    get_next_token(); //? eat '}'

    auto if_node = std::make_unique<If_Node>(std::move(conditions), condition_seperators, std::move(then));
    return if_node;
}

std::unique_ptr<Expression_Node> parse_import()
{
    get_next_token(); //? eat 'import'
    std::string path_with_quotes = cur_tok->value;
    std::string path = path_with_quotes.substr(1, path_with_quotes.size() - 2);
    get_next_token(); //? eat string
    if (cur_tok->type != Token_Types::tok_semicolon)
        return error("Expected semicolon");
    get_next_token(); //? eat ';'
    return std::make_unique<Import_Node>(path);
}

std::unique_ptr<Expression_Node> parse_for()
{
    get_next_token(); //? eat 'for'
    if (cur_tok->type != Token_Types::tok_open_paren)
        return error("Expected open parentheses");
    get_next_token(); //? eat '('

    auto var = parse_variable_declaration();
    auto lhs = parse_expression();
    auto op = Token_Types::tok_compare_eq;
    auto rhs = std::make_unique<Number_Expression_Node>(1, Variable_Types::type_bool);
    auto condition = std::make_unique<Condition_Expression>(std::move(lhs), op, std::move(rhs));
    auto action = parse_expression(false);

    if (cur_tok->type != Token_Types::tok_close_paren)
        return error("Expected closing parentheses");

    get_next_token(); //? eat ')'
    get_next_token(); //? eat '{'

    auto body = parse_fn_body();

    get_next_token(); //? eat '}'

    return std::make_unique<For_Node>(std::move(var), std::move(condition), std::move(action), std::move(body));
}

std::unique_ptr<Expression_Node> parse_object()
{
    get_next_token(); //? eat 'object'

    std::string name = cur_tok->value;

    get_next_token(); //? eat name;

    get_next_token(); //? eat '{'

    if (cur_tok->type == Token_Types::tok_close_curly_bracket)
    {
        return error("Cannot declare object type with no properties");
    }

    std::map<std::string, Variable_Types> properties;
    int i = 0;
    Variable_Types cur_property_type;
    std::string cur_property_name;
    while (cur_tok->type != Token_Types::tok_close_curly_bracket)
    {
        //? Expect a property type
        if (i == 0)
        {
            cur_property_type = token_type_to_variable_type(cur_tok->type);
            i++;
            get_next_token();
            continue;
        }
        //? Expect a name
        else if (i == 1)
        {
            cur_property_name = cur_tok->value;
            i++;
            get_next_token();
            continue;
        }
        //? Expect semicolon
        else if (i == 2)
        {
            i = 0;
            properties[cur_property_name] = cur_property_type;
            get_next_token();
            continue;
        }
    }

    get_next_token(); //? eat '}'
    get_next_token(); //? eat ';'

    auto obj_node = std::make_unique<Object_Node>(name, properties);
    obj_node->node_type = Node_Types::ObjectNode;
    return std::move(obj_node);
}

std::unique_ptr<Expression_Node> parse_object_expression()
{
    get_next_token(); //? eat '{'

    std::map<std::string, std::unique_ptr<Expression_Node>> properties;
    int i = 0;
    std::unique_ptr<Expression_Node> cur_property_value;
    std::string cur_property_name;
    while (cur_tok->type != Token_Types::tok_close_curly_bracket)
    {
        //? Expect a name
        if (i == 0)
        {
            cur_property_name = cur_tok->value;
            i++;
            get_next_token();
            continue;
        }
        //? Expect a colon
        else if (i == 1)
        {
            i++;
            get_next_token();
            continue;
        }
        //? Expect value
        else if (i == 2)
        {
            auto expr = parse_expression();
            cur_property_value = std::move(expr);
            i = 0;
            properties[cur_property_name] = std::move(cur_property_value);
            continue;
        }
    }

    get_next_token(); //? eat '}'

    auto obj_init_node = std::make_unique<Object_Expression_Node>(std::move(properties));
    obj_init_node->node_type = Node_Types::ObjectExpressionNode;
    return std::move(obj_init_node);
}

Variable_Types token_type_to_variable_type(Token_Types type)
{
    switch (type)
    {
    case Token_Types::tok_i64:
        return Variable_Types::type_i64;
    case Token_Types::tok_i32:
        return Variable_Types::type_i32;
    case Token_Types::tok_i16:
        return Variable_Types::type_i16;
    case Token_Types::tok_i8:
        return Variable_Types::type_i8;
    case Token_Types::tok_float:
        return Variable_Types::type_float;
    case Token_Types::tok_double:
        return Variable_Types::type_double;
    case Token_Types::tok_string:
        return Variable_Types::type_string;
    case Token_Types::tok_bool:
        return Variable_Types::type_bool;
    case Token_Types::tok_toi64:
        return Variable_Types::type_i64;
    case Token_Types::tok_toi32:
        return Variable_Types::type_i32;
    case Token_Types::tok_toi16:
        return Variable_Types::type_i16;
    case Token_Types::tok_toi8:
        return Variable_Types::type_i8;
    case Token_Types::tok_identifier:
        return Variable_Types::type_object;
    default:
        break;
    }
}

void get_next_token()
{
    tok_pointer++;
    cur_tok = toks[tok_pointer];
}

std::unique_ptr<Expression_Node> error(const char *str)
{
    fprintf(stderr, "LogError: %s\n", str);
    exit(1);
}

std::unique_ptr<Prototype_Node> error_p(const char *str)
{
    error(str);
    exit(1);
}

int get_tok_precedence()
{
    int tok_prec = bin_op_precedence[cur_tok->value];
    if (tok_prec <= 0)
        return -1;
    return tok_prec;
}

void Function_Node::set_variables(std::string name, llvm::Value *var)
{
    variables[name] = var;
}

llvm::Value *Function_Node::get_variable(std::string name)
{
    return variables[name];
}

std::vector<Variable_Types> Prototype_Node::get_arg_types()
{
    return arg_types;
}

std::unique_ptr<Prototype_Node> Function_Node::get_proto()
{
    return std::move(proto);
}

std::vector<Variable_Types> Function_Node::get_arg_types()
{
    return arg_types;
}

std::string Prototype_Node::get_name() { return name; }

std::unique_ptr<Expression_Node> Condition_Expression::get_lhs() { return std::move(lhs); };
std::unique_ptr<Expression_Node> Condition_Expression::get_rhs() { return std::move(rhs); };
Token_Types Condition_Expression::get_op() { return op; }
Variable_Types Prototype_Node::get_return_type() { return return_type; };
llvm::Value *Function_Node::get_return_value_ptr() { return return_value_ptr; };
llvm::BasicBlock *Function_Node::get_end_bb() { return end_bb; };
std::map<std::string, std::unique_ptr<Expression_Node>> Object_Expression_Node::get_properties() { return std::move(properties); };
std::map<std::string, std::unique_ptr<Expression_Node>> Variable_Expression_Node::get_properties() { return; };
std::map<std::string, std::unique_ptr<Expression_Node>> Binary_Expression_Node::get_properties() { return; };
std::map<std::string, std::unique_ptr<Expression_Node>> Call_Expression_Node::get_properties() { return; };
std::map<std::string, std::unique_ptr<Expression_Node>> Variable_Node::get_properties() { return; };
std::map<std::string, std::unique_ptr<Expression_Node>> Type_Cast_Node::get_properties() { return; };
std::map<std::string, std::unique_ptr<Expression_Node>> Assignment_Node::get_properties() { return; };
std::map<std::string, std::unique_ptr<Expression_Node>> If_Node::get_properties() { return; };
std::map<std::string, std::unique_ptr<Expression_Node>> Import_Node::get_properties() { return; };
std::map<std::string, std::unique_ptr<Expression_Node>> String_Expression::get_properties() { return; };
std::map<std::string, std::unique_ptr<Expression_Node>> For_Node::get_properties() { return; };
std::map<std::string, std::unique_ptr<Expression_Node>> Object_Node::get_properties() { return; };