#ifndef __FIR_AST_RESTART_NODE_H__
#define __FIR_AST_RESTART_NODE_H__

#include <cdk/ast/integer_node.h>

namespace fir {

  /**
   * Class for describing restart nodes.
   */
  class restart_node: public cdk::basic_node {
    int _value;

  public:
    inline restart_node(int lineno, int value = 1) :
        cdk::basic_node(lineno), _value(value) {
    }

  public:
    inline int value() const{
      return _value;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_restart_node(this, level);
    }

  };

} // fir

#endif
