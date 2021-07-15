#ifndef __FIR_AST_IF_ELSE_NODE_H__
#define __FIR_AST_IF_ELSE_NODE_H__

#include <cdk/ast/expression_node.h>

namespace fir {

  /**
   * Class for describing if-then-else nodes.
   */
  class if_else_node: public cdk::basic_node {
    cdk::expression_node *_condition;
    cdk::basic_node *_thenInstruction, *_elseInstruction;

  public:
    inline if_else_node(int lineno, cdk::expression_node *condition, cdk::basic_node *thenInstruction, cdk::basic_node *elseInstruction) :
        cdk::basic_node(lineno), _condition(condition), _thenInstruction(thenInstruction), _elseInstruction(elseInstruction) {
    }

  public:
    inline cdk::expression_node *condition() {
      return _condition;
    }
    inline cdk::basic_node *thenInstruction() {
      return _thenInstruction;
    }
    inline cdk::basic_node *elseInstruction() {
      return _elseInstruction;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_if_else_node(this, level);
    }

  };

} // fir

#endif
