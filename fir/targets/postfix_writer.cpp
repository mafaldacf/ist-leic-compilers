#include <string>
#include <sstream>
#include "targets/type_checker.h"
#include "targets/postfix_writer.h"
#include "ast/all.h"  // all.h is automatically generated

// TODO: rever SINT, INT, SDOUBLE, DOUBLE, etc na cdk atual
// TODO: necessário fazer DCMP depois de I2D

bool fir::postfix_writer::is_main_function(std::string id) {
  return id == "fir" || id == "_main";
}

bool fir::postfix_writer::is_public(int qualifier) {
  return qualifier == '*';
}

bool fir::postfix_writer::is_private(int qualifier) {
  return qualifier == 'p';
}

bool fir::postfix_writer::is_extern(int qualifier) {
  return qualifier == '?';
}

void fir::postfix_writer::process_pointer_reference(std::shared_ptr<cdk::basic_type> type) {
  if (cdk::reference_type::cast(type)->referenced()->name() == cdk::TYPE_DOUBLE)
    _pf.INT(3);
  else
    _pf.INT(2);
}

std::string fir::postfix_writer::process_function_id(std::string id) {
  if (id == "fir") 
    return "_main";
  else if (id == "_main") 
    return "._main";
  else 
    return id;
}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_data_node(cdk::data_node * const node, int lvl) {
  // EMPTY
}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_nil_node(cdk::nil_node * const node, int lvl) {
  // EMPTY
}

void fir::postfix_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++)
    node->node(i)->accept(this, lvl);
}

void fir::postfix_writer::do_evaluation_node(fir::evaluation_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl);
  if (node->argument()->is_typed(cdk::TYPE_DOUBLE))
    _pf.TRASH(8);
  else 
    _pf.TRASH(4);

}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_double_node(cdk::double_node * const node, int lvl) {
  if (_inFunctionBody)
    _pf.DOUBLE(node->value());
  else
    _pf.SDOUBLE(node->value());
}

void fir::postfix_writer::do_integer_node(cdk::integer_node * const node, int lvl) {
  if (_inFunctionBody)
    _pf.INT(node->value());
  else
    _pf.SINT(node->value());
}

void fir::postfix_writer::do_string_node(cdk::string_node * const node, int lvl) {
  int lbl_string;

  _pf.RODATA(); // read only data (const or string)
  _pf.ALIGN(); // align memory
  _pf.LABEL(mklbl(lbl_string = ++_lbl)); // define label
  _pf.SSTRING(node->value()); // allocate space by definiting initial value (string literal)

  if (_inFunctionBody) {
    _pf.TEXT();
    _pf.ALIGN();
    _pf.ADDR(mklbl(lbl_string));
  }
  else {
    _pf.DATA();
    _pf.ALIGN();
    _pf.SADDR(mklbl(lbl_string)); // alocate space by defining initial value (address of literal)
  }
}

void fir::postfix_writer::do_null_node(fir::null_node * const node, int lvl) {
  if (_inFunctionBody)
    _pf.INT(0);
  else
    _pf.SINT(0);
}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_add_node(cdk::add_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT))
    _pf.I2D();
  else if (node->is_typed(cdk::TYPE_POINTER) && node->left()->is_typed(cdk::TYPE_INT)) {
    process_pointer_reference(node->type());
    _pf.SHTL();
  }

  node->right()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT))
    _pf.I2D();
  else if (node->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT)) {
    process_pointer_reference(node->type());
    _pf.SHTL();
  }

  if (node->is_typed(cdk::TYPE_DOUBLE))
    _pf.DADD();
  else 
    _pf.ADD();
}


