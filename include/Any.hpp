#ifndef MUDUO_ANY_H
#define MUDUO_ANY_H

#include <typeinfo>
#include <utility>
/*通用类型any-容器，保存不同类型的数据*/
/*控制协议处理的上下文来控制处理节奏*/
/*c++17 有any*/
class Any
{
    /*利用多态，让父类指针指向子类对象从而调用子类对象，这样any在定义时就不需要指定类型了*/
    class Base
    {
    public:
        virtual ~Base() = default;
        virtual const std::type_info &type() const = 0; // 获取类型
        virtual Base *clone() const = 0;                // 克隆
    };

    template <typename T>
    class Derived : public Base
    {
    public:
        T _value;

        explicit Derived(const T &val) : _value(val) {}
        // 完美转发构造
        template <typename U>
        Derived(U &&val) : _value(std::forward<U>(val)) {}

        const std::type_info &type() const override
        {
            return typeid(T);
        }

        Base *clone() const override /*克隆新的子类对象*/
        {
            return new Derived(_value);
        }

        T &getValue() { return _value; }
        const T &getValue() const { return _value; }
    };

    Base *_content; // 父类指针

public:
    Any() : _content(nullptr) {}
    ~Any() { delete _content; }

    bool has_value() const { return _content != nullptr; }

    template <typename T>
    Any(const T &value) : _content(new Derived<T>(value)) {}

    Any(const Any &other) : _content(other._content ? other._content->clone() : nullptr) {}

    Any(Any &&other) noexcept : _content(other._content)
    {
        other._content = nullptr;
    }

    template <typename T>
    Any &operator=(const T &val)
    {
        Any(val).swap(*this);
        return *this;
    }
    Any &operator=(const Any &other)
    {
        Any(other).swap(*this);
        return *this;
    }
     Any &swap(Any &other)
    {
        std::swap(_content, other._content);
        return *this;
    }

    /*获取子类对象保存的数据的指针，两个版本-分别是const和非const*/
    template <typename T>
    T *get()
    {
        if (!has_value())
            return nullptr;
        if (_content->type() == typeid(T))
        {
            return &(static_cast<Derived<T> *>(_content)->_value);
        }
        return nullptr;
    }
    template <typename T>
    const T *get() const
    {
        if (!has_value())
            return nullptr;
        if (_content->type() == typeid(T))
        {
            return &(static_cast<Derived<T> *>(_content)->_value);
        }
        return nullptr;
    }
   
    const std::type_info &type() const
    {
        return has_value() ? _content->type() : typeid(void);
    }

    void reset() noexcept
    {
        delete _content;
        _content = nullptr;
    }
};

#endif