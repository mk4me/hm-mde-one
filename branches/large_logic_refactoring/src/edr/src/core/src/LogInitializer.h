/********************************************************************
    created:  2011/04/12
    created:  12:4:2011   18:15
    filename: LogInitializer.h
    author:	  Piotr Gwiazdowski

    purpose:
*********************************************************************/
#ifndef HEADER_GUARD_CORE__LOGINITIALIZER_H__
#define HEADER_GUARD_CORE__LOGINITIALIZER_H__

#include <cstddef>
#include <core/ILog.h>
#include <core/Filesystem.h>

class EDRConsoleWidget;

//! Obiekt logu. Powinien być tworzony w głównej binarce na samym początku maina.
class LogInitializer
{
public:
    //! \param configPath Ścieżka do pliku konfiguracyjnego.
    LogInitializer(const core::Filesystem::Path & configFile);
    //!
    ~LogInitializer();

public:
    //! \param console Widget konsoli.
    //void setConsoleWidget(ConsoleWidget* widget);
    void setConsoleWidget(EDRConsoleWidget* widget);

private:
    /**
     *	Przeładowania operatorów new-delete. Prywatne, aby niemożliwe było
     *  tworzenie instancji typu na stercie.
     */
    void *operator new(std::size_t bytes);
    //! \see LogInitializer::operator new
    void *operator new[](std::size_t bytes);
    //! \see LogInitializer::operator new
    void operator delete(void* p);
    //! \see LogInitializer::operator new
    void operator delete[](void* p);
};

#endif  // HEADER_GUARD_CORE__LOGINITIALIZER_H__
