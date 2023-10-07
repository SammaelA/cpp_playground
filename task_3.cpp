#include <iostream>
#include <memory>

template<typename BaseType>
class AllocatorWrapper
{
public:
template<typename T>
class Allocator
{
public:
    typedef T value_type;
    Allocator() noexcept {}
    template <class U> Allocator(const Allocator<U>&) noexcept {}
    T* allocate (std::size_t n) 
    {
        return reinterpret_cast<T*>( BaseType::operator new(n*sizeof(T)));
    }
    void deallocate (T* p, std::size_t n) 
    {
        BaseType::operator delete(p, n);
    }
};
};
class A
{
public:
    // ...
    static void *operator new(size_t size)
    {
        std::cout << "operator new!" << std::endl;
        return ::operator new(size);
    }
    static void operator delete(void *p, size_t size)
    {
        std::cout << "operator delete!" << std::endl;
        return ::operator delete(p);
    }
};

int main()
{
    AllocatorWrapper<A>::Allocator<A> alloc;
    auto sp = std::allocate_shared<A>(alloc);
    return 0;
}