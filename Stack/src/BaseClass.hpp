
#ifndef STACK_BASECLASS_HPP
#define STACK_BASECLASS_HPP

template <class elemType>
class stack
{
public:
    [[nodiscard]] virtual bool isEmpty() const = 0;
    virtual void push(const elemType &x) = 0;
    virtual elemType  pop() = 0;
    virtual elemType&  top() const = 0;
    virtual ~stack() {}
};



#endif //STACK_BASECLASS_HPP