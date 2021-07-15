#ifndef __FIR_AST_FUNCTION_DECLARATION_NODE_H__
#define __FIR_AST_FUNCTION_DECLARATION_NODE_H__

#include <cdk/ast/typed_node.h>
#include <cdk/types/basic_type.h>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>

namespace fir {

  /**
   * Class for describing function declaration nodes.
   */
  class function_declaration_node: public cdk::typed_node {
    int _qualifier;
    std::string _identifier;
    cdk::sequence_node *_arguments;

  public:
    inline function_declaration_node(int lineno, std::shared_ptr<cdk::basic_type> varType, int qualifier, 
    const std::string &identifier, cdk::sequence_node *arguments) :
      cdk::typed_node(lineno), _qualifier(qualifier), _identifier(identifier), _arguments(arguments) {
        type(varType);
    }

  public:
    inline int qualifier() {
      return _qualifier;
    }
    inline std::string identifier() {
      return _identifier;
    }
    inline cdk::sequence_node *arguments() {
      return _arguments;
    }
    inline cdk::typed_node* argument(size_t i) {
      return dynamic_cast<cdk::typed_node*>(_arguments->node(i));
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_declaration_node(this, level);
    }

  };

} // fir

#endif
