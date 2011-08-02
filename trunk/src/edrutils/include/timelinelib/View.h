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
    //! Aktualizuje w widoku wizualn¹ reprezentacjê stryktury kana³ów ( po dodaniu, usuniêciu kana³u )
    virtual void updateViewChannelsStructure() = 0;

    //! Aktualizuje w widoku wizualn¹ reprezentacjê tagów ( po dodaniu, usuniêciu, edycji czasu )
    virtual void updateViewTagsStructure() = 0;

    //! Odœwie¿a ca³y widok
    virtual void refreshAll() = 0;

    //! \return Kontroler z którym wspó³pracuje widok
    IController * getController()
    {
        return controller;
    }

    //! \return Kontroler z którym wspó³pracuje widok
    const IController * getController() const
    {
        return controller;
    }

    //! \param controller Kontroler z którym wspó³pracuje widok
    void setController(IController * controller)
    {
        UTILS_ASSERT((controller != nullptr), "B³êdny kontroler");
        this->controller = controller;
    }

private:
    //! Kontroler z któym wspó³pracuje widok
    IController * controller;
};

}

#endif  //  HEADER_GUARD_TIMELINE__VIEW_H__