#include <string>
#include "targets/type_checker.h"
#include "ast/all.h"  // automatically generated
#include <cdk/types/primitive_type.h>

#define ASSERT_UNSPEC { if (node->type() != nullptr && !node->is_typed(cdk::TYPE_UNSPEC)) return; }

std::string fir::type_checker::process_function_id(std::string id) {
  if (id == "fir") 
    return "_main";
  else if (id == "_main") 
    return "._main";
  else 
    return id;
}

bool fir::type_checker::is_public(int qualifier) {
  return qualifier == '*';
}

bool fir::type_checker::is_private(int qualifier) {
  return qualifier == 'p';
}

bool fir::type_checker::is_extern(int qualifier) {
  return qualifier == '?';
}

bool fir::type_checker::validate_pointers_level(std::shared_ptr<cdk::basic_type> left_type, std::shared_ptr<cdk::basic_type> right_type) {
  while(1) {
    left_type = cdk::reference_type::cast(left_type)->referenced();
    right_type = cdk::reference_type::cast(right_type)->referenced();
    if (left_type->name() != cdk::TYPE_POINTER || right_type->name() != cdk::TYPE_POINTER) 
      break;
  }
  if (left_type->name() != right_type->name()) {
    return false;
  }
  return true;
}

bool fir::type_checker::is_stack_alloc_node(cdk::expression_node *const node) {
  return dynamic_cast<fir::stack_alloc_node*>(node) != nullptr;
}

bool fir::type_checker::is_read_node(cdk::expression_node *const node) {
  return dynamic_cast<fir::read_node*>(node) != nullptr;
}

bool fir::type_checker::is_null_node(cdk::expression_node *const node) {
  return dynamic_cast<fir::null_node*>(node) != nullptr;
}

bool fir::type_checker::is_expression_node(cdk::expression_node *const node) {
  return node != nullptr;
}

auto fir::type_checker::get_read_node(cdk::expression_node * const node) {
  return dynamic_cast<fir::read_node*>(node);
}

auto fir::type_checker::get_expression_node(cdk::sequence_node * const node, size_t i) {
  return dynamic_cast<cdk::expression_node*>(node);
}

void fir::type_checker::validate_unspec_type(cdk::expression_node *const node) {
  if (!is_read_node(node))
    throw std::string("invalid unspecified type");
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

//---------------------------------------------------------------------------

void fir::type_checker::do_data_node(cdk::data_node *const node, int lvl) {
  // empty
}

//---------------------------------------------------------------------------
 
void fir::type_checker::do_nil_node(cdk::nil_node *const node, int lvl) {
  // empty
}

void fir::type_checker::do_sequence_node(cdk::sequence_node *const node, int lvl) {
  for(size_t i = 0; i < node->size(); i++) {
    node->node(i)->accept(this, lvl);
    auto expression_node = get_expression_node(node, i);
    if (is_expression_node(expression_node)) {
      auto read_node = get_read_node(expression_node);
      if (is_read_node(expression_node) && read_node->is_typed(cdk::TYPE_UNSPEC))
        read_node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    }
  }
}

//---------------------------------------------------------------------------

void fir::type_checker::do_double_node(cdk::double_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
}

void fir::type_checker::do_not_node(cdk::not_node *const node, int lvl) {
  processUnaryExpression(node, lvl);
}

//---------------------------------------------------------------------------

void fir::type_checker::do_integer_node(cdk::integer_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void fir::type_checker::do_string_node(cdk::string_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
}

void fir::type_checker::do_null_node(fir::null_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::reference_type::create(4, nullptr));
}

//---------------------------------------------------------------------------

// Auxiliar function for validation of unspec types in binary expressions with int only types
bool fir::type_checker::validateIntOnlyExpressionUnspecTypes(cdk::binary_operation_node *const node) {
  if (node->left()->is_typed(cdk::TYPE_UNSPEC) && (node->right()->is_typed(cdk::TYPE_INT))) {
    validate_unspec_type(node->left());
    return true;
  }
  else if (node->right()->is_typed(cdk::TYPE_UNSPEC) && (node->left()->is_typed(cdk::TYPE_INT))) {
    validate_unspec_type(node->right());
    return true;
  }
  else if (node->left()->is_typed(cdk::TYPE_UNSPEC) && node->right()->is_typed(cdk::TYPE_UNSPEC)) {
    validate_unspec_type(node->left());
    validate_unspec_type(node->right());
    return true;
  }
  return false;
}

// Auxiliar function for validation of unspec types in binary expressions
bool fir::type_checker::validateExpressionUnspecTypes(cdk::binary_operation_node *const node) {
  if (node->left()->is_typed(cdk::TYPE_UNSPEC) 
    && (node->right()->is_typed(cdk::TYPE_INT) || node->right()->is_typed(cdk::TYPE_DOUBLE))) {
    validate_unspec_type(node->left());
    return true;
  }
  else if (node->right()->is_typed(cdk::TYPE_UNSPEC) 
    && (node->left()->is_typed(cdk::TYPE_INT) || node->left()->is_typed(cdk::TYPE_DOUBLE))) {
    validate_unspec_type(node->right());
    return true;
  }
  else if (node->left()->is_typed(cdk::TYPE_UNSPEC) && node->right()->is_typed(cdk::TYPE_UNSPEC)) {
    validate_unspec_type(node->left());
    validate_unspec_type(node->right());
    return true;
  }
  return false;
}

// Auxiliar function for Pointer Binary Expressions and Binary Expressions
void fir::type_checker::processNonPointerBinaryExpression(cdk::binary_operation_node *const node) {
  if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_INT))
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE))
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT))
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_DOUBLE))
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  else if (validateExpressionUnspecTypes(node))
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  else 
    throw std::string("wrong types in binary expression");
}

