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

    //! Czy kontroler jest zajêty?
    volatile bool busy;
    //! Czy stan uleg³ zmianie, a nie zosta³ jeszcze zaakceptowany?
    bool dirty;
    //! Czy czas ma byæ inkrementowany?
    bool timeRequested;
    //! Czy dzia³a w trybie asynchronicznym?
    bool asynchronousMode;


    //! Muteks blokujacy modyfikacjê stanu modelu.
    OpenThreads::Mutex modelMutex;
    //! Muteks blokuj¹cy modyfikacjê stanu.
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
    double getNormalizedTime() const
    {
        return getTime() / getLength();
    }
    //! Ustawia czas blokuj¹co.
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

    //! \return Czy kontroler jest zajêty?
    bool isBusy() const;
    //! \return Czy bie¿¹cy stan jest "brudny"?
    bool isDirty() const;
    //! \return Czy strumienie s¹ odtwarzane?
    bool isPlaying() const;
    //! \return
    bool isTimeRequested() const;

private:
    //! W¹tek odtwarzaj¹cy.
    virtual void run();

    //! \return Czy mo¿liwy jest "czysty" zapis?
    bool isWriteEnabled() const;

    //! \return Bie¿¹cy stan. W zale¿noœci od rezultatu isWriteEnabled
    //! zwracany jest albo faktyczny stan modelu, albo jego "brudna" kopia.
    Model::State getState() const;

    //! Ustawia bie¿¹cy stan. Zmienna dirtyState zawsze jest ustawiana,
    //! ewentualnie jeszcze (na podstawie rezultatu isWriteEnabled) 
    //! ustawiana jest flaga dirty.
    //! \param state Bie¿¹cy stan.
    void setState(const Model::State& state);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////

#endif  // __HEADER_GUARD__TIMELINECONTROLLER_H__