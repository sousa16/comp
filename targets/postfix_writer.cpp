#include "targets/postfix_writer.h"

#include <sstream>
#include <string>

#include ".auto/all_nodes.h"  // all_nodes.h is automatically generated
#include "targets/type_checker.h"
#include "til_parser.tab.h"

//---------------------------------------------------------------------------

void til::postfix_writer::do_nil_node(cdk::nil_node* const node, int lvl) {
    // EMPTY
}
void til::postfix_writer::do_data_node(cdk::data_node* const node, int lvl) {
    // EMPTY
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_sequence_node(cdk::sequence_node* const node, int lvl) {
    for (size_t i = 0; i < node->size(); i++) {
        node->node(i)->accept(this, lvl);
    }
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_integer_node(cdk::integer_node* const node, int lvl) {
    _pf.INT(node->value());  // push an integer

    /*
     * if (inFunction()) {
        _pf.INT(node->value());  // push an integer
    } else {
        _pf.SINT(node->value());
    }
     */
}

void til::postfix_writer::do_double_node(cdk::double_node* const node, int lvl) {
    _pf.DOUBLE(node->value());  // push a double

    /*
    if (inFunction()) {
        _pf.DOUBLE(node->value());
    } else {
        _pf.SDOUBLE(node->value());
    }
    }
    */
}

void til::postfix_writer::do_string_node(cdk::string_node* const node, int lvl) {
    int lbl1;

    /* generate the string */
    _pf.RODATA();                     // strings are DATA readonly
    _pf.ALIGN();                      // make sure we are aligned
    _pf.LABEL(mklbl(lbl1 = ++_lbl));  // give the string a name
    _pf.SSTRING(node->value());       // output string characters

    /* leave the address on the stack */
    _pf.TEXT();             // return to the TEXT segment
    _pf.ADDR(mklbl(lbl1));  // the string to be printed

    /*
    if (inFunction()) {
        // leave the address on the stack
        _pf.TEXT(_functionLabels.top());  // return to the TEXT segment
        _pf.ADDR(mklbl(lbl1));            // the string to be stored
    } else {
        _pf.DATA();              // return to the DATA segment
        _pf.SADDR(mklbl(lbl1));  // the string to be stored
    }
    */
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_and_node(cdk::and_node* const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;

    int lbl;
    node->left()->accept(this, lvl);
    _pf.DUP32();
    _pf.JZ(mklbl(lbl = ++_lbl));  // short circuit
    node->right()->accept(this, lvl);
    _pf.AND();
    _pf.ALIGN();
    _pf.LABEL(mklbl(lbl));
}
void til::postfix_writer::do_or_node(cdk::or_node* const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;

    int lbl;
    node->left()->accept(this, lvl);
    _pf.DUP32();
    _pf.JNZ(mklbl(lbl = ++_lbl));  // short circuit
    node->right()->accept(this, lvl);
    _pf.OR();
    _pf.ALIGN();
    _pf.LABEL(mklbl(lbl));
}

void til::postfix_writer::do_not_node(cdk::not_node* const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;

    node->argument()->accept(this, lvl + 2);
    _pf.INT(0);
    _pf.EQ();
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_unary_minus_node(cdk::unary_minus_node* const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;
    node->argument()->accept(this, lvl);  // determine the value
    _pf.NEG();                            // 2-complement
}

void til::postfix_writer::do_unary_plus_node(cdk::unary_plus_node* const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;
    node->argument()->accept(this, lvl);  // determine the value
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_add_node(cdk::add_node* const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;

    node->left()->accept(this, lvl);
    if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) {
        _pf.I2D();
    } else if (node->is_typed(cdk::TYPE_POINTER) && node->left()->is_typed(cdk::TYPE_INT)) {
        auto ref = cdk::reference_type::cast(node->type());
        _pf.INT(std::max(static_cast<size_t>(1), ref->referenced()->size()));
        _pf.MUL();
    }

    node->right()->accept(this, lvl);
    if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
        _pf.I2D();
    } else if (node->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT)) {
        auto ref = cdk::reference_type::cast(node->type());
        _pf.INT(std::max(static_cast<size_t>(1), ref->referenced()->size()));
        _pf.MUL();
    }

    if (node->is_typed(cdk::TYPE_DOUBLE)) {
        _pf.DADD();
    } else {
        _pf.ADD();
    }
}
void til::postfix_writer::do_sub_node(cdk::sub_node* const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;

    node->left()->accept(this, lvl);
    if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) {
        _pf.I2D();
    } else if (node->is_typed(cdk::TYPE_POINTER) && node->left()->is_typed(cdk::TYPE_INT)) {
        auto ref = cdk::reference_type::cast(node->type());
        _pf.INT(std::max(static_cast<size_t>(1), ref->referenced()->size()));
        _pf.MUL();
    }

    node->right()->accept(this, lvl);
    if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
        _pf.I2D();
    } else if (node->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT)) {
        auto ref = cdk::reference_type::cast(node->type());
        _pf.INT(std::max(static_cast<size_t>(1), ref->referenced()->size()));
        _pf.MUL();
    }

    if (node->is_typed(cdk::TYPE_DOUBLE)) {
        _pf.DSUB();
    } else {
        _pf.SUB();
    }

    if (node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_POINTER)) {
        // the difference between two pointers must be divided by the size of what they're referencing
        auto lref = cdk::reference_type::cast(node->left()->type());
        _pf.INT(std::max(static_cast<size_t>(1), lref->referenced()->size()));
        _pf.DIV();
    }
}

