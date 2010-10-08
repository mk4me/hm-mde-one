/********************************************************************
	created:  2010/05/13
	created:  13:5:2010   16:56
	filename: Controller.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__TIMELINECONTROLLER_H__
#define __HEADER_GUARD__TIMELINECONTROLLER_H__


#include <OpenThreads/Thread>
#include <osg/Timer>

#include "Model.h"

////////////////////////////////////////////////////////////////////////////////
namespace timeline {
////////////////////////////////////////////////////////////////////////////////

/**
 *	
 */
class Controller : protected OpenThreads::Thread
{
private:
    //! Model danych.
    Model * model;
    //!
    OpenThreads::Mutex timeMutex;
    //!
    OpenThreads::Mutex stateMutex;
    //!
    osg::Timer timer;
  
public:
    //!
    Controller();
    //!
    virtual ~Controller();

    //! Rozpoczyna odtwarzanie strumieni. 
    void play();
    //! Zatrzymuje strumienie.
    void pause();

    //! \return Model.
    inline const Model* getModel() const
    { 
        return model;
    }
    //! \return Model.
    inline Model* getModel()
    { 
        return model;
    }

    //! \return Czas
    double getTime() const;
    //! Ustawia czas blokuj¹co.
    //! \param time Czas
    void setTime(double time);
    //! Ustawia czas nieblokuj¹co. Do u¿ytku przez kod UI.
    //! \param time Czas.
    void setUITime(double time);
    //! \return D³ugoœæ najd³u¿szego ze strumieni.
    double getLength() const;

    //! \return
    inline double getTimeScale() const
    { 
        return model->getTimeScale();
    }
    //! \param timeScale
    inline void setTimeScale(double timeScale) 
    { 
        model->setTimeScale(timeScale);
    }
    //! \return Znormalizowany czas.
    inline double getNormalizedTime() const
    {
        return getTime() / getLength();
    }
    //! Ustawia czas blokuj¹co.
    //! \param time Znormalizowany czas.
    inline void setNormalizedTime(double time)
    {
        setTime( time * getLength() );
    }
    //! Ustawia czas nieblokuj¹co. Do u¿ytku przez kod UI.
    //! \param time Znormalizowany czas.
    inline void setNormalizedUITime(double time)
    {
        setUITime(time * getLength());
    }

    //!
    void setStreamOffset(int idx, double offset);

private:
    //! W¹tek odtwarzaj¹cy.
    virtual void run();

    //! Odtwarzanie strumieni z automatycznym postêpem.
    /** Odtwarzanie w ten sposób nigdy siê nie koñczy (ci¹gle zwraca false).
      \return false
    */
    bool autoPlayback();
    //! Odtwarzanie asynchroniczne.
    /** Porównuje, czy czas UI jest równy czasowi strumieni; jeœli tak, to siê
      koñczy. Metoda wprowadzona po to, aby zachowaæ p³ynnoœæ UI przy odtwarzaniu
      asynchronicznym.
      \return true gdy koniec.
    */  
    bool asyncPlayback();
};

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////

#endif  // __HEADER_GUARD__TIMELINECONTROLLER_H__