void fir::postfix_writer::do_sub_node(cdk::sub_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT))
    _pf.I2D();
  else if (node->is_typed(cdk::TYPE_POINTER) && node->left()->is_typed(cdk::TYPE_INT)) {
    process_pointer_reference(node->type());
    _pf.SHTL();
  }

  node->right()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT))
    _pf.I2D();
  else if (node->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT)) {
    process_pointer_reference(node->type());
    _pf.SHTL();
  }

  if (node->is_typed(cdk::TYPE_DOUBLE))
    _pf.DSUB();
  else
    _pf.SUB();

  /* return number of objects (>0) between both pointers */
  if (node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_POINTER)) {
    int positive_lbl = ++_lbl;
    _pf.SUB();
    _pf.INT(cdk::reference_type::cast(node->left()->type())->referenced()->size());
    _pf.DIV(); // get number of arguments (<0, 0 or >0)
    _pf.DUP32();
    _pf.INT(0);
    _pf.LT();
    _pf.JZ(mklbl(positive_lbl)); // jump to label if number is already positive. otherwise gets its positive value
    _pf.INT(-1);
    _pf.MUL();
    _pf.ALIGN();
    _pf.LABEL(mklbl(positive_lbl));
  }
}

void fir::postfix_writer::do_mul_node(cdk::mul_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  node->right()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  if (node->is_typed(cdk::TYPE_INT))
    _pf.MUL();
  else 
    _pf.DMUL();
}
void fir::postfix_writer::do_div_node(cdk::div_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  node->right()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  if (node->is_typed(cdk::TYPE_INT))
    _pf.DIV();
  else 
    _pf.DDIV(); 
}
void fir::postfix_writer::do_mod_node(cdk::mod_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.MOD();
}
void fir::postfix_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  node->right()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DCMP();
    _pf.INT(0);
  }

  _pf.LT();
    
}
void fir::postfix_writer::do_le_node(cdk::le_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  node->right()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DCMP();
    _pf.INT(0);
  }

  _pf.LE();
}
void fir::postfix_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  node->right()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DCMP();
    _pf.INT(0);
  }

  _pf.GE();
}
void fir::postfix_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  node->right()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DCMP();
    _pf.INT(0);
  }

  _pf.GT();
}
void fir::postfix_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  node->right()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DCMP();
    _pf.INT(0);
  }

  _pf.NE();
}
void fir::postfix_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  node->right()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DCMP();
    _pf.INT(0);
  }

  _pf.EQ();
}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_and_node(cdk::and_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl = ++_lbl;

  node->left()->accept(this, lvl);
  _pf.DUP32();
  _pf.JZ(mklbl(lbl));

  node->right()->accept(this, lvl);
  _pf.AND();

  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl));
}

void fir::postfix_writer::do_or_node(cdk::or_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl = ++_lbl;

  node->left()->accept(this, lvl);
  _pf.DUP32();
  _pf.JNZ(mklbl(lbl));

  node->right()->accept(this, lvl);
  _pf.OR();

  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl));
}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_neg_node(cdk::neg_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_INT))
    _pf.NEG();
  else
    _pf.DNEG();
}

void fir::postfix_writer::do_not_node(cdk::not_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl + 2);
  _pf.INT(0);
  _pf.EQ();
}


//---------------------------------------------------------------------------

void fir::postfix_writer::do_identity_node(fir::identity_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl);
}

void fir::postfix_writer::do_sizeof_node(fir::sizeof_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  _pf.INT(node->argument()->type()->size());
}

void fir::postfix_writer::do_stack_alloc_node(fir::stack_alloc_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl);
  process_pointer_reference(node->type());
  _pf.SHTL();
  _pf.ALLOC();
  _pf.SP();

}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_indexation_node(fir::indexation_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  node->base()->accept(this, lvl);
  node->index()->accept(this, lvl);
  _pf.INT(node->type()->size());
  _pf.MUL();
  _pf.ADD();
}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_variable_node(cdk::variable_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  const std::string &id = node->name();
  auto symbol = _symtab.find(id);
  if (_inFunctionBody && (_function->name() == node->name())) {
    _newReturn = true;
    _pf.LOCAL(_returnValueOffset);
  }
  else if (symbol->global())
    _pf.ADDR(symbol->name());
  else
    _pf.LOCAL(symbol->offset());
}

void fir::postfix_writer::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->lvalue()->accept(this, lvl);

  if (node->is_typed(cdk::TYPE_DOUBLE))
    _pf.LDDOUBLE();
  else
    _pf.LDINT();
}

