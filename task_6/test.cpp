#include <gtest/gtest.h>
#include <iostream>
#include <exception>
#include <math.h>

class TMyException : public std::exception
{
public:

    template <typename T>
    TMyException& operator<<(const T& val)
    {
        message += std::to_string(val);
        return *this;      
    }

    TMyException& operator<<(const char *_message)
    {
        message += _message;
        return *this;
    }

    TMyException& operator<<(const std::string& _message)
    {
        message += _message;
        return *this;
    }
    const char* what() const noexcept override {return message.c_str();}
protected:
    std::string message;
};

class FirstMyException : public TMyException
{
};
 
class SecondMyException : public TMyException
{
public: 
};

double safe_log(double base, double val)
{
  if (base < 0)
  {
    FirstMyException e;
    e << "Logaritm base is "<<base<<", expected base > 0";
    throw e;
  }
  else if (val < 0)
  {
    SecondMyException e;
    e << "Logaritm x is "<<val<<", expected x > 0";
    throw e;  
  }
  return log(val)/log(base);
}

// Demonstrate some basic assertions.
TEST(ExceptionTest, NoThrow) {
  EXPECT_NO_THROW(safe_log(2,32));
}
TEST(ExceptionTest, Throw1) {
  EXPECT_THROW(safe_log(-1,32), FirstMyException);
}
TEST(ExceptionTest, Throw2) {
  EXPECT_THROW(safe_log(32,-1), SecondMyException);
}
