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
class Controller : protected OpenThreads::Thread, public utils::Observable<Model::State>
{
private:
    //! Model danych.
    Model* model;

    //! Kopia stanu modelu.
    Model::State dirtyState;

    //! Czy kontroler jest zaj�ty?
    volatile bool busy;
    //! Czy stan uleg� zmianie, a nie zosta� jeszcze zaakceptowany?
    bool dirty;
    //! Czy czas ma by� inkrementowany?
    bool timeRequested;
    //! Czy dzia�a w trybie asynchronicznym?
    bool asynchronousMode;


    //! Muteks blokujacy modyfikacj� stanu modelu.
    OpenThreads::Mutex modelMutex;
    //! Muteks blokuj�cy modyfikacj� stanu.
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

    //! \return Czas
    double getTime() const;
    //! Ustawia czas blokuj�co.
    //! \param time Czas
    void setTime(double time);
    //! \return
    double getTimeScale() const;
    //! \param timeScale
    void setTimeScale(double timeScale);
    //! \return D�ugo�� najd�u�szego ze strumieni.
    double getLength() const;
    //!
    void setStreamOffset(int idx, double offset);

    //! \return Znormalizowany czas.
    double getNormalizedTime() const
    {
        return getTime() / getLength();
    }
    //! Ustawia czas blokuj�co.
    //! \param time Znormalizowany czas.
    void setNormalizedTime(double time)
    {
        setTime( time * getLength() );
    }

    //! \return
    inline Model* getModel()
    { 
        return model;
    }

    //! \return
    inline bool isAsynchronousMode() const
    { 
        return asynchronousMode;
    }
    //! \param asynchronousMode
    inline void setAsynchronousMode(bool asynchronousMode) 
    { 
        this->asynchronousMode = asynchronousMode; 
    }

    //! \return Czy kontroler jest zaj�ty?
    bool isBusy() const;
    //! \return Czy bie��cy stan jest "brudny"?
    bool isDirty() const;
    //! \return Czy strumienie s� odtwarzane?
    bool isPlaying() const;
    //! \return
    bool isTimeRequested() const;

private:
    //! W�tek odtwarzaj�cy.
    virtual void run();

    //! \return Czy mo�liwy jest "czysty" zapis?
    bool isWriteEnabled() const;

    //! \return Bie��cy stan. W zale�no�ci od rezultatu isWriteEnabled
    //! zwracany jest albo faktyczny stan modelu, albo jego "brudna" kopia.
    Model::State getState() const;

    //! Ustawia bie��cy stan. Zmienna dirtyState zawsze jest ustawiana,
    //! ewentualnie jeszcze (na podstawie rezultatu isWriteEnabled) 
    //! ustawiana jest flaga dirty.
    //! \param state Bie��cy stan.
    void setState(const Model::State& state);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////

#endif  // __HEADER_GUARD__TIMELINECONTROLLER_H__