void fir::postfix_writer::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->rvalue()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    if (node->rvalue()->is_typed(cdk::TYPE_INT))
      _pf.I2D();
    _pf.DUP64();
  } 
  else
    _pf.DUP32();

  node->lvalue()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE))
    _pf.STDOUBLE();
  else
    _pf.STINT();
}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_write_node(fir::write_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  for (size_t i = 0; i < node->arguments()->size(); i++) {
    auto child = node->argument(i);
    std::shared_ptr<cdk::basic_type> child_type = child->type();
    child->accept(this, lvl);

    if (child_type->name() == cdk::TYPE_INT) {
      _functions_to_declare.insert("printi");
      _pf.CALL("printi");
      _pf.TRASH(4);
    } 
    else if (child_type->name() == cdk::TYPE_DOUBLE) {
      _functions_to_declare.insert("printd");
      _pf.CALL("printd");
      _pf.TRASH(8);
    } 
    else if (child_type->name() == cdk::TYPE_STRING) {
      _functions_to_declare.insert("prints");
      _pf.CALL("prints");
      _pf.TRASH(4);
    }
  }
  if (node->newline()) {
    _functions_to_declare.insert("println");
    _pf.CALL("println");
  }
}

void fir::postfix_writer::do_read_node(fir::read_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if (node->is_typed(cdk::TYPE_INT)) {
    _functions_to_declare.insert("readi");
    _pf.CALL("readi");
    _pf.LDFVAL32();
  }
  else if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _functions_to_declare.insert("readd");
    _pf.CALL("readd");
    _pf.LDFVAL64();
  }
}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_if_node(fir::if_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl_end = ++_lbl;

  // if condition
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl_end));

  // then instruction
  node->instruction()->accept(this, lvl + 2);

  // end
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl_end));
}

void fir::postfix_writer::do_if_else_node(fir::if_else_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl_else, lbl_end;

  // if condition
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl_else = ++_lbl));

  // then instruction
  node->thenInstruction()->accept(this, lvl + 2);
  _pf.JMP(mklbl(lbl_end = ++_lbl));

  // else instruction
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl_else));
  node->elseInstruction()->accept(this, lvl + 2);

  // end
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl_end));
}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_while_node(fir::while_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  _whileInitLbls.push_back(++_lbl);
  _whileEndLbls.push_back(++_lbl);
  _symtab.push();

  // while condition
  _pf.ALIGN();
  _pf.LABEL(mklbl(_whileInitLbls.back()));
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(_whileEndLbls.back()));

  // instruction
  node->instruction()->accept(this, lvl + 2);

  _pf.JMP(mklbl(_whileInitLbls.back()));

  // end
  _pf.ALIGN();
  _pf.LABEL(mklbl(_whileEndLbls.back()));

  _symtab.pop();
  _whileInitLbls.pop_back();
  _whileEndLbls.pop_back();
}

void fir::postfix_writer::do_while_finally_node(fir::while_finally_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  _whileInitLbls.push_back(++_lbl);
  _whileEndLbls.push_back(++_lbl);
  _symtab.push();

  // while condition
  _pf.ALIGN();
  _pf.LABEL(mklbl(_whileInitLbls.back()));
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(_whileEndLbls.back()));

  // instruction
  node->doInstruction()->accept(this, lvl + 2);
  _pf.JMP(mklbl(_whileInitLbls.back()));

  // end (finally instruction)
  _pf.ALIGN();
  _pf.LABEL(mklbl(_whileEndLbls.back()));
  _inFinally = true;
  node->finallyInstruction()->accept(this, lvl + 2);
  _inFinally = false;

  _symtab.pop();
  _whileInitLbls.pop_back();
  _whileEndLbls.pop_back();
}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_restart_node(fir::restart_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if (_inFinally)
    error(node->lineno(), "'restart' inside 'finally'");
  else if (_whileInitLbls.size() == 0)
    error(node->lineno(), "'restart' outside 'while'");
  else if ((size_t) node->value() > _whileInitLbls.size())
    error(node->lineno(), "restart value doesn't match any cycle level");
  else
    _pf.JMP(mklbl(_whileInitLbls.at(_whileInitLbls.size() - (size_t) node->value())));
}