// Auxiliar function for Binary Scalar Logical Expressions and Binary General Logical Expressions functions
void fir::type_checker::processNonPointerBinaryLogicalExpression(cdk::binary_operation_node *const node) {
  if ((node->left()->is_typed(cdk::TYPE_INT) || node->left()->is_typed(cdk::TYPE_DOUBLE)) &&
    (node->right()->is_typed(cdk::TYPE_INT) || node->right()->is_typed(cdk::TYPE_DOUBLE)))
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  else if (validateExpressionUnspecTypes(node))
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  else 
    throw std::string("wrong types in binary logical expression");
}

//---------------------------------------------------------------------------

void fir::type_checker::processPointerBinaryExpression(cdk::binary_operation_node *const node, int lvl, bool is_sub) {
  ASSERT_UNSPEC;

  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);


  if (node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT))
    node->type(node->left()->type());
  else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_POINTER))
    node->type(node->right()->type());
  else if (is_sub && node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_POINTER)) {
    if (!validate_pointers_level(node->left()->type(), node->right()->type()))
      throw std::string("wrong types in binary expression: pointers level don't match");
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  }
  else
    processNonPointerBinaryExpression(node);
}

void fir::type_checker::processIntOnlyBinaryExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;

  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_INT))
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  else if (validateIntOnlyExpressionUnspecTypes(node))
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  else 
    throw std::string("wrong types in binary expression");
}

void fir::type_checker::processBinaryScalarLogicalExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;

  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  processNonPointerBinaryLogicalExpression(node);
}

void fir::type_checker::processBinaryGeneralLogicalExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;

  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  if (node->left()->is_typed(cdk::TYPE_POINTER) || node->right()->is_typed(cdk::TYPE_POINTER)) {
    if (!validate_pointers_level(node->left()->type(), node->right()->type()))
      throw std::string("wrong types in binary general logical expression: pointers level don't match");
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  }
  else 
    processNonPointerBinaryLogicalExpression(node);
}

