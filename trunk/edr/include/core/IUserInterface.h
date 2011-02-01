#ifndef IUSERINTERFACE_H
#define IUSERINTERFACE_H

#include <core/Window.h>

class IUserInterface
{
public:
    //!
    virtual ~IUserInterface() {}
    //! \return Wskaünik na g≥Ûwne okno.
    virtual core::Window* getMainWindow() = 0;
};

#endif //IUSERINTERFACE_H