void til::postfix_writer::prepareIDBinaryExpression(cdk::binary_operation_node* const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;

    node->left()->accept(this, lvl);
    if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
        _pf.I2D();
    }

    node->right()->accept(this, lvl);
    if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
        _pf.I2D();
    }
}

void til::postfix_writer::do_mul_node(cdk::mul_node* const node, int lvl) {
    prepareIDBinaryExpression(node, lvl);

    if (node->is_typed(cdk::TYPE_DOUBLE)) {
        _pf.DMUL();
    } else {
        _pf.MUL();
    }
}
void til::postfix_writer::do_div_node(cdk::div_node* const node, int lvl) {
    prepareIDBinaryExpression(node, lvl);

    if (node->is_typed(cdk::TYPE_DOUBLE)) {
        _pf.DDIV();
    } else {
        _pf.DIV();
    }
}
void til::postfix_writer::do_mod_node(cdk::mod_node* const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;
    node->left()->accept(this, lvl);
    node->right()->accept(this, lvl);
    _pf.MOD();
}

void til::postfix_writer::prepareIDBinaryComparisonExpression(cdk::binary_operation_node* const node, int lvl) {
    prepareIDBinaryExpression(node, lvl);

    if (node->left()->is_typed(cdk::TYPE_DOUBLE) || node->right()->is_typed(cdk::TYPE_DOUBLE)) {
        _pf.DCMP();
        _pf.INT(0);
    }
}

