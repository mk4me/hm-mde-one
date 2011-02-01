#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <stdio.h>

#define LOG(type) (type)&&(type)

//*******************************************************
// Policies

// Wypisywanie na stdout
struct CStdoutDispatchPolicy
{
  void Initialize(const std::string &i_string)
  { m_name = i_string; i = 1; }

	void Dispatch(const std::string &i_string)
  { std::clog << i << "> " << i_string; }
  //{ std::clog << i_string; }

  void Close()
  {} 

  void increment()
  {
    ++i; 
  }

private:
  std::string m_name; 
  int i; 
};

// Wypisywanie do pliku
struct CFileDispatchPolicy
{
  void Initialize(const std::string &i_string)
  { 
    SYSTEMTIME st; 
    ::GetLocalTime(&st);
    // Determine day of the week.
    std::string day;
    switch (st.wDayOfWeek)
    {
    case 0:
      day = "Sunday";
      break;
    case 1:
      day = "Monday";
      break;
    case 2:
      day = "Tuesday";
      break;
    case 3:
      day = "Wednesday";
      break;
    case 4:
      day = "Thursday";
      break;
    case 5:
      day = "Friday";
      break;
    case 6:
      day = "Saturday";
      break;
    }

    std::string temp = i_string; 
    temp.append(".log");
    fopen_s(&pFile, temp.c_str(), "w+"); 
    fprintf(pFile, "%s %02d.%02d.%u  %02d:%02d:%02d \n\n", 
      day.c_str(), st.wDay, st.wMonth, st.wYear,
      st.wHour, st.wMinute, st.wSecond);
  }

	void Dispatch(const std::string &i_string)
  {
    fprintf(pFile, "%s", i_string.c_str()); 
  }

  void Close()
  {
    fclose(pFile);
  }
private:
  FILE* pFile; 
};

// pobieranie konfiguracji z pliku
struct CFileConfigurationPolicy
{
  bool GetLoggingFlag(std::string i_name)
  {
    return true; 
    //if(g_config.GetIsInitialized())
    //{
    //  if(g_config.GetIntParam(i_name)>0)
    //    return true; 
    //  else
    //    return false; 
    //}
    //else
    //  return true; 
  };
};

//*******************************************************
// LogImplementation
template <
  class TDispatchPolicy, 
  class TConfigurationPolicy
>
class CLogger
{
  std::stringstream buffer_; 
  std::string name_; 

  TDispatchPolicy dispatchPolicy_; 
  TConfigurationPolicy configurationPolicy_; 
  // wypisywanie bufora
  // zwraca zawsze true !! 
  //bool Dispatch(); 
  bool Dispatch()
  {
	  // Convert buffer to string.
    //const std::string l_string = m_buffer.str();
    std::string l_string = buffer_.str();

	  // Only dispatch when the buffer contains something.
	  if(!l_string.empty()) {
		  // Convert buffer to string and dispatch that string.
		  dispatchPolicy_.Dispatch(l_string);

		  // Clear the buffer.
		  //t_BufferPolicy::m_Clear(m_buffer);
      buffer_.str(std::string());
	  }

	  // Always return true.
	  return(true);
  }
  bool m_flag; 

public:
  CLogger(void):
    m_flag(true)
   ,name_("") 
  {
    dispatchPolicy_.Initialize(name_); 
  }

  CLogger(const char* i_name):
    m_flag(true)
   ,name_(i_name) 
  {
    dispatchPolicy_.Initialize(name_); 
    m_flag = configurationPolicy_.GetLoggingFlag(name_); 
  }

  ~CLogger(void)
  {
    dispatchPolicy_.Close(); 
  }

  void increment()
  {
    dispatchPolicy_.increment();
  };

  void setDispatchFlag(bool flag)
  {
    m_flag = flag; 
  }

  operator bool() 
  { return(m_flag && Dispatch()); }

  template <typename t_Type>
		CLogger<TDispatchPolicy, TConfigurationPolicy>& operator<<(t_Type i_value)
	{ buffer_ << i_value; return(*this); }
};

//*******************************************************
// Typy


typedef CLogger<CStdoutDispatchPolicy, CFileConfigurationPolicy> LogStdout; 
typedef CLogger<CFileDispatchPolicy, CFileConfigurationPolicy> LogFile; 