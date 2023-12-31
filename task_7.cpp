#include <iostream>
#include <memory>

template <typename T>
class alignas(8) TBasePtr
{
public:
  TBasePtr() = default;
  inline T* operator->() const { return ptr;}
  inline T& operator*() const { return *ptr;}
  inline bool operator==(const TBasePtr &p) const { return p.ptr == ptr; }
  inline bool operator!=(const TBasePtr &p) const { return p.ptr != ptr; }
  inline operator bool() const {return ptr; }

protected:
  T *ptr = nullptr;
};

template <typename T>
class alignas(8) TIntrusivePtr : public TBasePtr<T>
{
public:
  TIntrusivePtr() = default;
  TIntrusivePtr(const TIntrusivePtr<T> &p):
  TIntrusivePtr(p.ptr)
  { }

  TIntrusivePtr(T *raw_ptr)
  {
    this->ptr = raw_ptr;
    if (raw_ptr)
      raw_ptr->intrusive_ptr_count++;
  }
  ~TIntrusivePtr()
  {
    if (this->ptr)
    {
      this->ptr->intrusive_ptr_count--;
      if (this->ptr->intrusive_ptr_count == 0)
        delete this->ptr;
    }
  }

  inline int UseCount() const
  {
    if (this->ptr)
      return this->ptr->intrusive_ptr_count;
    else
      return 0;
  }

  inline T *Get() { return this->ptr;}
  inline T *Release()
  {
    T *ret_ptr = this->ptr;
    if (this->ptr)
    {
      this->ptr->intrusive_ptr_count--;
      this->ptr = nullptr;
    }
    return ret_ptr;
  }
  inline void Reset(TIntrusivePtr<T> new_ptr = TIntrusivePtr<T>())
  {
    if (this->ptr != new_ptr.ptr)
    {
      if (this->ptr)
      {
        this->ptr->intrusive_ptr_count--;
        if (this->ptr->intrusive_ptr_count == 0)
          delete this->ptr;
      }
      this->ptr = new_ptr.ptr;
      if (new_ptr.ptr)
        new_ptr.ptr->intrusive_ptr_count++;
    }
  }

  inline TIntrusivePtr& operator=(const TIntrusivePtr& other) // copy assignment
  {
    Reset(other);
    return *this;
  }
 
  inline TIntrusivePtr& operator=(TIntrusivePtr&& other) noexcept // move assignment
  {
    std::swap(this->ptr, other.ptr);
    return *this;
  }
};

  template<typename T, class... Args >
  TIntrusivePtr<T> MakeIntrusive( Args&&... args )
  {
    return TIntrusivePtr<T>(new T(std::forward<Args>(args)...));
  }

template <typename T>
class TRefCounter
{
public:
  friend class TIntrusivePtr<T>;
  int RefCount() const { return intrusive_ptr_count; }
private:
  int intrusive_ptr_count = 0;
};

class TDoc : public TRefCounter<TDoc>
{
public:
  TDoc() { std::cout << "Document created\n";}
  ~TDoc() { std::cout << "Document deleted\n";}
};

int main()
{
  {
    TIntrusivePtr<TDoc> ptr = nullptr;
    ptr = MakeIntrusive<TDoc>(); // ptr.RefCount() == 1
    TIntrusivePtr<TDoc> ptr2 = ptr; // ptr.RefCount() == 2
    TIntrusivePtr<TDoc> ptr3 = MakeIntrusive<TDoc>();
    ptr3.Reset(ptr2); // ptr.RefCount() == ptr3.RefCount() == 3
    ptr3.Reset(); // ptr.RefCount() == ptr3.RefCount() == 2
    ptr3.Reset(std::move(ptr2)); // ptr.RefCount() == ptr3.RefCount() == 2
    static_assert(sizeof(TDoc*) == sizeof(ptr));
  }
  std::cout << "Program finished\n";
}