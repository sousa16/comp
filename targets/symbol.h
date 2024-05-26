#ifndef __SIMPLE_TARGETS_SYMBOL_H__
#define __SIMPLE_TARGETS_SYMBOL_H__

#include <cdk/types/basic_type.h>

#include <memory>
#include <string>

namespace til {

class symbol {
    std::shared_ptr<cdk::basic_type> _type;
    std::string _name;
    int _qualifier;
    long _value;  // hack!

   public:
    symbol(std::shared_ptr<cdk::basic_type> type, const std::string &name, long value) : _type(type), _name(name), _value(value) {
    }

    virtual ~symbol() {
        // EMPTY
    }

    std::shared_ptr<cdk::basic_type> type() const {
        return _type;
    }
    bool is_typed(cdk::typename_type name) const {
        return _type->name() == name;
    }

    const std::string &name() const {
        return _name;
    }
    int qualifier() const {
        return _qualifier;
    }
    long value() const {
        return _value;
    }
    long value(long v) {
        return _value = v;
    }
};

}  // namespace til

#endif
