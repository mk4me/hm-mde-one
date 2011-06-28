#include <utils/ObserverPattern.h>
#include <timelinelib/Controller.h>

namespace timeline {

/**
 * Klasa bazowa dla widoku timeline.
 */
class View : public utils::Observer<Model>
{
public:
    //! Typ wskaünika na kontroler.
    typedef boost::shared_ptr<Controller> ControllerPtr;

protected:
    //! Kontroler timeline.
    ControllerPtr controller;

public:
    //! Konstruktor zerujπcy.
    View()
    {}
    //! Destruktor.
    virtual ~View()
    {}

    //! \return Uøywany kontroler.
    inline ControllerPtr getController()
    { 
        return controller;
    }
    //! \return Uøywany kontroler.
    inline const ControllerPtr getController() const
    { 
        return controller;
    }
    //! \param controller Uøywany kontroler.
    inline void setController(ControllerPtr controller) 
    { 
        this->controller = ControllerPtr(controller);
    }
};

} // namespace timeline