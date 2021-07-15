#ifndef __FIR_AST_INDEXATION_NODE_H__
#define __FIR_AST_INDEXATION_NODE_H__

#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>

namespace fir {

  /**
   * Class for describing ptr indexation nodes.
   */
  class indexation_node: public cdk::lvalue_node {
    cdk::expression_node *_base;
    cdk::expression_node *_index;

  public:
    inline indexation_node(int lineno, cdk::expression_node *base, cdk::expression_node *index) :
        cdk::lvalue_node(lineno), _base(base), _index(index) {
    }

  public:
    inline cdk::expression_node *base() {
      return _base;
    }
    inline cdk::expression_node *index() {
      return _index;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_indexation_node(this, level);
    }

  };

} // fir

#endif
