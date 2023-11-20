#include <gtest/gtest.h>
#include <iostream>
#include <exception>
#include <math.h>

class TMyException : public std::exception
{
public: 
    virtual TMyException& operator<<(const std::string& _message)
    {
        message = "TMyException:"+_message;
        return *this;
    }
    const char* what() const noexcept override {return message.c_str();}
protected:
    std::string message;
};

class FirstMyException : public TMyException
{
public: 
    FirstMyException& operator<<(const std::string& _message) override
    {
        message = "FirstMyException:"+_message;
        return *this;
    }
};
 
class SecondMyException : public TMyException
{
public: 
    SecondMyException& operator<<(const std::string& _message) override
    {
        message = "SecondMyException:"+_message;
        return *this;
    }
};

double safe_log(double base, double val)
{
  if (base < 0)
  {
    FirstMyException e;
    e << "Unable to take logarithm with base<=0";
    throw e;
  }
  else if (val < 0)
  {
    SecondMyException e;
    e << "Unable to take logarithm from x<=0";
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
