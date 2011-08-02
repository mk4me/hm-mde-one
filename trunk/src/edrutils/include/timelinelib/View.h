/********************************************************************
    created:  2011/07/19
    created:  19:7:2011   14:18
    filename: View.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_TIMELINE__VIEW_H__
#define HEADER_GUARD_TIMELINE__VIEW_H__

#include <utils/Debug.h>

namespace timeline{

class IController;

class View
{
public:
    //! Aktualizuje w widoku wizualn� reprezentacj� stryktury kana��w ( po dodaniu, usuni�ciu kana�u )
    virtual void updateViewChannelsStructure() = 0;

    //! Aktualizuje w widoku wizualn� reprezentacj� tag�w ( po dodaniu, usuni�ciu, edycji czasu )
    virtual void updateViewTagsStructure() = 0;

    //! Od�wie�a ca�y widok
    virtual void refreshAll() = 0;

    //! \return Kontroler z kt�rym wsp�pracuje widok
    IController * getController()
    {
        return controller;
    }

    //! \return Kontroler z kt�rym wsp�pracuje widok
    const IController * getController() const
    {
        return controller;
    }

    //! \param controller Kontroler z kt�rym wsp�pracuje widok
    void setController(IController * controller)
    {
        UTILS_ASSERT((controller != nullptr), "B��dny kontroler");
        this->controller = controller;
    }

private:
    //! Kontroler z kt�ym wsp�pracuje widok
    IController * controller;
};

}

#endif  //  HEADER_GUARD_TIMELINE__VIEW_H__