#ifndef __TIL_AST_IDENTITY_NODE_H__
#define __TIL_AST_IDENTITY_NODE_H__

#include <cdk/ast/unary_operation_node.h>

namespace til {

/**
 * Class for describing identity nodes.
 */
class identity_node : public cdk::unary_operation_node {
   public:
    /**
     * Constructor for the identity node.
     * @param lineno Source code line number for this node.
     * @param argument Node to be assigned to the identity node.
     */
    identity_node(int lineno, cdk::expression_node *argument) : cdk::unary_operation_node(lineno, argument) {
    }

   public:
    /**
     * Accepts the visitor for this node.
     * @param v Visitor that will visit this node.
     * @param lvl Level of the node.
     */
    void accept(basic_ast_visitor *sp, int lvl) {
        sp->do_identity_node(this, lvl);
    }
};

}  // namespace til

#endif