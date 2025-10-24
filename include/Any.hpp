#ifndef MUDUO_ANY_H
#define MUDUO_ANY_H

#include <typeinfo>
#include <utility>

class Any {
    class Base {
    public:
        virtual ~Base() = default;
        virtual const std::type_info& type() const = 0;
        virtual Base* clone() const = 0;
    };

    template<typename T>
    class Derived : public Base {
    public:
        T _value;

        explicit Derived(const T& val) : _value(val) {}
        // 完美转发构造
        template<typename U>
        Derived(U&& val) : _value(std::forward<U>(val)) {}

        const std::type_info& type() const override {
            return typeid(T);
        }

        Base* clone() const override {
            return new Derived(_value);
        }

        T& getValue() { return _value; }
        const T& getValue() const { return _value; }
    };

    Base* _content;

    Any& swap(Any& other) {
        std::swap(_content, other._content);
        return *this;
    }

public:
    Any() : _content(nullptr) {}
    ~Any() { delete _content; }

    bool has_value() const { return _content != nullptr; }

    template<typename T>
    Any(const T& value) : _content(new Derived<T>(value)) {}

    Any(const Any& other) : _content(other._content ? other._content->clone() : nullptr) {}

    Any(Any&& other) noexcept : _content(other._content) {
        other._content = nullptr;
    }

    template<typename T>
    Any& operator=(const T& val) {
        Any(val).swap(*this);
        return *this;
    }

    Any& operator=(const Any& other) {
        Any(other).swap(*this);
        return *this;
    }

    template<typename T>
    T* get() {
        if (!has_value()) return nullptr;
        if (_content->type() == typeid(T)) {
            return &(static_cast<Derived<T>*>(_content)->_value);
        }
        return nullptr;
    }

    template<typename T>
    const T* get() const {
        if (!has_value()) return nullptr;
        if (_content->type() == typeid(T)) {
            return &(static_cast<Derived<T>*>(_content)->_value);
        }
        return nullptr;
    }

    const std::type_info& type() const {
        return has_value() ? _content->type() : typeid(void);
    }

    void reset() noexcept {
        delete _content;
        _content = nullptr;
    }
};

#endif