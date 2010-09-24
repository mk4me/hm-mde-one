/********************************************************************
	created:  2010/07/30
	created:  30:7:2010   18:25
	filename: PtrWrapper.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__UTILS_PTRWRAPPER_H__
#define __HEADER_GUARD__UTILS_PTRWRAPPER_H__

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

template <class T, class PtrPolicy>
class Adapter : public PtrPolicy::Base
{
private:
  //! Wartoœæ.
  T value;

public:
  //!
  Adapter()
  {}
  //!
  Adapter(const T& value) : value(value) 
  {}
  //! 
  Adapter(const Adapter& rhs) : value(rhs.value) 
  {}
  //!
  Adapter& operator=(const T& value) 
  { 
    this->value = value;
    return *this; 
  };
  //!
  operator T&()
  {
    return value;
  }

  //!
  T& get() 
  { 
    return value; 
  }
  //!
  const T& get() const 
  { 
    return value; 
  }
};

/**
 *	Typ wrapuj¹cy wskaŸniki typów abstrakcyjnych, które nie maj¹ klasy bazowej niezbêdnej,
 *  aby mog³y byæ u¿ywane w "m¹drych" wskaŸnikach.
 */
template <class T, class PtrPolicy>
class PtrWrapper : public PtrPolicy::Base
{
private:
  //! Wartoœæ.
  T * ptr;

public:
  //!
  PtrWrapper() : ptr(NULL) 
  {}
  //!
  PtrWrapper(T* ptr) : ptr(ptr) 
  {}
  //! 
  PtrWrapper(const PtrWrapper& rhs) : ptr(rhs.ptr) 
  {}
  //!
  template <class Other> 
  PtrWrapper(const PtrWrapper<Other, PtrPolicy>& rhs) : ptr(rhs.ptr) 
  {}
  //!
  ~PtrWrapper() 
  { 
    delete ptr; 
  }

  //!
  PtrWrapper& operator = (T* ptr) 
  { 
    if ( ptr != this->ptr ) {
      delete this->ptr;
      this->ptr = ptr; 
    }
    return *this; 
  };

  //! implicit output conversion
  operator T*() const 
  { 
    return ptr; 
  }
  //!
  T& operator*() const 
  { 
    return *ptr; 
  }
  //!
  T* operator->() const
  { 
    return ptr; 
  }
  //!
  T* get() const 
  { 
    return ptr; 
  }
};


////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////

#endif  // __HEADER_GUARD__UTILS_PTRWRAPPER_H__