void til::postfix_writer::do_lt_node(cdk::lt_node* const node, int lvl) {
    prepareIDBinaryComparisonExpression(node, lvl);
    _pf.LT();
}
void til::postfix_writer::do_le_node(cdk::le_node* const node, int lvl) {
    prepareIDBinaryComparisonExpression(node, lvl);
    _pf.LE();
}
void til::postfix_writer::do_ge_node(cdk::ge_node* const node, int lvl) {
    prepareIDBinaryComparisonExpression(node, lvl);
    _pf.GE();
}
void til::postfix_writer::do_gt_node(cdk::gt_node* const node, int lvl) {
    prepareIDBinaryComparisonExpression(node, lvl);
    _pf.GT();
}
void til::postfix_writer::do_ne_node(cdk::ne_node* const node, int lvl) {
    prepareIDBinaryComparisonExpression(node, lvl);
    _pf.NE();
}
void til::postfix_writer::do_eq_node(cdk::eq_node* const node, int lvl) {
    prepareIDBinaryComparisonExpression(node, lvl);
    _pf.EQ();
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_address_of_node(til::address_of_node* const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;
    node->lvalue()->accept(this, lvl + 2);
}

void til::postfix_writer::do_alloc_node(til::alloc_node* const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;

    // Get the type of the node's argument
    auto argType = node->argument()->type();

    // If the argument type is a reference type, get the size of the referenced type
    size_t size = 1;
    if (argType->name() == cdk::TYPE_POINTER) {
        auto refType = cdk::reference_type::cast(argType);
        size = refType->referenced()->size();
    }

    node->argument()->accept(this, lvl);
    _pf.INT(std::max(static_cast<size_t>(1), size));
    _pf.MUL();
    _pf.ALLOC();
    _pf.SP();
}
//---------------------------------------------------------------------------

void til::postfix_writer::do_variable_node(cdk::variable_node* const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;
    auto symbol = _symtab.find(node->name());  // type checker already ensured symbol exists

    if (symbol->qualifier() == tEXTERNAL) {
        _externalFunctionName = symbol->name();
    } else if (symbol->global()) {
        _pf.ADDR(node->name());
    } else {
        _pf.LOCAL(symbol->offset());
    }
}

void til::postfix_writer::do_pointer_index_node(til::pointer_index_node* const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;
    node->base()->accept(this, lvl);
    node->index()->accept(this, lvl);
    _pf.INT(node->type()->size());
    _pf.MUL();
    _pf.ADD();
}

void til::postfix_writer::do_rvalue_node(cdk::rvalue_node* const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;
    node->lvalue()->accept(this, lvl);

    if (_externalFunctionName) {
        return;  // name passed through this field; nothing in stack to be loaded
    }

    if (node->is_typed(cdk::TYPE_DOUBLE)) {
        _pf.LDDOUBLE();
    } else {
        _pf.LDINT();  // non-ints are int-sized too
    }
}

void til::postfix_writer::do_assignment_node(cdk::assignment_node* const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;
    node->rvalue()->accept(this, lvl);  // determine the new value
    _pf.DUP32();
    if (new_symbol() == nullptr) {
        node->lvalue()->accept(this, lvl);  // where to store the value
    } else {
        _pf.DATA();                       // variables are all global and live in DATA
        _pf.ALIGN();                      // make sure we are aligned
        _pf.LABEL(new_symbol()->name());  // name variable location
        reset_new_symbol();
        _pf.SINT(0);                        // initialize it to 0 (zero)
        _pf.TEXT();                         // return to the TEXT segment
        node->lvalue()->accept(this, lvl);  // DAVID: bah!
    }
    _pf.STINT();  // store the value at address
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_program_node(til::program_node* const node, int lvl) {
    // Note that Simple doesn't have functions. Thus, it doesn't need
    // a function node. However, it must start in the main function.
    // The ProgramNode (representing the whole program) doubles as a
    // main function node.

    // generate the main function (RTS mandates that its name be "_main")
    _pf.TEXT();
    _pf.ALIGN();
    _pf.GLOBAL("_main", _pf.FUNC());
    _pf.LABEL("_main");
    _pf.ENTER(0);  // Simple doesn't implement local variables

    node->block()->accept(this, lvl);

    // end the main function
    _pf.INT(0);
    _pf.STFVAL32();
    _pf.LEAVE();
    _pf.RET();

    // these are just a few library function imports
    _pf.EXTERN("readi");
    _pf.EXTERN("printi");
    _pf.EXTERN("prints");
    _pf.EXTERN("println");
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_evaluation_node(til::evaluation_node* const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;

    node->argument()->accept(this, lvl);

    if (node->argument()->type()->size() > 0) {
        _pf.TRASH(node->argument()->type()->size());
    }
}

void til::postfix_writer::do_print_node(til::print_node* const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;

    for (size_t ix = 0; ix < node->arguments()->size(); ix++) {
        auto child = dynamic_cast<cdk::expression_node*>(node->arguments()->node(ix));

        child->accept(this, lvl);  // determine the value to print
        if (child->is_typed(cdk::TYPE_INT)) {
            _pf.CALL("printi");
            _pf.TRASH(4);  // delete the printed value
        } else if (child->is_typed(cdk::TYPE_STRING)) {
            _pf.CALL("prints");
            _pf.TRASH(4);  // delete the printed value's address
        } else if (child->is_typed(cdk::TYPE_DOUBLE)) {
            _pf.CALL("printd");
            _pf.TRASH(8);  // delete the printed value
        } else {
            std::cerr << "ERROR: CANNOT HAPPEN!" << std::endl;
            exit(1);
        }
        if (node->newline()) {
            _pf.CALL("println");  // print a newline
        }
    }
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_read_node(til::read_node* const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;

    if (node->is_typed(cdk::TYPE_DOUBLE)) {
        _pf.CALL("readd");
        _pf.LDFVAL64();
    } else {
        _pf.CALL("readi");
        _pf.LDFVAL32();
    }
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_while_node(til::while_node* const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;
    int lbl1, lbl2;
    _pf.LABEL(mklbl(lbl1 = ++_lbl));
    node->condition()->accept(this, lvl);
    _pf.JZ(mklbl(lbl2 = ++_lbl));
    node->block()->accept(this, lvl + 2);
    _pf.JMP(mklbl(lbl1));
    _pf.LABEL(mklbl(lbl2));
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_if_node(til::if_node* const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;
    int lbl1;
    node->condition()->accept(this, lvl);
    _pf.JZ(mklbl(lbl1 = ++_lbl));
    node->block()->accept(this, lvl + 2);
    _pf.LABEL(mklbl(lbl1));
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_if_else_node(til::if_else_node* const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;
    int lbl1, lbl2;
    node->condition()->accept(this, lvl);
    _pf.JZ(mklbl(lbl1 = ++_lbl));
    node->thenblock()->accept(this, lvl + 2);
    _pf.JMP(mklbl(lbl2 = ++_lbl));
    _pf.LABEL(mklbl(lbl1));
    node->elseblock()->accept(this, lvl + 2);
    _pf.LABEL(mklbl(lbl1 = lbl2));
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_nullptr_node(til::nullptr_node* const node, int lvl) {
    _pf.INT(0);
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_sizeof_node(til::sizeof_node* const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;
    _pf.INT(node->argument()->type()->size());
}
//---------------------------------------------------------------------------

void til::postfix_writer::do_block_node(til::block_node* const node, int lvl) {
    _symtab.push();  // for block-local variables
    node->declarations()->accept(this, lvl + 2);

    _visitedFinalInstruction = false;
    for (size_t i = 0; i < node->instructions()->size(); i++) {
        auto child = node->instructions()->node(i);

        if (_visitedFinalInstruction) {
            throw_error_for_node(child, "unreachable code; further instructions found after a final instruction");
        }

        child->accept(this, lvl + 2);
    }
    _visitedFinalInstruction = false;

    _symtab.pop();
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_next_node(til::next_node* const node, int lvl) {
    auto level = static_cast<size_t>(node->level());

    if (level == 0) {
        throw_error_for_node(node, "invalid loop control instruction level");
    } else if (_currentFunctionLoopLabels->size() < level) {
        throw_error_for_node(node, "loop control instruction not within sufficient loops (expected at most " +
                                       std::to_string(_currentFunctionLoopLabels->size()) + ")");
    }

    auto index = _currentFunctionLoopLabels->size() - level;
    auto label = std::get<0>(_currentFunctionLoopLabels->at(index));
    _pf.JMP(label);

    _visitedFinalInstruction = true;
}

void til::postfix_writer::do_stop_node(til::stop_node* const node, int lvl) {
    auto level = static_cast<size_t>(node->level());

    if (level == 0) {
        throw_error_for_node(node, "invalid loop control instruction level");
    } else if (_currentFunctionLoopLabels->size() < level) {
        throw_error_for_node(node, "loop control instruction not within sufficient loops (expected at most " +
                                       std::to_string(_currentFunctionLoopLabels->size()) + ")");
    }

    auto index = _currentFunctionLoopLabels->size() - level;
    auto label = std::get<1>(_currentFunctionLoopLabels->at(index));
    _pf.JMP(label);

    _visitedFinalInstruction = true;
}

void til::postfix_writer::do_return_node(til::return_node* const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;

    // symbol is validated in type checker, we are sure it exists
    auto symbol = _symtab.find("@", 1);
    auto rettype = cdk::functional_type::cast(symbol->type())->output(0);
    auto rettype_name = rettype->name();

    if (rettype_name != cdk::TYPE_VOID) {
        node->returnval()->accept(this, lvl + 2);

        if (rettype_name == cdk::TYPE_DOUBLE) {
            _pf.STFVAL64();
        } else {
            _pf.STFVAL32();
        }
    }

    _pf.JMP(_currentFunctionRetLabel);

    _visitedFinalInstruction = true;
}

void til::postfix_writer::do_declaration_node(til::declaration_node* const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;

    auto symbol = new_symbol();
    reset_new_symbol();

    int offset = 0;
    int typesize = node->type()->size();  // in bytes

    if (_inFunctionArgs) {
        offset = _offset;
        _offset += typesize;
    } else if (inFunction()) {
        _offset -= typesize;
        offset = _offset;
    } else {
        // global variable
        offset = 0;
    }
    symbol->offset(offset);

    // Function local variables
    if (inFunction()) {
        // Nothing to do for function args or local variables without initializer
        if (_inFunctionArgs || node->initializer() == nullptr) {
            return;
        }

        node->initializer()->accept(this, lvl);
        if (node->is_typed(cdk::TYPE_DOUBLE)) {
            _pf.LOCAL(symbol->offset());
            _pf.STFVAL64();
        } else {
            _pf.LOCAL(symbol->offset());
            _pf.STFVAL32();
        }

        return;
    }

    // Global variable
    if (node->initializer() == nullptr) {
        _pf.BSS();
        _pf.ALIGN();

        if (symbol->qualifier() == tPUBLIC) {
            _pf.GLOBAL(symbol->name(), _pf.OBJ());
        }

        _pf.LABEL(symbol->name());
        _pf.SALLOC(typesize);
        return;
    }

    // Check if initializer is a valid literal
    if (!isInstanceOf<cdk::integer_node, cdk::double_node, cdk::string_node, til::nullptr_node, til::function_node>(node->initializer())) {
        THROW_ERROR("non-literal initializer for global variable '" + symbol->name() + "'");
    }

    _pf.DATA();
    _pf.ALIGN();

    if (symbol->qualifier() == tPUBLIC) {
        _pf.GLOBAL(symbol->name(), _pf.OBJ());
    }

    _pf.LABEL(symbol->name());

    // Handle integer to double promotion
    if (node->is_typed(cdk::TYPE_DOUBLE) && node->initializer()->is_typed(cdk::TYPE_INT)) {
        auto int_node = dynamic_cast<cdk::integer_node*>(node->initializer());
        _pf.SDOUBLE(int_node->value());
    }
    // Handle integer
    else if (node->is_typed(cdk::TYPE_INT)) {
        auto int_node = dynamic_cast<cdk::integer_node*>(node->initializer());
        _pf.SINT(int_node->value());
    }
    // Handle double
    else if (node->is_typed(cdk::TYPE_DOUBLE)) {
        auto double_node = dynamic_cast<cdk::double_node*>(node->initializer());
        _pf.SDOUBLE(double_node->value());
    } else {
        node->initializer()->accept(this, lvl);
    }
}

void til::postfix_writer::do_function_node(til::function_node* const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;

    std::string functionLabel;
    functionLabel = mklbl(++_lbl);
    _functionLabels.push(functionLabel);

    // Start a new function definition
    _pf.TEXT(_functionLabels.top());
    _pf.ALIGN();

    // Assuming the function name is defined elsewhere
    _pf.GLOBAL(functionLabel, _pf.FUNC());
    _pf.LABEL(_functionLabels.top());

    auto oldOffset = _offset;
    _offset = 8;  // function arguments start at offset 8
    _symtab.push();

    _inFunctionArgs = true;
    node->args()->accept(this, lvl);
    _inFunctionArgs = false;

    // Compute the frame size manually
    int frame_size = 0;
    for (size_t i = 0; i < node->args()->size(); i++) {
        auto arg = dynamic_cast<cdk::typed_node*>(node->args()->node(i));
        frame_size += arg->type()->size();
    }

    // Set the frame size
    _pf.ENTER(frame_size);

    // Generate code for the function body
    node->block()->accept(this, lvl + 2);

    auto oldFunctionRetLabel = _currentFunctionRetLabel;
    _currentFunctionRetLabel = mklbl(++_lbl);

    auto oldFunctionLoopLabels = _currentFunctionLoopLabels;
    _currentFunctionLoopLabels = new std::vector<std::pair<std::string, std::string>>();

    _offset = 0;  // local variables start at offset 0

    node->block()->accept(this, lvl);

    _pf.ALIGN();
    _pf.LABEL(_currentFunctionRetLabel);
    _pf.LEAVE();
    _pf.RET();

    delete _currentFunctionLoopLabels;
    _currentFunctionLoopLabels = oldFunctionLoopLabels;
    _currentFunctionRetLabel = oldFunctionRetLabel;
    _offset = oldOffset;
    _symtab.pop();
    _functionLabels.pop();

    /*
    if (inFunction()) {
        _pf.TEXT(_functionLabels.top());
        _pf.ADDR(functionLabel);
    } else {
        _pf.DATA();
        _pf.SADDR(functionLabel);
    }
    */

    _pf.TEXT(_functionLabels.top());
    _pf.ADDR(functionLabel);
}

void til::postfix_writer::do_function_call_node(til::function_call_node* const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;

    std::shared_ptr<cdk::functional_type> func_type;
    if (node->func() == nullptr) {  // recursive call; "@"
        auto symbol = _symtab.find("@", 1);
        func_type = cdk::functional_type::cast(symbol->type());
    } else {
        func_type = cdk::functional_type::cast(node->func()->type());
    }

    int args_size = 0;
    // arguments must be visited in reverse order since the first argument has to be
    // on top of the stack
    for (size_t i = node->arguments()->size(); i > 0; i--) {
        auto arg = dynamic_cast<cdk::expression_node*>(node->arguments()->node(i - 1));

        args_size += arg->type()->size();
        arg->accept(this, lvl + 2);
    }

    if (node->func() == nullptr) {  // recursive call; "@"
        _pf.ADDR(_functionLabels.top());
    } else {
        node->func()->accept(this, lvl);
    }

    _pf.CALL(_functionLabels.top());

    if (args_size > 0) {
        _pf.TRASH(args_size);
    }

    if (node->is_typed(cdk::TYPE_DOUBLE)) {
        _pf.LDFVAL64();
    } else if (!node->is_typed(cdk::TYPE_VOID)) {
        _pf.LDFVAL32();
    }
}
