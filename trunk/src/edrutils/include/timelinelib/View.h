#include <utils/ObserverPattern.h>
#include <timelinelib/Controller.h>

namespace timeline {

/**
 * Klasa bazowa dla widoku timeline.
 */
class View : public utils::Observer<Model>
{
public:
    //! Typ wska�nika na kontroler.
    typedef boost::shared_ptr<Controller> ControllerPtr;

protected:
    //! Kontroler timeline.
    ControllerPtr controller;

public:
    //! Konstruktor zeruj�cy.
    View()
    {}
    //! Destruktor.
    virtual ~View()
    {}

    //! \return U�ywany kontroler.
    inline ControllerPtr getController()
    { 
        return controller;
    }
    //! \return U�ywany kontroler.
    inline const ControllerPtr getController() const
    { 
        return controller;
    }
    //! \param controller U�ywany kontroler.
    inline void setController(ControllerPtr controller) 
    { 
        this->controller = ControllerPtr(controller);
    }
};

} // namespace timeline