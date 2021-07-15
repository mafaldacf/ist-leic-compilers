#ifndef __FIR_AST_WRITE_NODE_H__
#define __FIR_AST_WRITE_NODE_H__

#include <cdk/ast/sequence_node.h>

namespace fir {

  /**
   * Class for describing write nodes.
   */
  class write_node: public cdk::basic_node {
    cdk::sequence_node *_arguments;
    bool _newline;

  public:
    inline write_node(int lineno, cdk::sequence_node *arguments, bool newline) :
        cdk::basic_node(lineno), _arguments(arguments), _newline(newline) {
    }

  public:
    inline cdk::sequence_node *arguments() {
      return _arguments;
    }
    bool newline(){
      return _newline;
    }
    inline cdk::expression_node *argument(size_t i) {
      return dynamic_cast<cdk::expression_node*>(_arguments->node(i));
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_write_node(this, level);
    }

  };

} // fir

#endif
