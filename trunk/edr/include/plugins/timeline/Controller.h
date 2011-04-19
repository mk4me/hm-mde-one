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

    //! Czy kontroler jest zajêty?
    volatile bool busy;
    //! Czy stan uleg³ zmianie, a nie zosta³ jeszcze zaakceptowany?
    bool dirty;
    //! Wyodrêbnienie opisu stanu czasu.
    bool timeDirty;
    //! Czy dzia³a w trybie asynchronicznym?
    bool asynchronous;

    //! Muteks blokujacy modyfikacjê stanu modelu.
    OpenThreads::Mutex modelMutex;
    //! Muteks blokuj¹cy modyfikacjê stanu.
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
    //! Ustawia czas blokuj¹co.
    //! \param time Czas
    void setTime(double time);
    //! \return
    double getTimeScale() const;
    //! \param timeScale
    void setTimeScale(double timeScale);
    //! \return D³ugoœæ najd³u¿szego ze strumieni.
    double getLength() const;
    //!
    void setStreamOffset(int idx, double offset);

    //! \return Znormalizowany czas.
    double getNormalizedTime() const;
    //! Ustawia czas blokuj¹co.
    //! \param time Znormalizowany czas.
    void setNormalizedTime(double time);

    //! \return
    inline Model* getModel()
    { 
        return model;
    }

    //! \param stream Strumieñ do dodania.
    void addStream(StreamPtr stream);
    //! \param stream Strumieñ do usniêcia.
    void removeStream(StreamPtr stream);

    //! \return Czy kontroler jest zajêty?
    bool isBusy() const;
    //! \return Czy bie¿¹cy stan jest "brudny"?
    bool isDirty() const;
    //! \return Czy strumienie s¹ odtwarzane?
    bool isPlaying() const;
    //! \return Czy bie¿¹cy czas jest "brudny"? true -> isDirty()==true
    bool isTimeDirty() const;
    //! \return Czy dzia³a w trybie asynchronicznym?
    bool isAsynchronous() const;
    //! \param asynchronous Czy ma dzia³aæ w trybie asynchronicznym?
    void setAsynchronous(bool asynchronous);

    bool compute();

private:
    //! W¹tek odtwarzaj¹cy.
    virtual void run();

    
    //! \return Czy mo¿liwy jest "czysty" zapis?
    bool isWriteEnabled() const;

    //! \return Bie¿¹cy stan. W zale¿noœci od rezultatu isWriteEnabled
    //! zwracany jest albo faktyczny stan modelu, albo jego "brudna" kopia.
    State getState() const;

    //! Ustawia bie¿¹cy stan. Zmienna dirtyState zawsze jest ustawiana,
    //! ewentualnie jeszcze (na podstawie rezultatu isWriteEnabled) 
    //! ustawiana jest flaga dirty.
    //! \param state Bie¿¹cy stan.
    void setState(const State& state);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////

#endif  // __HEADER_GUARD__TIMELINECONTROLLER_H__