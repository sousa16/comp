#ifndef __SIMPLE_AST_FUNCTION_NODE_H__
#define __SIMPLE_AST_FUNCTION_NODE_H__

#include <cdk/ast/expression_node.h>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/typed_node.h>
#include <cdk/types/basic_type.h>
#include <cdk/types/functional_type.h>
#include <cdk/types/primitive_type.h>
#include <cdk/types/typename_type.h>

#include <memory>
#include <vector>

#include "block_node.h"

namespace til {

/**
 * Class for describing function nodes.
 */
class function_node : public cdk::expression_node {
    cdk::sequence_node *_args;
    til::block_node *_block;

   public:
    function_node(int lineno,
                  cdk::sequence_node *args,
                  std::shared_ptr<cdk::basic_type> return_type,
                  til::block_node *block) : cdk::expression_node(lineno), _args(args), _block(block) {
        std::vector<std::shared_ptr<cdk::basic_type>> arg_types;
        for (size_t i = 0; i < args->size(); i++) {
            arg_types.push_back(dynamic_cast<cdk::typed_node *>(args->node(i))->type());
        }

        this->type(cdk::functional_type::create(arg_types, return_type));
    }

    function_node(int lineno, til::block_node *block) : cdk::expression_node(lineno), _args(new cdk::sequence_node(lineno)), _block(block) {
        this->type(cdk::functional_type::create(cdk::primitive_type::create(4, cdk::TYPE_INT)));
    }

    inline cdk::sequence_node *args() { return _args; }

    inline cdk::basic_node *block() { return _block; }

    void accept(basic_ast_visitor *sp, int level) { sp->do_function_node(this, level); }
};

}  // namespace til

#endif
