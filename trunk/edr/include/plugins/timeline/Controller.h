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
#include "State.h"

////////////////////////////////////////////////////////////////////////////////
namespace timeline {
////////////////////////////////////////////////////////////////////////////////

/**
 *	
 */
class Controller : public OpenThreads::Thread, public utils::Observable<State>
{
private:
    //! Model danych.
    Model* model;

    //! Kopia stanu modelu.
    State dirtyState;

    //! Czy kontroler jest zaj�ty?
    volatile bool busy;
    //! Czy stan uleg� zmianie, a nie zosta� jeszcze zaakceptowany?
    bool dirty;
    //! Wyodr�bnienie opisu stanu czasu.
    bool timeDirty;
    //! Czy dzia�a w trybie asynchronicznym?
    bool asynchronous;

    //! Muteks blokujacy modyfikacj� stanu modelu.
    OpenThreads::Mutex modelMutex;
    //! Muteks blokuj�cy modyfikacj� stanu.
    OpenThreads::Mutex stateMutex;
    //! Muteks pauzujacy kontroler
    OpenThreads::Mutex pauseMutex;

    //! Czy niszczymy kontroler i konczymy watek?
    bool killThread;

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
    double getNormalizedTime() const;
    //! Ustawia czas blokuj�co.
    //! \param time Znormalizowany czas.
    void setNormalizedTime(double time);

    //! \return
    inline Model* getModel()
    { 
        return model;
    }

    //! \param stream Strumie� do dodania.
    void addStream(StreamPtr stream);
    //! \param stream Strumie� do usni�cia.
    void removeStream(StreamPtr stream);

    //! \return Czy kontroler jest zaj�ty?
    bool isBusy() const;
    //! \return Czy bie��cy stan jest "brudny"?
    bool isDirty() const;
    //! \return Czy strumienie s� odtwarzane?
    bool isPlaying() const;
    //! \return Czy bie��cy czas jest "brudny"? true -> isDirty()==true
    bool isTimeDirty() const;
    //! \return Czy dzia�a w trybie asynchronicznym?
    bool isAsynchronous() const;
    //! \param asynchronous Czy ma dzia�a� w trybie asynchronicznym?
    void setAsynchronous(bool asynchronous);

    bool compute();

private:
    //! W�tek odtwarzaj�cy.
    virtual void run();

    
    //! \return Czy mo�liwy jest "czysty" zapis?
    bool isWriteEnabled() const;

    //! \return Bie��cy stan. W zale�no�ci od rezultatu isWriteEnabled
    //! zwracany jest albo faktyczny stan modelu, albo jego "brudna" kopia.
    State getState() const;

    //! Ustawia bie��cy stan. Zmienna dirtyState zawsze jest ustawiana,
    //! ewentualnie jeszcze (na podstawie rezultatu isWriteEnabled) 
    //! ustawiana jest flaga dirty.
    //! \param state Bie��cy stan.
    void setState(const State& state);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////

#endif  // __HEADER_GUARD__TIMELINECONTROLLER_H__