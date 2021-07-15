#ifndef __FIR_AST_VARIABLE_DECLARATION_NODE_H__
#define __FIR_AST_VARIABLE_DECLARATION_NODE_H__

#include <cdk/ast/typed_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/types/basic_type.h>

namespace fir {

  /**
   * Class for describing variable declaration nodes.
   */
  class variable_declaration_node: public cdk::typed_node {
    int _qualifier;
    std::string _identifier;
    cdk::expression_node *_initializer;

  public:
    inline variable_declaration_node(int lineno, std::shared_ptr<cdk::basic_type> varType,
    int qualifier, const std::string &identifier, cdk::expression_node *initializer):
    cdk::typed_node(lineno), _qualifier(qualifier), _identifier(identifier), _initializer(initializer) {
      type(varType);
      }

  public:
    inline int qualifier() {
      return _qualifier;
    }
    inline std::string identifier() {
      return _identifier;
    }
    inline cdk::expression_node *initializer() {
      return _initializer;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_variable_declaration_node(this, level);
    }

  };

} // fir

#endif