void fir::type_checker::do_add_node(cdk::add_node *const node, int lvl) {
  processPointerBinaryExpression(node, lvl, false);
}
void fir::type_checker::do_sub_node(cdk::sub_node *const node, int lvl) {
  processPointerBinaryExpression(node, lvl, true);
}
void fir::type_checker::do_mul_node(cdk::mul_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  processNonPointerBinaryExpression(node);
}
void fir::type_checker::do_div_node(cdk::div_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  processNonPointerBinaryExpression(node);
}
void fir::type_checker::do_mod_node(cdk::mod_node *const node, int lvl) {
  processIntOnlyBinaryExpression(node, lvl);
}
void fir::type_checker::do_lt_node(cdk::lt_node *const node, int lvl) {
  processBinaryScalarLogicalExpression(node, lvl);
}
void fir::type_checker::do_le_node(cdk::le_node *const node, int lvl) {
  processBinaryScalarLogicalExpression(node, lvl);
}
void fir::type_checker::do_ge_node(cdk::ge_node *const node, int lvl) {
  processBinaryScalarLogicalExpression(node, lvl);
}
void fir::type_checker::do_gt_node(cdk::gt_node *const node, int lvl) {
  processBinaryScalarLogicalExpression(node, lvl);
}
void fir::type_checker::do_ne_node(cdk::ne_node *const node, int lvl) {
  processBinaryGeneralLogicalExpression(node, lvl);
}
void fir::type_checker::do_eq_node(cdk::eq_node *const node, int lvl) {
  processBinaryGeneralLogicalExpression(node, lvl);
}


//---------------------------------------------------------------------------

void fir::type_checker::processBooleanExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_INT))
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  else if (validateIntOnlyExpressionUnspecTypes(node))
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  else
    throw std::string("wrong types in binary boolean expression");
}

void fir::type_checker::do_and_node(cdk::and_node *const node, int lvl) {
  processBooleanExpression(node, lvl);
}

void fir::type_checker::do_or_node(cdk::or_node *const node, int lvl) {
  processBooleanExpression(node, lvl);
}

//---------------------------------------------------------------------------

void fir::type_checker::processUnaryExpression(cdk::unary_operation_node *const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
  if (node->argument()->is_typed(cdk::TYPE_INT))
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  else
    throw std::string("wrong type in argument of unary expression");
}

void fir::type_checker::do_neg_node(cdk::neg_node *const node, int lvl) {
  processUnaryExpression(node, lvl);
}


//---------------------------------------------------------------------------

void fir::type_checker::do_variable_node(cdk::variable_node *const node, int lvl) {
  ASSERT_UNSPEC;

  const std::string &id = node->name();
  auto symbol = _symtab.find(id);
  
  if (symbol) {
    if (symbol->is_function()) {
      if (symbol->is_typed(cdk::TYPE_VOID))
        throw std::string("can't define a function a return value for a void function");
      else if (_inFunctionBody && symbol->name() == id)
        node->type(symbol->type());
      else
        throw std::string("can't define a new function return value outside of it");
    }
    else {
      node->type(symbol->type());
    }
  }
  else {
    throw std::string("undeclared variable '" + id + "'");
  }
}

void fir::type_checker::do_rvalue_node(cdk::rvalue_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->lvalue()->accept(this, lvl);
  node->type(node->lvalue()->type());
}

void fir::type_checker::do_assignment_node(cdk::assignment_node *const node, int lvl) {
  ASSERT_UNSPEC;

  node->lvalue()->accept(this, lvl + 4);
  node->rvalue()->accept(this, lvl + 4);

  if (node->lvalue()->is_typed(cdk::TYPE_INT)) {
    if (node->rvalue()->is_typed(cdk::TYPE_INT)) {
      node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    } 
    else if (node->rvalue()->is_typed(cdk::TYPE_UNSPEC) && is_read_node(node->rvalue())) {
        node->rvalue()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
        node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    } 
    else {
      throw std::string("wrong assignment to integer");
    }
  } 

  else if (node->lvalue()->is_typed(cdk::TYPE_DOUBLE)) {
    if (node->rvalue()->is_typed(cdk::TYPE_DOUBLE) || node->rvalue()->is_typed(cdk::TYPE_INT))
      node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    else if (node->rvalue()->is_typed(cdk::TYPE_UNSPEC) && is_read_node(node->rvalue())) {
        node->rvalue()->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
        node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    } 
    else
      throw std::string("wrong assignment to real");
  }

  else if (node->lvalue()->is_typed(cdk::TYPE_STRING)) {
    if (node->rvalue()->is_typed(cdk::TYPE_STRING))
      node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
    else
      throw std::string("wrong assignment to string");
  }

  else if (node->lvalue()->is_typed(cdk::TYPE_POINTER)) {
    if (node->rvalue()->is_typed(cdk::TYPE_POINTER)) {
      if (!is_null_node(node->rvalue()))
        if (!validate_pointers_level(node->lvalue()->type(), node->rvalue()->type()))
          throw std::string("wrong assignment to pointer: invalid pointer level");
      node->type(node->lvalue()->type());
    }
    else if (node->rvalue()->is_typed(cdk::TYPE_UNSPEC) && is_stack_alloc_node(node->rvalue())) {
      node->rvalue()->type(node->lvalue()->type());
      node->type(node->lvalue()->type());
    }
    else
      throw std::string("wrong assignment to pointer");
  }
  else 
    throw std::string("wrong types in assignment");
}

