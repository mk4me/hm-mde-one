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

namespace coreUI {

class CoreConsoleWidget;

}

namespace core {

//! Obiekt logu. Powinien być tworzony w głównej binarce na samym początku maina.
class LogInitializer
{
public:
    //! \param configPath Ścieżka do pliku konfiguracyjnego.
    LogInitializer(const Filesystem::Path & configFile);
    //!
    ~LogInitializer();

public:
    //! \param console Widget konsoli.
    //void setConsoleWidget(ConsoleWidget* widget);
    void setConsoleWidget(coreUI::CoreConsoleWidget* widget);
};

}

#endif  // HEADER_GUARD_CORE__LOGINITIALIZER_H__111