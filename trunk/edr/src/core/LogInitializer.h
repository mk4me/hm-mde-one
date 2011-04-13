/********************************************************************
    created:  2011/04/12
    created:  12:4:2011   18:15
    filename: LogInitializer.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__LOGINITIALIZER_H__
#define HEADER_GUARD_CORE__LOGINITIALIZER_H__

#include <core/Log.h>

class ConsoleWidget;

//! Obiekt logu. Powinien by� tworzony w g��wnej binarce na samym pocz�tku maina.
class LogInitializer 
{
public:
    //! \param configPath �cie�ka do pliku konfiguracyjnego.
    LogInitializer(const char* configPath);
    //!
    ~LogInitializer();

public:
    //! \param console Widget konsoli.
    void setConsoleWidget(ConsoleWidget* widget);

private:
    /**
     *	Prze�adowania operator�w new-delete. Prywatne, aby niemo�liwe by�o
     *  tworzenie instancji typu na stercie.
     */     
    void *operator new(size_t bytes);
    //! \see LogInitializer::operator new
    void *operator new[](size_t bytes);
    //! \see LogInitializer::operator new
    void operator delete(void* p);
    //! \see LogInitializer::operator new
    void operator delete[](void* p);
};

#endif  // HEADER_GUARD_CORE__LOGINITIALIZER_H__