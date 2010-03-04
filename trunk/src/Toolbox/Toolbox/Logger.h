#pragma once 

#include <string>
#include <vector>
#include <streambuf>

#include "ConsoleWidget.h"

class ConsoleWidgetOutputBuffer : public std::streambuf 
{
public:
  ConsoleWidgetOutputBuffer(ConsoleWidget *console, std::size_t buff_sz = 256): 
    _buffer(buff_sz + 1)
  , _console(console)
  {
    char *base = &_buffer.front();
    setp(base, base + _buffer.size() - 1);
  }

  virtual ~ConsoleWidgetOutputBuffer()
  {
    _console = NULL; 
    _buffer.clear(); 
  }
private: 
  virtual int_type overflow(int_type c = traits_type::eof()) 
  {
    *pptr() = c;
    pbump(1);
    flush_buffer();
    return c;
  }

  virtual int sync()
  {
    return flush_buffer() ? 0 : -1;
  }

  bool flush_buffer()
  {
    std::string buf = std::string(pbase(), pptr()); 
    _console->print(buf); 
    char *base = &_buffer.front();
    setp(base, base + _buffer.size() - 1);
    return true; 
  }
private: 
  ConsoleWidget *_console; 
  std::vector<char> _buffer;
};

/*
template <class TDispatchPolicy, class TOutputClass> 
class Logger
{
public:
  static Logger<TDispatchPolicy, TOutputClass> &getInstance()
  {
    if(instance_ == 0)
    {
      instance_ = new Logger<TDispatchPolicy, TOutputClass>(); 
    }
    return *instance_; 
  }
  static void clear()
  {
    if(instance_ != 0)
    {
      delete instance_; 
      instance_ = 0; 
    }
  }

  static void initialize(const std::string &i_string, TOutputClass o)
  { 
    if(instance_ != 0)
    {
      instance_->dispatchPolicy_.initialize(i_string, o);   
    }
    else
    {
      instance_ = new Logger<TDispatchPolicy, TOutputClass>(); 
      instance_->dispatchPolicy_.initialize(i_string, o);   
    }
  }

  virtual ~Logger() {}; 

  template<typename T> 
  Logger<TDispatchPolicy, TOutputClass> &operator<<(T const& t) 
  { buffer_ << t; return *this; }

  operator bool() 
  { return(dispatch()); }

protected: 
  Logger() {}; 

private: 
  std::stringstream buffer_; 
  TDispatchPolicy dispatchPolicy_; 
  static Logger<TDispatchPolicy, TOutputClass> *instance_; 

  bool dispatch()
  {
    //const std::string l_string = m_buffer.str();
    std::string l_string = buffer_.str();

    if(!l_string.empty()) 
    {
      dispatchPolicy_.dispatch(l_string);

      buffer_.str(std::string());
    }
    return true; 
  }
};

template <class TDispatchPolicy, class TOutputClass> 
Logger<TDispatchPolicy, TOutputClass> *Logger<TDispatchPolicy, TOutputClass>::instance_ = 0; 
*/