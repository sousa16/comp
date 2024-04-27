#ifndef __SIMPLE_AST_RETURN_NODE_H__
#define __SIMPLE_AST_RETURN_NODE_H__

#include <cdk/ast/basic_node.h>

namespace til {

/**
 * Class for describing return nodes.
 */
class return_node : public cdk::basic_node {
    cdk::expression_node *_returnval;

   public:
    return_node(int lineno, cdk::expression_node *returnval) : cdk::basic_node(lineno), _returnval(returnval) {
    }

    cdk::expression_node *returnval() { return _returnval; }

    void accept(basic_ast_visitor *sp, int level) { sp->do_return_node(this, level); }
};

}  // til

#endif
