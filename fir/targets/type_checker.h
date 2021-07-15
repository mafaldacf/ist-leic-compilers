#ifndef __FIR_TARGETS_TYPE_CHECKER_H__
#define __FIR_TARGETS_TYPE_CHECKER_H__

#include "targets/basic_ast_visitor.h"

namespace fir {

  /**
   * Print nodes as XML elements to the output stream.
   */
  class type_checker: public basic_ast_visitor {
    cdk::symbol_table<fir::symbol> &_symtab;

    basic_ast_visitor *_parent;
    bool _inFunctionBody;

  public:
    type_checker(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<fir::symbol> &symtab, basic_ast_visitor *parent, bool inFunctionBody) :
        basic_ast_visitor(compiler), _symtab(symtab), _parent(parent), _inFunctionBody(inFunctionBody) {
    }

  public:
    ~type_checker() {
      os().flush();
    }

  protected:
    std::string process_function_id(std::string id);
    bool is_public(int qualifier);
    bool is_private(int qualifier);
    bool is_extern(int qualifier);
    bool validate_pointers_level(std::shared_ptr<cdk::basic_type> left_type, std::shared_ptr<cdk::basic_type> right_type);
    bool is_stack_alloc_node(cdk::expression_node *const node);
    bool is_read_node(cdk::expression_node *const node);
    bool is_null_node(cdk::expression_node *const node);
    bool is_expression_node(cdk::expression_node *const node);
    auto get_read_node(cdk::expression_node * const node);
    auto get_expression_node(cdk::sequence_node * const node, size_t i);
    void validate_unspec_type(cdk::expression_node *const node);
    bool validateIntOnlyExpressionUnspecTypes(cdk::binary_operation_node *const node);
    bool validateExpressionUnspecTypes(cdk::binary_operation_node *const node);
    void processPointerBinaryExpression(cdk::binary_operation_node *const node, int lvl, bool is_sub);
    void processNonPointerBinaryExpression(cdk::binary_operation_node * const node);
    void processNonPointerBinaryLogicalExpression(cdk::binary_operation_node * const node);
    void processUnaryExpression(cdk::unary_operation_node *const node, int lvl);
    void processIntOnlyBinaryExpression(cdk::binary_operation_node *const node, int lvl);
    void processBinaryScalarLogicalExpression(cdk::binary_operation_node *const node, int lvl);
    void processBinaryGeneralLogicalExpression(cdk::binary_operation_node *const node, int lvl);
    void processBooleanExpression(cdk::binary_operation_node *const node, int lvl);
    template<typename T>
    void process_literal(cdk::literal_node<T> *const node, int lvl) {
    }

  public:
    // do not edit these lines
#define __IN_VISITOR_HEADER__
#include "ast/visitor_decls.h"       // automatically generated
#undef __IN_VISITOR_HEADER__
    // do not edit these lines: end

  };

} // fir

//---------------------------------------------------------------------------
//     HELPER MACRO FOR TYPE CHECKING
//---------------------------------------------------------------------------

#define CHECK_TYPES(compiler, symtab, node, _inFunctionBody) { \
  try { \
    fir::type_checker checker(compiler, symtab, this, _inFunctionBody); \
    (node)->accept(&checker, 0); \
  } \
  catch (const std::string &problem) { \
    std::cerr << (node)->lineno() << ": " << problem << std::endl; \
    return; \
  } \
}

#define ASSERT_SAFE_EXPRESSIONS CHECK_TYPES(_compiler, _symtab, node, _inFunctionBody)

#endif
