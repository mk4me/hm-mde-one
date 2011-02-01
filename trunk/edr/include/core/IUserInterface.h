#ifndef IUSERINTERFACE_H
#define IUSERINTERFACE_H

#include <core/Window.h>

class IUserInterface
{
public:
    //!
    virtual ~IUserInterface() {}
    //! \return Wska�nik na g��wne okno.
    virtual core::Window* getMainWindow() = 0;
};

#endif //IUSERINTERFACE_H