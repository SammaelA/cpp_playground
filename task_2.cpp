#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <tuple>
#include <sstream>

struct TPrettyPrinter 
{
public:
  std::string Str() const
  {
    return stream.str();
  }
  TPrettyPrinter& Format(short a) { stream << a; return *this; }
  TPrettyPrinter& Format(int   a) { stream << a; return *this; }
  TPrettyPrinter& Format(long  a) { stream << a; return *this; }
  TPrettyPrinter& Format(unsigned short a) { stream << a; return *this; }
  TPrettyPrinter& Format(unsigned int   a) { stream << a; return *this; }
  TPrettyPrinter& Format(unsigned long  a) { stream << a; return *this; }
  TPrettyPrinter& Format(const std::string &a) { stream << a; return *this; }

  template <typename InputIterator>
  void FormatCont(InputIterator begin, InputIterator end)
  {
    auto it = begin;
    Format(*it);
    it++;
    while (it != end)
    {
      stream << ", ";
      Format(*it);
      it++;
    }
  }

  template <typename T>
  TPrettyPrinter& Format(const std::vector<T> &v)
  {
    stream << "[";
    if (!v.empty())
    {
      FormatCont(v.begin(), v.end());
    }
    stream << "]";
    return *this; 
  }

  template <typename T>
  TPrettyPrinter& Format(const std::set<T> &v)
  {
    stream << "{";
    if (!v.empty())
    {
      FormatCont(v.begin(), v.end());
    }
    stream << "}";
    return *this; 
  }

  template <std::size_t I = 0, typename ... Ts>
  void FormatTuple(const std::tuple<Ts...> tup) 
  {
    if constexpr (I == sizeof...(Ts)) 
    {
      return;
    } 
    else 
    {
      Format(std::get<I>(tup));
      if constexpr (I+1 != sizeof...(Ts))
        stream << ", ";
      FormatTuple<I+1>(tup);
    }
  }

  template <std::size_t I = 0, typename ... Ts>
  TPrettyPrinter& Format(const std::tuple<Ts...> tup) 
  {
    stream << "[";
    FormatTuple(tup);
    stream << "]";
    return *this;
  }

  template <typename T1, typename T2>
  TPrettyPrinter& Format(const std::pair<T1, T2> &p)
  {
    stream << "(";
    Format(p.first);
    stream << ", ";
    Format(p.second);
    stream << ")";
    return *this; 
  }


private:
  std::stringstream stream;
};

template<typename T>
std::string Format(const T& t) 
{
  return TPrettyPrinter().Format(t).Str();
}

int main()
{
  std::tuple<std::string, int, int> t = {"xyz", 1, 2};
  std::vector<std::pair<int, int> > v = {{1, 4}, {5, 6}};
  std::set<long> s = {1, 4, 5, 6, 1, 2, 3};

  std::string s1 = TPrettyPrinter().Format("vector: ").Format(v).Str();
  std::string s2 = TPrettyPrinter().Format(t).Format(" ! ").Format(0).Str();
  std::string s3 = TPrettyPrinter().Format("set: ").Format(s).Str();
  std::cout<<s1<<"\n";
  std::cout<<s2<<"\n";
  std::cout<<s3<<"\n";
  return 0;
}