//---------------------------------------------------------------------------

void fir::type_checker::do_evaluation_node(fir::evaluation_node *const node, int lvl) {
  node->argument()->accept(this, lvl + 2);

  if (node->argument()->is_typed(cdk::TYPE_UNSPEC)) {
    if (is_read_node(node->argument()))
      node->argument()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    else
      throw std::string("wrong type in evaluation");
  }
}


void fir::type_checker::do_write_node(fir::write_node *const node, int lvl) {
  node->arguments()->accept(this, lvl + 2);
  for (size_t i = 0; i < node->arguments()->size(); i++) {
    auto argument = node->argument(i);
    if (argument != nullptr && argument->is_typed(cdk::TYPE_UNSPEC) && is_read_node(argument))
      argument->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  }

}

//---------------------------------------------------------------------------

void fir::type_checker::do_read_node(fir::read_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(0, cdk::TYPE_UNSPEC));
}

//---------------------------------------------------------------------------

void fir::type_checker::do_while_node(fir::while_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
  node->instruction()->accept(this, lvl + 4);
}

void fir::type_checker::do_while_finally_node(fir::while_finally_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
  node->doInstruction()->accept(this, lvl + 4);
  node->finallyInstruction()->accept(this, lvl + 4);
}

//---------------------------------------------------------------------------

void fir::type_checker::do_if_node(fir::if_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);

  if (!node->condition()->is_typed(cdk::TYPE_INT))
    throw std::string("expected integer condition");
}

void fir::type_checker::do_if_else_node(fir::if_else_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
  
  if (!node->condition()->is_typed(cdk::TYPE_INT))
    throw std::string("expected integer condition");

  node->thenInstruction()->accept(this, lvl + 4);
  node->elseInstruction()->accept(this, lvl + 4);
}

//---------------------------------------------------------------------------

void fir::type_checker::do_variable_declaration_node(fir::variable_declaration_node *const node, int lvl) {
  bool is_initialized = false;
  if (node->initializer()) {
    is_initialized = true;
    node->initializer()->accept(this, lvl + 2);

    if (_inFunctionBody) {
      if (is_public(node->qualifier()) || is_extern(node->qualifier()))
        throw std::string("variable " + node->identifier() + " cannot be declared as public inside a function");
      else if (is_extern(node->qualifier()))
        throw std::string("variable " + node->identifier() + " cannot be declared as extern inside a function");
    }

    if (node->qualifier() == '?')
      throw std::string("cannot redefine external variable " + node->identifier());

    if (node->is_typed(cdk::TYPE_INT) && !node->initializer()->is_typed(cdk::TYPE_INT)) {
      if (node->initializer()->is_typed(cdk::TYPE_UNSPEC) && is_read_node(node->initializer()))
        node->initializer()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
      else if (!node->initializer()->is_typed(cdk::TYPE_INT))
        throw std::string("wrong type in variable declaration initializer: expected integer");
    }
    else if (node->is_typed(cdk::TYPE_DOUBLE)) {
      if (node->initializer()->is_typed(cdk::TYPE_UNSPEC) && is_read_node(node->initializer()))
        node->initializer()->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
      else if (!node->initializer()->is_typed(cdk::TYPE_DOUBLE) && !node->initializer()->is_typed(cdk::TYPE_INT)) {
        throw std::string("wrong type in variable declaration initializer: expected double or integer");
      }
    }
    else if (node->is_typed(cdk::TYPE_STRING)) {
      if (!node->initializer()->is_typed(cdk::TYPE_STRING))
        throw std::string("wrong type in variable declaration initializer: expected string");
    }
    else if (node->is_typed(cdk::TYPE_POINTER)) { 
      if (node->initializer()->is_typed(cdk::TYPE_POINTER)) {
        if (!is_null_node(node->initializer()) && !validate_pointers_level(node->type(), node->initializer()->type()))
          throw std::string("wrong type in variable declaration initializer: pointers level don't match");
      }
      else if (node->initializer()->is_typed(cdk::TYPE_UNSPEC) && is_stack_alloc_node(node->initializer()))
        node->initializer()->type(node->type());
      else {
        throw std::string("wrong type in variable declaration initializer: expected pointer");
      }
    }
  }

  const std::string &id = node->identifier();
  auto symbol = fir::make_symbol(node->qualifier(), node->type(), id, is_initialized, false);

  if (_symtab.insert(id, symbol))
    _parent->set_new_symbol(symbol);
  else {
    throw std::string("variable '" + id + "' redeclared");
  }
}