void fir::postfix_writer::do_leave_node(fir::leave_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if (_inFinally)
    error(node->lineno(), "'leave' inside 'finally'");
  else if (_whileEndLbls.size() == 0)
    error(node->lineno(), "'leave' outside 'while'");
  else if ((size_t) node->value() > _whileInitLbls.size())
    error(node->lineno(), "leave value doesn't match any cycle level");
  else
    _pf.JMP(mklbl(_whileEndLbls.at(_whileEndLbls.size() - (size_t) node->value())));
}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_variable_declaration_node(fir::variable_declaration_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  auto id = node->identifier();
  std::cout << "INITIAL OFFSET: " << _offset << std::endl;
  int offset = 0, typesize = node->type()->size();
  std::cout << "ARG: " << id << ", " << typesize << std::endl;
  if (_inFunctionBody) {
    std::cout << "IN BODY" << std::endl;
    _offset -= typesize;
    offset = _offset;
  } 
  else if (_inFunctionArgs) {
    std::cout << "IN ARGS" << std::endl;
    offset = _offset;
    _offset += typesize;
  } 
  else {
    std::cout << "GLOBAL!" << std::endl;
    offset = 0;
  }
  std::cout << "OFFSET: " << id << ", " << offset << std::endl;

  auto symbol = new_symbol();
  if (symbol) {
    symbol->set_offset(offset);
    reset_new_symbol();
  }

  if (_inFunctionBody) {
    if (node->initializer()) {
      node->initializer()->accept(this, lvl);
      if (node->is_typed(cdk::TYPE_INT) || node->is_typed(cdk::TYPE_STRING) || node->is_typed(cdk::TYPE_POINTER)) {
        _pf.LOCAL(symbol->offset());
        _pf.STINT();
      } 
      else if (node->is_typed(cdk::TYPE_DOUBLE)) {
        if (node->initializer()->is_typed(cdk::TYPE_INT))
          _pf.I2D();

        _pf.LOCAL(symbol->offset());
        _pf.STDOUBLE();
      }
      else
        error(node->lineno(), "cannot initialize");
    }
  } 
  else if (!_function) {
    if (!node->initializer()) {
      _pf.BSS();
      _pf.ALIGN();
      _pf.LABEL(id);
      _pf.SALLOC(typesize);
    } 
    else if (node->is_typed(cdk::TYPE_INT) || node->is_typed(cdk::TYPE_DOUBLE) || node->is_typed(cdk::TYPE_POINTER)) {
      _pf.DATA();
      _pf.ALIGN();
      _pf.LABEL(id);

      if (node->is_typed(cdk::TYPE_DOUBLE)) {
        if (node->initializer()->is_typed(cdk::TYPE_DOUBLE)) {
          node->initializer()->accept(this, lvl);
        } 
        else if (node->initializer()->is_typed(cdk::TYPE_INT)) {
          cdk::integer_node *dclini = dynamic_cast<cdk::integer_node*>(node->initializer());
          cdk::double_node ddi(dclini->lineno(), dclini->value());
          ddi.accept(this, lvl);
        } 
        else
          error(node->lineno(), ": '" + id + "' has bad initializer for real value");
      }
      else
        node->initializer()->accept(this, lvl);
    }
    else if (node->is_typed(cdk::TYPE_STRING)) {
      _pf.DATA();
      _pf.ALIGN();
      _pf.LABEL(id);
      node->initializer()->accept(this, lvl);
    }
    else
      error(node->lineno(), ": '" + id + "' has unexpected initializer");
  }
}

void fir::postfix_writer::do_function_declaration_node(fir::function_declaration_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  if (!new_symbol()) 
    return;

  auto function = new_symbol();
  _functions_to_declare.insert(function->name());
  reset_new_symbol();
}


