#ifndef HEADER_GUARD__TIMELINECONTROLLER_H__
#define HEADER_GUARD__TIMELINECONTROLLER_H__

#include <timelinelib/Types.h>
#include <timelinelib/Model.h>
#include <timelinelib/State.h>
#include <utils/ObserverPattern.h>

#include <OpenThreads/Thread>
#include <OpenThreads/Mutex>

////////////////////////////////////////////////////////////////////////////////
namespace timeline{
////////////////////////////////////////////////////////////////////////////////

/** Klasa kontrolera odpowiedzialnego za zarzadzanie modelem danych.
* Wzorzec projektowy MVC - Model - View - Controller, gdzie widok, operacje wejscia i dane sa rozdzielone.
*/
class Controller : public OpenThreads::Thread //,
    //public utils::Observable<State>
{
private:

    //! Model ktorym zarzadzamy
    ModelPtr model;

    //! Model ktorym zarzadzamy
    ModelConstPtr constModel;

    //! Kopia stanu modelu
    State dirtyState;

    //! Stan zajetosci kontrolera
    volatile bool busy;

    //! Czy stan sie zmienil ale nie zostal zaakceptoany
    bool dirty;

    //! Opis stanu czasu
    bool timeDirty;

    //! Czy kontroler dziala w trybie asynchronicznym
    bool asynchronous;

    //! Muteks blokujace modyfikacje stanu modelu
    OpenThreads::Mutex modelMutex;

    //! Muteks blokujace modyfikacje stanu
    OpenThreads::Mutex stateMutex;

public:
    //! Konstruktor zerujacy
    Controller();

    ~Controller(void);

    //------- Zarzadzanie kontrolerem ---------------------

    //! Rozpoczyna odtwarzanie
    void play();

    //! Wstrzymuje odtwarzanie
    void pause();

    //! Zwraca czas odtwarzania
    double getTime() const;

    //! \param time Czas dla kanalow
    void setTime(double time);

    //! \return Aktualny czas timeline w formacie znormalizowanym [0;1]
    double getNormalizedTime() const;

    //! \param normalizedTime Czas timeline w formacie znormalizowanym
    void setNormalizedTime(double normalizedTime);

    //! \return Model danych zwiazany z tym kontrolerem
    const ModelPtr & getModel();

    //! \return Model danych zwiazany z tym kontrolerem
    const ModelConstPtr & getModel() const;


    //------- Stan kontrolera ---------------------

    //! \return Czy kontroler jest zajety
    bool isBusy() const;

    //! \return Czy stan jest "brudny" - zmieniony ale nie zatwierdzony/zaakceptowany
    bool isDirty() const;

    //! \return Czy timeline jest odtwarzany
    bool isPlaying() const;

    //! \return Czy czas sie zmienil ale nie zostal zatwierdzony/zaakceptowany
    bool isTimeDirty() const;

    //! \return Czy timeline dziala asynchronicznie
    bool isAsynchronous() const;


    //------- Akcje kontrolera ---------------------

    bool compute();

private:

    //! Watek odtwarzajacy timeline
    virtual void run();

    //! \return Czy mozna wykonac czysty zapis stanu
    bool isWriteEnable() const;

    //! \return Aktualny stan modelu lub jego brudna kopie
    const State & getState() const;

    //! \param state Nowy stan modelu
    void setState(const State & state);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD__TIMELINECONTROLLER_H__