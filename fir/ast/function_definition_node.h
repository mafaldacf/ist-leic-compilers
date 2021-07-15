#ifndef __FIR_AST_FUNCTION_DEFINITION_NODE_H__
#define __FIR_AST_FUNCTION_DEFINITION_NODE_H__

#include <cdk/ast/typed_node.h>
#include <cdk/types/basic_type.h>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>

namespace fir {

  /**
   * Class for describing function definition nodes.
   */
  class function_definition_node: public cdk::typed_node {
    int _qualifier;
    std::string _identifier;
    cdk::sequence_node *_arguments;
    cdk::expression_node *_defReturnValue;
    fir::prologue_node *_prologue;
    fir::block_node *_block;
    fir::block_node *_epilogue;

  public:
    inline function_definition_node(int lineno, std::shared_ptr<cdk::basic_type> varType, int qualifier, const std::string &identifier, 
    cdk::sequence_node *arguments, cdk::expression_node *defReturnValue, fir::prologue_node *prologue, fir::block_node *block, fir::block_node *epilogue) :
      cdk::typed_node(lineno), _qualifier(qualifier), _identifier(identifier), _arguments(arguments), _defReturnValue(defReturnValue), _prologue(prologue),
      _block(block), _epilogue(epilogue) {
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
    inline cdk::expression_node *defReturnValue() {
      return _defReturnValue;
    }
    inline fir::prologue_node *prologue() {
      return _prologue;
    }
    inline fir::block_node *block() {
      return _block;
    }
    inline fir::block_node *epilogue() {
      return _epilogue;
    }
    inline cdk::typed_node* argument(size_t i) {
      return dynamic_cast<cdk::typed_node*>(_arguments->node(i));
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_definition_node(this, level);
    }

  };

} // fir

#endif
