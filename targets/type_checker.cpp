#include "targets/type_checker.h"

#include <cdk/types/primitive_type.h>

#include <string>

#include ".auto/all_nodes.h"  // automatically generated
#include "til_parser.tab.h"

#define ASSERT_UNSPEC                                                             \
    {                                                                             \
        if (node->type() != nullptr && !node->is_typed(cdk::TYPE_UNSPEC)) return; \
    }

/*Auxiliar function to see if the types are the same*/

bool til::type_checker::typeComparison(std::shared_ptr<cdk::basic_type> left,
                                       std::shared_ptr<cdk::basic_type> right, bool value) {
    if (left->name() == cdk::TYPE_UNSPEC || right->name() == cdk::TYPE_UNSPEC) {
        return false;
    } else if (left->name() == cdk::TYPE_FUNCTIONAL) {
        if (right->name() != cdk::TYPE_FUNCTIONAL) {
            return false;
        }

        auto leftFunc = cdk::functional_type::cast(left);
        auto rightFunc = cdk::functional_type::cast(right);

        if (leftFunc->input_length() != rightFunc->input_length() || leftFunc->output_length() != rightFunc->output_length()) {
            return false;
        }

        for (size_t i = 0; i < leftFunc->input_length(); i++) {
            if (!typeComparison(rightFunc->input(i), leftFunc->input(i), value)) {
                return false;
            }
        }

        for (size_t i = 0; i < leftFunc->output_length(); i++) {
            if (!typeComparison(leftFunc->output(i), rightFunc->output(i), value)) {
                return false;
            }
        }

        return true;
    } else if (right->name() == cdk::TYPE_FUNCTIONAL) {
        return false;
    } else if (left->name() == cdk::TYPE_POINTER) {
        if (right->name() != cdk::TYPE_POINTER) {
            return false;
        }

        return typeComparison(cdk::reference_type::cast(left)->referenced(),
                              cdk::reference_type::cast(right)->referenced(), false);
    } else if (right->name() == cdk::TYPE_POINTER) {
        return false;
    } else if (value && left->name() == cdk::TYPE_DOUBLE) {
        return right->name() == cdk::TYPE_DOUBLE || right->name() == cdk::TYPE_INT;
    } else {
        return left == right;
    }
}

//---------------------------------------------------------------------------

void til::type_checker::do_sequence_node(cdk::sequence_node *const node, int lvl) {
    // EMPTY
}

//---------------------------------------------------------------------------

void til::type_checker::do_nil_node(cdk::nil_node *const node, int lvl) {
    // EMPTY
}
void til::type_checker::do_data_node(cdk::data_node *const node, int lvl) {
    // EMPTY
}
void til::type_checker::do_double_node(cdk::double_node *const node, int lvl) {
    // EMPTY
}
void til::type_checker::do_not_node(cdk::not_node *const node, int lvl) {
    // EMPTY
}
void til::type_checker::do_and_node(cdk::and_node *const node, int lvl) {
    // EMPTY
}
void til::type_checker::do_or_node(cdk::or_node *const node, int lvl) {
    // EMPTY
}

//---------------------------------------------------------------------------