void fir::postfix_writer::do_function_definition_node(fir::function_definition_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  _epilogueLbl = ++_lbl;
  _functionReturnLbl = ++_lbl;
  std::string id = process_function_id(node->identifier());
  _newReturn = false;

  _function = new_symbol();
  _function->set_name(node->identifier());
  _functions_to_declare.erase(_function->name());
  reset_new_symbol();
  _offset = 8;
  _symtab.push();

  if (node->arguments()->size() > 0) {
    _inFunctionArgs = true;
    node->arguments()->accept(this, lvl + 2);
    _inFunctionArgs = false;
  }

  _pf.TEXT();
  _pf.ALIGN();
  if (is_public(node->qualifier()))
    _pf.GLOBAL(id, _pf.FUNC());

  _pf.LABEL(id);

  frame_size_calculator lsc(_compiler, _symtab, _function);
  node->accept(&lsc, lvl);
  _pf.ENTER(lsc.localsize());

 
  _offset = -node->type()->size();
  _returnValueOffset = _offset;

  _inFunctionBody = true;

  if (node->defReturnValue() || _newReturn) {
    node->defReturnValue()->accept(this, lvl);
    if (node->is_typed(cdk::TYPE_INT) || node->is_typed(cdk::TYPE_STRING) || node->is_typed(cdk::TYPE_POINTER)) {
      _pf.LOCAL(_returnValueOffset);
      _pf.STINT();
    } 
    else if (node->is_typed(cdk::TYPE_DOUBLE)) {
      if (node->defReturnValue()->is_typed(cdk::TYPE_INT))
        _pf.I2D();

      _pf.LOCAL(_returnValueOffset);
      _pf.STDOUBLE();
    }
  }
  
  os() << "        ;; before body " << std::endl;
  _inFunctionBody = true;

  if (node->prologue())
    node->prologue()->accept(this, lvl + 2);

  if (node->block()) {
    _symtab.push();
    node->block()->accept(this, lvl + 2);
    _symtab.pop();
  }

  _pf.LABEL(mklbl(_epilogueLbl));
  if (node->epilogue()) {
    _inEpilogue = true;
    _symtab.push();
    node->epilogue()->accept(this, lvl + 2);
    _symtab.pop();
    _inEpilogue = false;
  }

  os() << "        ;; after body " << std::endl;
  _inFunctionBody = false;

  if (is_main_function(id))
    for (std::string s : _functions_to_declare)
      _pf.EXTERN(s);
  

  _pf.LABEL(mklbl(_functionReturnLbl));

  
  if (node->defReturnValue() || _newReturn) {
    _pf.LOCAL(_returnValueOffset);
    if (node->is_typed(cdk::TYPE_DOUBLE)) {
      _pf.LDDOUBLE();
      _pf.STFVAL64();
    }
    else {
      _pf.LDINT();
      _pf.STFVAL32();
    }
  }
  else if (node->is_typed(cdk::TYPE_INT) || node->is_typed(cdk::TYPE_POINTER)) {
    
    _pf.INT(0);
    _pf.STFVAL32();
  }

  _pf.LEAVE();
  _pf.RET();
  _symtab.pop();
}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_block_node(fir::block_node * const node, int lvl) {
  if (node->declarations())
    node->declarations()->accept(this, lvl + 2);
  if (node->instructions())
    node->instructions()->accept(this, lvl + 2);
}

void fir::postfix_writer::do_prologue_node(fir::prologue_node * const node, int lvl) {
  node->block()->accept(this, lvl + 2);
}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_return_node(fir::return_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if (_inEpilogue)
    _pf.JMP(mklbl(_functionReturnLbl));
  else {
    _pf.JMP(mklbl(_epilogueLbl));
  }
}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_address_of_node(fir::address_of_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->lvalue()->accept(this, lvl + 2);
}

void fir::postfix_writer::do_function_call_node(fir::function_call_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  std::string id = process_function_id(node->identifier());

  auto symbol = _symtab.find(id);
  size_t  arguments_size = 0;

  if (node->arguments()->size() > 0)
    for (int i = node->arguments()->size() - 1; i >= 0; i--) {
      cdk::expression_node *arg = dynamic_cast<cdk::expression_node*>(node->arguments()->node(i));
      arg->accept(this, lvl + 2);
      if (symbol->argument_is_typed(i, cdk::TYPE_DOUBLE) && arg->is_typed(cdk::TYPE_INT))
        _pf.I2D();
       arguments_size += symbol->argument_size(i);
    }

  _pf.CALL(node->identifier());
  if ( arguments_size != 0)
    _pf.TRASH( arguments_size);

  if (symbol->is_typed(cdk::TYPE_DOUBLE))
    _pf.LDFVAL64();
  else
    _pf.LDFVAL32();
}