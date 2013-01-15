#include "CorePCH.h"
#include <core/Window.h>
#include <core/PluginCommon.h>
#include <core/ILog.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

void Window::addMenuItem(const std::string& path, ItemPressedWeakPtr callback, bool checkable, bool initialState)
{
    if ( !menuItems.insert(std::make_pair(path, callback)).second ) {
        CORE_LOG_ERROR("Menu item " << path << " already exists.");
    } else {
        onAddMenuItem(path, checkable, initialState);
    }
}


void Window::removeMenuItem( const std::string& path )
{
    size_t erased = menuItems.erase(path);
    if ( erased != 0 ) {
        if ( erased > 1 ) {
            CORE_LOG_ERROR("Erased multiple items " << path);
        }
        onRemoveMenuItem(path);
    } else {
        CORE_LOG_ERROR("Item " << path << " does not exist.");
    }
}

void Window::triggerMenuItem( const std::string& path, bool state )
{
    MenuItems::iterator found = menuItems.find(path);
    if ( found != menuItems.end() ) {
        ItemPressedWeakPtr callback = found->second;
        if ( ItemPressedPtr locked = callback.lock() ) {
            (*locked)(path, state);
        } else {
            menuItems.erase(found);
            onRemoveMenuItem(path);
        }
    } else {
        CORE_LOG_ERROR("Menu item " << path << " does not exist.");
    }
}

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////