void fir::type_checker::do_function_declaration_node(fir::function_declaration_node *const node, int lvl) {

  std::string id = process_function_id(node->identifier());
  auto function = fir::make_symbol(node->qualifier(), node->type(), id, false, true);

  std::vector<std::shared_ptr<cdk::basic_type>> argument_types;
  for (size_t i = 0; i < node->arguments()->size(); i++)
    argument_types.push_back(node->argument(i)->type());
  function->set_argument_types(argument_types);

  std::shared_ptr<fir::symbol> symbol = _symtab.find(id);

  if (symbol) {
    if (symbol->is_variable())
      throw std::string(function->name() + " is already declared as a variable");
    throw std::string(function->name() + " is already declared");
  }
  
  _symtab.insert(function->name(), function);
  _parent->set_new_symbol(function);
}

void fir::type_checker::do_function_definition_node(fir::function_definition_node *const node, int lvl) {

  std::string id = process_function_id(node->identifier());
  auto function = fir::make_symbol(node->qualifier(), node->type(), id, true, true);

  std::vector<std::shared_ptr<cdk::basic_type>> argument_types;
  for (size_t i = 0; i < node->arguments()->size(); i++)
    argument_types.push_back(node->argument(i)->type());
  function->set_argument_types(argument_types);

  std::shared_ptr<fir::symbol> previous = _symtab.find(id);

  if (id == "_main" && previous)
    throw std::string("main function already declared");

  if (is_extern(function->qualifier()))
    throw std::string("cannot define function " + function->name() + " as external");

  if (previous) {
    if (previous->is_variable())
      throw std::string(function->name() + " is already declared as a variable");
    else if (previous->is_defined())
      throw std::string(function->name() + " is already defined");
    else if (is_private(previous->qualifier() && is_public(function->qualifier())))
      throw std::string("qualifiers don't match in function " + function->name() + 
      " definition: expected private qualifier instead of public qualifier");
    else if (is_public(previous->qualifier() && is_private(function->qualifier())))
      throw std::string("qualifiers don't match in function " + function->name() + 
      " definition: expected public qualifier instead of private qualifier");
    else if (!previous->is_typed(function->type()->name()))
      throw std::string("types don't match in function " + function->name() + " definition");
    else if (previous->is_typed(cdk::TYPE_POINTER) && function->is_typed(cdk::TYPE_POINTER)) {
      if (!validate_pointers_level(previous->type(), function->type()))
        throw std::string("pointers level don't match in function " + function->name() + " definition");
    }
    else if (previous->number_of_arguments() != function->number_of_arguments())
      throw std::string("invalid number of arguments in function " + function->name() + " definition");
    else
      for (size_t i = 0; i < previous->number_of_arguments(); i++)
        if (!previous->argument_is_typed(i, function->argument_type(i)->name()))
          throw std::string("wrong argument types in function " + function->name() + " definition");
          
    function->set_defined();
    _symtab.replace(function->name(), function);
  }
  else
    _symtab.insert(function->name(), function);

  _parent->set_new_symbol(function);

  if (node->defReturnValue()) {
    node->defReturnValue()->accept(this, lvl);
    if(function->is_typed(cdk::TYPE_VOID))
      throw std::string("void function " + function->name() + " cannot have a return value");
    if (!function->is_typed(node->defReturnValue()->type()->name())) {
      if (!(function->is_typed(cdk::TYPE_DOUBLE) && node->defReturnValue()->is_typed(cdk::TYPE_INT)))
        throw std::string("wrong return value type in function " + function->name() + " definition");
    }
    else if (function->is_typed(cdk::TYPE_POINTER)) {
      if (!validate_pointers_level(function->type(), node->defReturnValue()->type()))
        throw std::string("pointers level of return value don't match in function " + function->name() + " definition");
    }
  }
}