void til::type_checker::do_integer_node(cdk::integer_node *const node, int lvl) {
    ASSERT_UNSPEC;
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void til::type_checker::do_string_node(cdk::string_node *const node, int lvl) {
    ASSERT_UNSPEC;
    node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
}

//---------------------------------------------------------------------------

void til::type_checker::processUnaryExpression(cdk::unary_operation_node *const node, int lvl) {
    node->argument()->accept(this, lvl + 2);
    if (!node->argument()->is_typed(cdk::TYPE_INT)) throw std::string("wrong type in argument of unary expression");

    // in Simple, expressions are always int
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void til::type_checker::do_unary_minus_node(cdk::unary_minus_node *const node, int lvl) {
    processUnaryExpression(node, lvl);
}

void til::type_checker::do_unary_plus_node(cdk::unary_plus_node *const node, int lvl) {
    processUnaryExpression(node, lvl);
}

//---------------------------------------------------------------------------

void til::type_checker::processBinaryExpression(cdk::binary_operation_node *const node, int lvl) {
    ASSERT_UNSPEC;
    node->left()->accept(this, lvl + 2);
    if (!node->left()->is_typed(cdk::TYPE_INT)) throw std::string("wrong type in left argument of binary expression");

    node->right()->accept(this, lvl + 2);
    if (!node->right()->is_typed(cdk::TYPE_INT)) throw std::string("wrong type in right argument of binary expression");

    // in Simple, expressions are always int
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void til::type_checker::do_add_node(cdk::add_node *const node, int lvl) {
    processBinaryExpression(node, lvl);
}
void til::type_checker::do_sub_node(cdk::sub_node *const node, int lvl) {
    processBinaryExpression(node, lvl);
}
void til::type_checker::do_mul_node(cdk::mul_node *const node, int lvl) {
    processBinaryExpression(node, lvl);
}
void til::type_checker::do_div_node(cdk::div_node *const node, int lvl) {
    processBinaryExpression(node, lvl);
}
void til::type_checker::do_mod_node(cdk::mod_node *const node, int lvl) {
    processBinaryExpression(node, lvl);
}
void til::type_checker::do_lt_node(cdk::lt_node *const node, int lvl) {
    processBinaryExpression(node, lvl);
}
void til::type_checker::do_le_node(cdk::le_node *const node, int lvl) {
    processBinaryExpression(node, lvl);
}
void til::type_checker::do_ge_node(cdk::ge_node *const node, int lvl) {
    processBinaryExpression(node, lvl);
}
void til::type_checker::do_gt_node(cdk::gt_node *const node, int lvl) {
    processBinaryExpression(node, lvl);
}
void til::type_checker::do_ne_node(cdk::ne_node *const node, int lvl) {
    processBinaryExpression(node, lvl);
}
void til::type_checker::do_eq_node(cdk::eq_node *const node, int lvl) {
    processBinaryExpression(node, lvl);
}

//---------------------------------------------------------------------------

void til::type_checker::do_address_of_node(til::address_of_node *const node, int lvl) {
    ASSERT_UNSPEC;

    node->lvalue()->accept(this, lvl + 2);
    if (node->lvalue()->is_typed(cdk::TYPE_POINTER)) {
        auto ref = cdk::reference_type::cast(node->lvalue()->type());
        if (ref->referenced()->name() == cdk::TYPE_VOID) {
            node->type(node->lvalue()->type());
            return;
        }
    }
    node->type(cdk::reference_type::create(4, node->lvalue()->type()));
}

void til::type_checker::do_alloc_node(til::alloc_node *const node, int lvl) {
    ASSERT_UNSPEC;

    node->argument()->accept(this, lvl + 2);

    if (node->argument()->is_typed(cdk::TYPE_UNSPEC)) {
        node->argument()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    } else if (!node->argument()->is_typed(cdk::TYPE_INT)) {
        throw std::string("wrong type in argument of unary expression");
    }

    node->type(cdk::reference_type::create(4, cdk::primitive_type::create(0, cdk::TYPE_UNSPEC)));
}

//---------------------------------------------------------------------------

void til::type_checker::do_variable_node(cdk::variable_node *const node, int lvl) {
    ASSERT_UNSPEC;
    const std::string &id = node->name();
    std::shared_ptr<til::symbol> symbol = _symtab.find(id);

    if (symbol != nullptr) {
        node->type(symbol->type());
    } else {
        throw id;
    }
}

void til::type_checker::do_pointer_index_node(til::pointer_index_node *const node, int lvl) {
    ASSERT_UNSPEC;

    node->base()->accept(this, lvl + 2);
    if (!node->base()->is_typed(cdk::TYPE_POINTER)) {
        throw std::string("wrong type in pointer index's base (expected pointer)");
    }

    node->index()->accept(this, lvl + 2);
    if (node->index()->is_typed(cdk::TYPE_UNSPEC)) {
        node->index()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    } else if (!node->index()->is_typed(cdk::TYPE_INT)) {
        throw std::string("wrong type in pointer index's index (expected integer)");
    }

    auto basetype = cdk::reference_type::cast(node->base()->type());

    if (basetype->referenced()->name() == cdk::TYPE_UNSPEC) {
        basetype = cdk::reference_type::create(4, cdk::primitive_type::create(4, cdk::TYPE_INT));
        node->base()->type(basetype);
    }

    node->type(basetype->referenced());
}

void til::type_checker::do_rvalue_node(cdk::rvalue_node *const node, int lvl) {
    ASSERT_UNSPEC;
    try {
        node->lvalue()->accept(this, lvl);
        node->type(node->lvalue()->type());
    } catch (const std::string &id) {
        throw "undeclared variable '" + id + "'";
    }
}

void til::type_checker::do_assignment_node(cdk::assignment_node *const node, int lvl) {
    ASSERT_UNSPEC;

    ASSERT_UNSPEC;

    node->lvalue()->accept(this, lvl);
    node->rvalue()->accept(this, lvl);

    if (node->rvalue()->is_typed(cdk::TYPE_UNSPEC)) {
        node->rvalue()->type(node->lvalue()->type());
    } else if (node->rvalue()->is_typed(cdk::TYPE_POINTER) && node->lvalue()->is_typed(cdk::TYPE_POINTER)) {
        auto lref = cdk::reference_type::cast(node->lvalue()->type());
        auto rref = cdk::reference_type::cast(node->rvalue()->type());

        if (rref->referenced()->name() == cdk::TYPE_UNSPEC || rref->referenced()->name() == cdk::TYPE_VOID || lref->referenced()->name() == cdk::TYPE_VOID) {
            node->rvalue()->type(node->lvalue()->type());
        }
    }

    if (node->lvalue()->type() != node->rvalue()->type()) {
        throw std::string("wrong type in right argument of assignment expression");
    }

    node->type(node->lvalue()->type());
}

//---------------------------------------------------------------------------

void til::type_checker::do_program_node(til::program_node *const node, int lvl) {
    node->block()->accept(this, lvl);
}

void til::type_checker::do_evaluation_node(til::evaluation_node *const node, int lvl) {
    node->argument()->accept(this, lvl);

    if (node->argument()->is_typed(cdk::TYPE_UNSPEC)) {
        node->argument()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    } else if (node->argument()->is_typed(cdk::TYPE_POINTER)) {
        auto ref = cdk::reference_type::cast(node->argument()->type());

        if (ref != nullptr && ref->referenced()->name() == cdk::TYPE_UNSPEC) {
            node->argument()->type(cdk::reference_type::create(4, cdk::primitive_type::create(4, cdk::TYPE_INT)));
        }
    }
}

void til::type_checker::do_print_node(til::print_node *const node, int lvl) {
    for (size_t i = 0; i < node->arguments()->size(); i++) {
        auto child = dynamic_cast<cdk::expression_node *>(node->arguments()->node(i));

        child->accept(this, lvl);

        if (child->is_typed(cdk::TYPE_UNSPEC)) {
            child->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
        } else if (!child->is_typed(cdk::TYPE_INT) && !child->is_typed(cdk::TYPE_DOUBLE) && !child->is_typed(cdk::TYPE_STRING)) {
            throw std::string("invalid argument type " + std::to_string(i + 1) + " of print instruction");
        }
    }
}

//---------------------------------------------------------------------------

void til::type_checker::do_read_node(til::read_node *const node, int lvl) {
    ASSERT_UNSPEC;

    node->type(cdk::primitive_type::create(0, cdk::TYPE_UNSPEC));
}

//---------------------------------------------------------------------------

void til::type_checker::do_while_node(til::while_node *const node, int lvl) {
    node->condition()->accept(this, lvl + 4);

    if (node->condition()->is_typed(cdk::TYPE_UNSPEC)) {
        node->condition()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    } else if (!node->condition()->is_typed(cdk::TYPE_INT)) {
        throw std::string("wrong type in condition of loop instruction");
    }
}

//---------------------------------------------------------------------------

void til::type_checker::do_if_node(til::if_node *const node, int lvl) {
    node->condition()->accept(this, lvl + 4);

    if (node->condition()->is_typed(cdk::TYPE_UNSPEC)) {
        node->condition()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    } else if (!node->condition()->is_typed(cdk::TYPE_INT)) {
        throw std::string("wrong type in condition of conditional instruction");
    }
}

void til::type_checker::do_if_else_node(til::if_else_node *const node, int lvl) {
    node->condition()->accept(this, lvl + 4);

    if (node->condition()->is_typed(cdk::TYPE_UNSPEC)) {
        node->condition()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    } else if (!node->condition()->is_typed(cdk::TYPE_INT)) {
        throw std::string("wrong type in condition of conditional instruction");
    }
}

//---------------------------------------------------------------------------

void til::type_checker::do_nullptr_node(til::nullptr_node *const node, int lvl) {
    ASSERT_UNSPEC;

    node->type(cdk::reference_type::create(4, cdk::primitive_type::create(0, cdk::TYPE_UNSPEC)));
}

//---------------------------------------------------------------------------

void til::type_checker::do_sizeof_node(til::sizeof_node *const node, int lvl) {
    ASSERT_UNSPEC;
    node->argument()->accept(this, lvl + 2);

    if (node->argument()->is_typed(cdk::TYPE_UNSPEC)) {
        node->argument()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    }

    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

//---------------------------------------------------------------------------

void til::type_checker::do_block_node(til::block_node *const node, int lvl) {
    // EMPTY
}

void til::type_checker::do_next_node(til::next_node *const node, int lvl) {
    // EMPTY
}

void til::type_checker::do_stop_node(til::stop_node *const node, int lvl) {
    // EMPTY
}

//---------------------------------------------------------------------------

void til::type_checker::do_return_node(til::return_node *const node, int lvl) {
    /*Checks the type declared in the function header*/
    auto symbol = _symtab.find("@", 1);
    if (symbol == nullptr) {
        throw std::string("return statement outside begin end block");
    }

    std::shared_ptr<cdk::functional_type> functype = cdk::functional_type::cast(symbol->type());

    auto returntype = functype->output(0);
    auto returntype_name = returntype->name();

    /* return has not expression*/
    if (node->returnval() == nullptr) {
        if (returntype_name != cdk::TYPE_VOID) {
            throw std::string("no return value  for non void function");
        }
        return;
    }

    /* return has expression*/

    if (returntype_name == cdk::TYPE_VOID) {
        throw std::string("return value for void function");
    }

    node->returnval()->accept(this, lvl + 2);

    if (!typeComparison(returntype, node->returnval()->type(), true)) {
        throw std::string("wrong type for return expression");
    }
}

void til::type_checker::do_function_node(til::function_node *const node, int lvl) {
    // type of function_node is set in the AST node's constructor

    auto function = til::make_symbol("@", node->type());

    if (!_symtab.insert(function->name(), function)) {
        // if it can't insert, it's because it already exists in local context
        _symtab.replace(function->name(), function);
    }
}

void til::type_checker::do_declaration_node(til::declaration_node *const node, int lvl) {
    if (node->type() == nullptr) {  // auto
        node->initializer()->accept(this, lvl + 2);

        if (node->initializer()->is_typed(cdk::TYPE_UNSPEC)) {
            node->initializer()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
        } else if (node->initializer()->is_typed(cdk::TYPE_POINTER)) {
            auto ref = cdk::reference_type::cast(node->initializer()->type());
            if (ref->referenced()->name() == cdk::TYPE_UNSPEC) {
                node->initializer()->type(cdk::reference_type::create(4,
                                                                      cdk::primitive_type::create(4, cdk::TYPE_INT)));
            }
        } else if (node->initializer()->is_typed(cdk::TYPE_VOID)) {
            throw std::string("cannot declare variable of type void");
        }

        node->type(node->initializer()->type());
    } else {  // not auto; node already has a type set
        if (node->initializer() != nullptr) {
            node->initializer()->accept(this, lvl + 2);

            if (node->initializer()->is_typed(cdk::TYPE_UNSPEC)) {
                if (node->is_typed(cdk::TYPE_DOUBLE)) {
                    node->initializer()->type(node->type());
                } else {
                    // if node->type() is not an int, a type mismatch error will be thrown later
                    node->initializer()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
                }
            } else if (node->initializer()->is_typed(cdk::TYPE_POINTER) && node->is_typed(cdk::TYPE_POINTER)) {
                auto noderef = cdk::reference_type::cast(node->type());
                auto initref = cdk::reference_type::cast(node->initializer()->type());
                if (initref->referenced()->name() == cdk::TYPE_UNSPEC || initref->referenced()->name() == cdk::TYPE_VOID || noderef->referenced()->name() == cdk::TYPE_VOID) {
                    node->initializer()->type(node->type());
                }
            }

            if (!typeComparison(node->type(), node->initializer()->type(), true)) {
                throw std::string("wrong type in initializer for variable '" + node->identifier() + "'");
            }
        }
    }

    if (node->qualifier() == tEXTERNAL && !node->is_typed(cdk::TYPE_FUNCTIONAL)) {
        throw std::string("foreign declaration of non-function '" + node->identifier() + "'");
    }

    auto symbol = make_symbol(node->identifier(), node->type(), node->qualifier());

    if (_symtab.insert(node->identifier(), symbol)) {
        _parent->set_new_symbol(symbol);
        return;
    }

    auto prev = _symtab.find(node->identifier());

    if (prev != nullptr && prev->qualifier() == tFORWARD) {
        if (typeComparison(prev->type(), symbol->type(), false)) {
            _symtab.replace(node->identifier(), symbol);
            _parent->set_new_symbol(symbol);
            return;
        }
    }

    throw std::string("redeclaration of variable '" + node->identifier() + "'");
}

void til::type_checker::do_function_call_node(til::function_call_node *const node, int lvl) {
    ASSERT_UNSPEC;

    std::shared_ptr<cdk::functional_type> functype;

    if (node->func() == nullptr) {  // recursive
        auto symbol = _symtab.find("@", 1);
        if (symbol == nullptr) {
            throw std::string("recursive call outside function");
        }

        functype = cdk::functional_type::cast(symbol->type());
    } else {
        node->func()->accept(this, lvl);

        if (!node->func()->is_typed(cdk::TYPE_FUNCTIONAL)) {
            throw std::string("wrong type in function call");
        }
    }
    if (functype->input()->length() != node->arguments()->size()) {
        throw std::string("wrong number of arguments in function call");
    }

    for (size_t i = 0; i < node->arguments()->size(); i++) {
        auto arg = dynamic_cast<cdk::expression_node *>(node->arguments()->node(i));
        arg->accept(this, lvl);

        auto paramtype = functype->input(i);

        if (arg->is_typed(cdk::TYPE_UNSPEC)) {
            if (paramtype->name() == cdk::TYPE_DOUBLE) {
                arg->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
            } else {
                arg->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
            }
        } else if (arg->is_typed(cdk::TYPE_POINTER) && paramtype->name() == cdk::TYPE_POINTER) {
            auto paramref = cdk::reference_type::cast(paramtype);
            auto argref = cdk::reference_type::cast(arg->type());

            if (argref->referenced()->name() == cdk::TYPE_UNSPEC || argref->referenced()->name() == cdk::TYPE_VOID || paramref->referenced()->name() == cdk::TYPE_VOID) {
                arg->type(paramtype);
            }
        }

        if (!typeComparison(paramtype, arg->type(), true)) {
            throw std::string("wrong type for argument " + std::to_string(i + 1) + " in function call");
        }
    }

    // note this may result in this node being typed TYPE_VOID
    node->type(functype->output(0));
}
