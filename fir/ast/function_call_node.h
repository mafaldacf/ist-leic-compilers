#ifndef __FIR_AST_FUNCTION_CALL_NODE_H__
#define __FIR_AST_FUNCTION_CALL_NODE_H__

#include <cdk/ast/sequence_node.h>

namespace fir {

  /**
   * Class for describing function call nodes.
   */
  class function_call_node: public cdk::expression_node {
    std::string _identifier;
    cdk::sequence_node *_arguments;

  public:
    /*
     * Constructor for a function call node without any arguments.
     * A new empty sequence is generated for the arguments.
     */
    inline function_call_node(int lineno, const std::string &identifier) :
      cdk::expression_node(lineno), _identifier(identifier), _arguments(new cdk::sequence_node(lineno)) {
    }

    /*
     * Constructor for a function call node with arguments.
     */
    inline function_call_node(int lineno, const std::string &identifier, cdk::sequence_node *arguments) :
      cdk::expression_node(lineno), _identifier(identifier), _arguments(arguments) {
    }

  public:
    inline std::string identifier() {
      return _identifier;
    }
    inline cdk::sequence_node *arguments() {
      return _arguments;
    }
    inline cdk::expression_node *argument(size_t i) {
      return dynamic_cast<cdk::expression_node*>(_arguments->node(i));
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_call_node(this, level);
    }

  };

} // fir

#endif
