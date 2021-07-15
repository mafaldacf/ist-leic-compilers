#ifndef __FIR_AST_LEAVE_NODE_H__
#define __FIR_AST_LEAVE_NODE_H__

#include <cdk/ast/integer_node.h>

namespace fir {

  /**
   * Class for describing leave nodes.
   */
  class leave_node: public cdk::basic_node {
    int _value;

  public:
    inline leave_node(int lineno, int value = 1) :
        cdk::basic_node(lineno), _value(value) {
    }

  public:
    inline int value() const{
      return _value;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_leave_node(this, level);
    }

  };

} // fir

#endif
