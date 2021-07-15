#ifndef __FIR_TARGETS_POSTFIX_WRITER_H__
#define __FIR_TARGETS_POSTFIX_WRITER_H__

#include "targets/basic_ast_visitor.h"

#include <sstream>
#include <cdk/emitters/basic_postfix_emitter.h>
#include "targets/frame_size_calculator.h"
#include <set>

namespace fir {

  //!
  //! Traverse syntax tree and generate the corresponding assembly code.
  //!
  class postfix_writer: public basic_ast_visitor {
    std::set<std::string> _functions_to_declare;
    cdk::symbol_table<fir::symbol> &_symtab;
    cdk::basic_postfix_emitter &_pf;
    
    /* values */
    int _lbl;
    int _offset;

    /* function symbol to keep track of it */
    std::shared_ptr<fir::symbol> _function;

    /* flags */
    bool _inFunctionBody;
    bool _inFunctionArgs;
    bool _inEpilogue;
    bool _inFinally;
    bool _newReturn;

    /* labels */
    int _functionReturnLbl;
    int _returnValueOffset;
    int _epilogueLbl;

    /* for interruptions (restart/leave) in while loops */
    std::vector<int> _whileInitLbls = std::vector<int>();
    std::vector<int> _whileEndLbls = std::vector<int>();

  public:
    postfix_writer(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<fir::symbol> &symtab,
                   cdk::basic_postfix_emitter &pf) :
        basic_ast_visitor(compiler), _symtab(symtab), _pf(pf), _lbl(0),
        _function(nullptr),
        _inFunctionBody(false), _inFunctionArgs(false), _inEpilogue(false), _inFinally(false), _newReturn(false),
        _returnValueOffset(0), _epilogueLbl(0) {
    }

  public:
    ~postfix_writer() {
      os().flush();
    }

  protected:
    bool is_main_function(std::string id);
    bool is_public(int qualifier);
    bool is_private(int qualifier);
    bool is_extern(int qualifier);
    std::string process_function_id(std::string id);
    void process_pointer_reference(std::shared_ptr<cdk::basic_type> type);
    void create_local_variable(cdk::expression_node * node, int offset);
    
  private:
    /** Method used to generate sequential labels. */
    inline std::string mklbl(int lbl) {
      std::ostringstream oss;
      if (lbl < 0)
        oss << ".L" << -lbl;
      else
        oss << "_L" << lbl;
      return oss.str();
    }

    void error(int lineno, std::string s) {
      std::cerr << "error: " << lineno << ": " << s << std::endl;
      exit(2);
    }

  public:
  // do not edit these lines
#define __IN_VISITOR_HEADER__
#include "ast/visitor_decls.h"       // automatically generated
#undef __IN_VISITOR_HEADER__
  // do not edit these lines: end

  };

} // fir

#endif
