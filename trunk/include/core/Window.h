#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <string>
#include <map>
#include <utility>
#include <boost/function.hpp>
#include <core/SmartPtr.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

class Window
{
public:
    typedef boost::function2<void, const std::string&, bool> ItemPressed;
    typedef CORE_SHARED_PTR(ItemPressed) ItemPressedPtr;
    typedef CORE_WEAK_PTR(ItemPressed) ItemPressedWeakPtr;

private:
    //! Opcje menu.
    typedef std::map<std::string, ItemPressedWeakPtr> MenuItems;
    //! Opcje menu.
    MenuItems menuItems;

public:
    //! Dodaje opcj� do menu.
    void addMenuItem(const std::string& path, ItemPressedWeakPtr callback, bool checkable = false, bool initialState = false);
    //!
    void removeMenuItem(const std::string& path);
    
protected:
    //! Natywne dodanie opcji do menu.
    virtual void onAddMenuItem( const std::string& path, bool checkable, bool initalState ) = 0;
    //! Natywne usuni�cie opcji z menu.
    virtual void onRemoveMenuItem( const std::string& path ) = 0;

    //! Odpala callbacka.
    void triggerMenuItem( const std::string& path, bool state );
};

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif //MAINWINDOW_H