//---------------------------------------------------------------------------

void fir::type_checker::do_block_node(fir::block_node *const node, int lvl) {
  if(node->declarations())
    node->declarations()->accept(this, lvl + 4);
  if(node->instructions())
    node->instructions()->accept(this, lvl + 4);
}

void fir::type_checker::do_prologue_node(fir::prologue_node *const node, int lvl) {
  node->block()->accept(this, lvl + 2);
}

//---------------------------------------------------------------------------

void fir::type_checker::do_restart_node(fir::restart_node *const node, int lvl) {
  if (node->value() < 1) 
    throw std::string("invalid cycle integer value in restart instruction");
}

void fir::type_checker::do_leave_node(fir::leave_node *const node, int lvl) {
  if (node->value() < 1) 
    throw std::string("invalid cycle integer value in leave instruction");
}

void fir::type_checker::do_return_node(fir::return_node *const node, int lvl) {
  // empty
}

//---------------------------------------------------------------------------

void fir::type_checker::do_address_of_node(fir::address_of_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->lvalue()->accept(this, lvl + 2);
  if (node->lvalue()->is_typed(cdk::TYPE_UNSPEC))
    node->type(cdk::reference_type::create(4, node->lvalue()->type()));
  else
    throw std::string("wrong type in 'adress of' expression");
}

void fir::type_checker::do_function_call_node(fir::function_call_node *const node, int lvl) {
  ASSERT_UNSPEC;

  std::string id = process_function_id(node->identifier());
  auto symbol = _symtab.find(id);

  if (symbol == nullptr || !symbol->is_function())
    throw std::string("function '" + id + "' undeclared");

  node->type(symbol->type());
  node->arguments()->accept(this, lvl + 4);
  
  if (symbol->number_of_arguments() != node->arguments()->size())
    throw std::string("invalid number of arguments in function " + node->identifier() + " call");

  for(size_t i = 0; i < node->arguments()->size(); i++)
    if (!symbol->argument_is_typed(i, node->argument(i)->type()->name()))
      if (!(node->argument(i)->is_typed(cdk::TYPE_INT) && symbol->argument_is_typed(i, cdk::TYPE_DOUBLE)))
        throw std::string("wrong type of argument in function '" + id + "' call");
}

//---------------------------------------------------------------------------

void fir::type_checker::do_identity_node(fir::identity_node *const node, int lvl) {
  ASSERT_UNSPEC;
  processUnaryExpression(node, lvl);
}

void fir::type_checker::do_sizeof_node(fir::sizeof_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void fir::type_checker::do_stack_alloc_node(fir::stack_alloc_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  if (node->argument()->is_typed(cdk::TYPE_UNSPEC) && is_read_node(node->argument()))
    node->argument()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  else if (!node->argument()->is_typed(cdk::TYPE_INT))
    throw std::string("wrong type in stack alloc: expected integer");

  node->type(cdk::primitive_type::create(0, cdk::TYPE_UNSPEC));
}

//---------------------------------------------------------------------------

void fir::type_checker::do_indexation_node(fir::indexation_node *const node, int lvl) { //TODO verify if stack aloc?
  ASSERT_UNSPEC;
  node->base()->accept(this, lvl + 2);
  if (!node->base()->is_typed(cdk::TYPE_POINTER))
    throw std::string("wrong type in indexation base: expected pointer");

  node->index()->accept(this, lvl + 2);
  if (node->index()->is_typed(cdk::TYPE_UNSPEC) && is_read_node(node->index()))
    node->index()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  else if (!node->index()->is_typed(cdk::TYPE_INT))
    throw std::string("wrong type in indexation left value: expected pointer");

  node->type(cdk::reference_type::cast(node->base()->type())->referenced());
}
