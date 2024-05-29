#ifndef __SIMPLE_TARGETS_POSTFIX_WRITER_H__
#define __SIMPLE_TARGETS_POSTFIX_WRITER_H__

#include <cdk/emitters/basic_postfix_emitter.h>
#include <cdk/types/types.h>

#include <optional>
#include <set>
#include <sstream>
#include <stack>

#include "targets/basic_ast_visitor.h"

namespace til {

//!
//! Traverse syntax tree and generate the corresponding assembly code.
//!
class postfix_writer : public basic_ast_visitor {
    cdk::symbol_table<til::symbol> &_symtab;
    cdk::basic_postfix_emitter &_pf;
    int _lbl;

    bool _forceOutsideFunction = false;       // whether to force future declarations to be global
    std::stack<std::string> _functionLabels;  // (history of) label of current visiting function
    bool _inFunctionArgs = false;
    std::optional<std::string> _externalFunctionName;
    std::set<std::string> _externalFunctionsToDeclare;
    std::string _currentFunctionRetLabel;  // where to jump when a return occurs
    int _offset;                           // current offset for local variables
    std::vector<std::pair<std::string, std::string>> *_currentFunctionLoopLabels;

    bool _visitedFinalInstruction = false;

   public:
    postfix_writer(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<til::symbol> &symtab,
                   cdk::basic_postfix_emitter &pf) : basic_ast_visitor(compiler), _symtab(symtab), _pf(pf), _lbl(0) {
    }

   public:
    ~postfix_writer() {
        os().flush();
    }

   private:
    /** Method used to generate sequential labels. */
    inline std::string mklbl(int lbl) {
        std::ostringstream oss;
        if (lbl < 0)
            oss << ".L" << -lbl;
        else
            oss << "_L" << lbl;
        return oss.str();
    }

    inline bool inFunction() {
        return !_forceOutsideFunction && !_functionLabels.empty();
    }

    template <class T>
    inline bool isInstanceOf(cdk::basic_node *const node) {
        return dynamic_cast<T *>(node) != nullptr;
    }
    template <class T, class... Rest, typename std::enable_if<sizeof...(Rest) != 0, int>::type = 0>
    inline bool isInstanceOf(cdk::basic_node *const node) {
        return dynamic_cast<T *>(node) != nullptr || isInstanceOf<Rest...>(node);
    }

   public:
    // do not edit these lines
#define __IN_VISITOR_HEADER__
#include ".auto/visitor_decls.h"  // automatically generated
#undef __IN_VISITOR_HEADER__
    // do not edit these lines: end
};

inline void throw_error_for_node(const auto &subject, const std::string &msg) {
    std::cerr << subject->lineno() << ": " << msg << std::endl;
    return;
}

#define THROW_ERROR(msg) throw_error_for_node(node, msg)

}  // namespace til

#endif
