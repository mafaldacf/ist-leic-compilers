#ifndef __FIR_TARGETS_SYMBOL_H__
#define __FIR_TARGETS_SYMBOL_H__

#include <string>
#include <memory>
#include <cdk/types/basic_type.h>

namespace fir {

  class symbol {
    int _qualifier;
    std::shared_ptr<cdk::basic_type> _type;
    std::vector<std::shared_ptr<cdk::basic_type>> _argument_types;
    std::string _name;
    bool _defined;
    bool _function;
    int _offset = 0;

    // bool epilogue?


    long _value = 0;// hack!

  public:
    symbol(int qualifier, std::shared_ptr<cdk::basic_type> type, const std::string &name, bool defined, bool function) :
        _qualifier(qualifier), _type(type), _name(name), _defined(defined), _function(function) {
    }

    virtual ~symbol() {
      // EMPTY
    }
    void set_name(const std::string &id){
      _name = id;
    }
    int qualifier() const {
      return _qualifier;
    }
    std::shared_ptr<cdk::basic_type> type() const {
      return _type;
    }
    std::shared_ptr<cdk::basic_type> argument_type(int i) const {
      return _argument_types[i];
    }
    bool is_typed(cdk::typename_type name) const {
      return _type->name() == name;
    }
    bool argument_is_typed(int i, cdk::typename_type name) const {
      return _argument_types[i]->name() == name;
    }
    void set_argument_types(const std::vector<std::shared_ptr<cdk::basic_type>> &types) {
      _argument_types = types;
    }
    const std::string &name() const {
      return _name;
    }
    long value() const {
      return _value;
    }
    long value(long v) {
      return _value = v;
    }
    bool is_defined() const {
      return _defined;
    }
    bool is_initialized() const {
      return _defined;
    }
    bool is_function() const {
      return _function;
    }
    bool is_variable() const {
      return !_function;
    }
    size_t number_of_arguments() const {
      return _argument_types.size();
    }
    int offset() const {
      return _offset;
    }
    void set_offset(int offset) {
      _offset = offset;
    }
    void set_defined() {
      _defined = true;
    }
    bool global() const {
      return _offset == 0;
    }
    std::shared_ptr<cdk::basic_type> argument_type(size_t i) const {
      return _argument_types[i];
    }
    size_t argument_size(size_t ax) const {
      return _argument_types[ax]->size();
    }
  };

  inline auto make_symbol(int qualifier, std::shared_ptr<cdk::basic_type> type, const std::string &name,
    bool defined, bool function) {
      return std::make_shared<symbol>(qualifier, type, name, defined, function);
    }

} // fir

#endif
