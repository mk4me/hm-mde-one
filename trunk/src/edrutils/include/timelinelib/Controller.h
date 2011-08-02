/********************************************************************
    created:  2011/07/19
    created:  19:7:2011   14:17
    filename: Controller.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_TIMELINE__CONTROLLER_H__
#define HEADER_GUARD_TIMELINE__CONTROLLER_H__

#include <OpenThreads/Thread>
#include <OpenThreads/Mutex>
#include <OpenThreads/ScopedLock>

#include <timelinelib/Types.h>
#include <timelinelib/View.h>
#include <timelinelib/Model.h>

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <utils/ObserverPattern.h>

namespace timeline{

class IController : public utils::Observable<State>
{
public:

    //! Spos�b aktualizacji czasu
    enum TimeUpdateMode {
        AsynchTimeUpdate, //! Asynchroniczna aktualizacja czasu - przyjscie nowego czasu przerywa poprzedni� aktualizacj� je�li taka by�a i rozpoczyna aktualizacj� od nowa
        SynchTimeUpdate     //! Czeka a� sko�czy si� poprzenia aktualizacja czasu, ustawia nowy czas do kolejnej aktualizacji
    };

    //! Kierunek odtwarzania timeline
    enum PlaybackDirection {
        PlayForward     = 1,    // odtwarzanie do przodu
        PlayBackward    = -1    // odtwarzanie do ty�u
    };

    IController(State * state) : utils::Observable<State>(state) {}

    //! \return Aktualny model kontrolera
    virtual const ModelConstPtr & getModel() const = 0;

    //! \param view Widok kontrolera
    virtual void setView(const ViewPtr & view) = 0;
    //! \return Aktualny widok kontrolera
    virtual const ViewPtr & getView() = 0;
    //! \return Aktualny widok kontrolera
    virtual const ViewConstPtr & getView() const = 0;

    //------------------------------- Konfiguracja aktualizacji czasu -----------------------

    //! \param timeUpdateMode Spos�b aktualizacji czasu
    virtual void setTimeUpdateMode(TimeUpdateMode timeUpdateMode) = 0;

    //! \return Spos�b aktualizacji czasu
    virtual TimeUpdateMode getTimeUpdateMode() const = 0;

    //! \param playbackDirection Kierunek aktualizacji czasu w timeline
    virtual void setPlaybackDirection(PlaybackDirection playbackDirection) = 0;

    //! \return Kierunek aktualizacji czasu w timeline
    virtual PlaybackDirection getPlaybackDirection() const = 0;

//------------------------------- Zarzadzanie odtwarzaniem ------------------------------

    //! Uruchamia timeline - odtwarzanie
    virtual void play() = 0;

    //! Wstrzymuje odtwarzanie timeline
    virtual void pause() = 0;

    //! \return Czy timeline jest odtwarzany
    virtual bool isPlaying() const = 0;

    //! \return Czy stan timeline jest "brudny"
    virtual bool isDirty() const = 0;

    //! \return Czy czas timeline jest "brudny"
    virtual bool isTimeDirty() const = 0;

    //! \return Czy kontroler jest zaj�ty?
    virtual bool isBusy() const = 0;

    //------------------------------- Operacje na timeline -----------------------------------

    //! \return Dlugosc kanalu w sekundach, uwzgledniajac skale
    virtual double getLength() const = 0;

    //! \return Aktualny czas kanalu
    virtual double getTime() const = 0;

    //! \return Znormalizowany czas timeline
    virtual double getNormalizedTime() const = 0;

    //! \return Skala czasu kanalu
    virtual double getTimeScale() const = 0;

    //! \param timeScale Nowa skala czasowa
    virtual void setTimeScale(double timeScale) = 0;

    //! propaguje zmiane na wszystkie aktywne podkanaly
    //! sprawdza maske i offset
    //! \param time Aktualny czas timeline, 0 <= time <= length
    //! \param lockHolder Obiekt chcacy aktualizowac czas, trzymajacy blokade
    virtual void setTime(double time) = 0;

    //! propaguje zmiane na wszystkie aktywne podkanaly
    //! sprawdza maske i offset
    //! \param normTime Aktualny czas timeline, 0 <= time <= length
    //! \param lockHolder Obiekt chcacy aktualizowac czas, trzymajacy blokade
    virtual void setNormalizedTime(double normTime) = 0;

    //! \param path Sciezka nowego kanalu
    //! \param channel fatyczny kanal dotarczony przez klienta
    virtual void addChannel(const std::string & path, const IChannelPtr & channel = IChannelPtr()) = 0;

    //! \param path Sciezka kanalu do usuniecia
    virtual void removeChannel(const std::string & path) = 0;
};    

class Controller : public IController
{
protected:

    //! Typ lokalnego lockowania mutexa
    typedef OpenThreads::ScopedLock<OpenThreads::Mutex> ScopedLock;

private:

    typedef boost::function<double (Controller *, unsigned long int)> TimeGenerator;

    class Timer : public OpenThreads::Thread
    {
    public:
        Timer(unsigned long int delay)
            : delay(delay)
        {
            UTILS_ASSERT((delay != 0), "Z�y czas pomi�dzy tikami tegara");
        }

        virtual void run()
        {
            if(controller == nullptr){
                std::runtime_error("Wrong controller to update");
            }

            while(true){
                {
                    // czy nie ma pauzy
                    ScopedLock (controller->pauseMutex);
                }
                
                //aktualizujemy czas
                controller->setNextTime(delay);
                    
                //zasypiamy
                OpenThreads::Thread::microSleep(delay);
            }
        }

    void setController(Controller * controller)
    {
        UTILS_ASSERT((controller != nullptr), "B��dny kontroler do aktualizacji");
        this->controller = controller;
    }

    void setDelay(unsigned long int delay)
    {
        UTILS_ASSERT((delay > 0), "Bl�dne op�nienie zegara");
        this->delay = delay;
    }

    unsigned long int getDelay() const
    {
        return delay;
    }

    private:
        Controller * controller;
        unsigned long int delay;
    };

    friend class Timer;

    typedef boost::shared_ptr<Timer> TimerPtr;

private:
    //! Model logiczny timeline
    ModelPtr model;
    //! Model logiczny timeline w wersji const
    ModelConstPtr constModel;

    //! Widok timeline
    ViewPtr view;
    //! Widok timeline w wersji const
    ViewConstPtr constView;

    //! Wewn�trzny timer
    TimerPtr timer;

    //! Spos�b aktualizacji czasu
    TimeUpdateMode timeUpdateMode;

    //! Kierunek aktualizacji czasu
    PlaybackDirection playbackDirection;

    //! Mutex do synchronizacji edycji modelu (struktura kana��w)
    OpenThreads::Mutex modelMutex;
    
    //! Mutex do edycji odtwarzania timeline
    mutable OpenThreads::Mutex stateMutex;

    //! Mutex do zarzadzania odtwarzaniem timeline - wstrzymywania go
    OpenThreads::Mutex pauseMutex;

    //! Czy kontroler jest zaj�ty?
    volatile bool busy;

    //! Czy czas modelu i timeline s� r�ne
    bool dirty;

    //! "Brudny" stan timeline
    State dirtyState;

    //! Czy czas kontrolera i modelu s� r�zne
    bool timeDirty;

    //! Generator nowego czasu dla timera
    TimeGenerator timeGenerator;

public:

    Controller(const ViewPtr & view);
    virtual ~Controller();

    //------------------------------- Konfiguracja kontrolera ------------------------------

    //! \param model Model do ustawienia w kontrolerze
    virtual void setModel(const timeline::ModelPtr & model);
    //! \return Aktualny model kontrolera
    virtual const ModelPtr & getModel();
    //! \return Aktualny model kontrolera
    virtual const ModelConstPtr & getModel() const;

    //! \param view Widok kontrolera
    virtual void setView(const ViewPtr & view);
    //! \return Aktualny widok kontrolera
    virtual const ViewPtr & getView();
    //! \return Aktualny widok kontrolera
    virtual const ViewConstPtr & getView() const;

    //------------------------------- Konfiguracja aktualizacji czasu -----------------------

    //! \param timeUpdateMode Spos�b aktualizacji czasu
    virtual void setTimeUpdateMode(TimeUpdateMode timeUpdateMode);

    //! \return Spos�b aktualizacji czasu
    virtual TimeUpdateMode getTimeUpdateMode() const;

    //! \param playbackDirection Kierunek aktualizacji czasu w timeline
    virtual void setPlaybackDirection(PlaybackDirection playbackDirection);

    //! \return Kierunek aktualizacji czasu w timeline
    virtual PlaybackDirection getPlaybackDirection() const;

    //------------------------------- Zarzadzanie odtwarzaniem ------------------------------

    //! Uruchamia timeline - odtwarzanie
    virtual void play();

    //! Wstrzymuje odtwarzanie timeline
    virtual void pause();

    //! \return Czy timeline jest odtwarzany
    virtual bool isPlaying() const;

    //! \return Czy stan timeline jest "brudny"
    virtual bool isDirty() const;

    //! \return Czy czas timeline jest "brudny"
    virtual bool isTimeDirty() const;

    //! \return Czy kontroler jest zaj�ty?
    bool isBusy() const;

    //------------------------------- Operacje na timeline -----------------------------------

    //! \return Dlugosc kanalu w sekundach, uwzgledniajac skale
    virtual double getLength() const;

    //! \return Aktualny czas kanalu
    virtual double getTime() const;

    //! \return Znormalizowany czas timeline
    virtual double getNormalizedTime() const;

    //! \return Skala czasu kanalu
    virtual double getTimeScale() const;

    //! \param timeScale Nowa skala czasowa
    virtual void setTimeScale(double timeScale);

    //! propaguje zmiane na wszystkie aktywne podkanaly
    //! sprawdza maske i offset
    //! \param time Aktualny czas timeline, 0 <= time <= length
    //! \param lockHolder Obiekt chcacy aktualizowac czas, trzymajacy blokade
    virtual void setTime(double time);

    //! propaguje zmiane na wszystkie aktywne podkanaly
    //! sprawdza maske i offset
    //! \param normTime Aktualny czas timeline, 0 <= time <= length
    //! \param lockHolder Obiekt chcacy aktualizowac czas, trzymajacy blokade
    virtual void setNormalizedTime(double normTime);

    //! \param path Sciezka nowego kanalu
    //! \param channel fatyczny kanal dotarczony przez klienta
    virtual void addChannel(const std::string & path, const IChannelPtr & channel = IChannelPtr());

    //! \param path Sciezka kanalu do usuniecia
    virtual void removeChannel(const std::string & path);

private:

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

    //! \param controller Kontroler, kt�rego czas aktualizujemy do przodu
    //! \param realTimeDelta Rzeczywisty krok czasu w milisekundach
    //! \return nowy czas do ustawienia
    static double forwardTimeUpdate(Controller * controller, unsigned long int realTimeDelta);

    //! \param controller Kontroler, kt�rego czas aktualizujemy do ty�u
    //! \param realTimeDelta Rzeczywisty krok czasu w milisekundach
    //! \return nowy czas do ustawienia
    static double backwardTimeUpdate(Controller * controller, unsigned long int realTimeDelta);

    //! \param realTimeDelta Rzeczywisty krok czasu w milisekundach    
    //! \return Czy osiagni�to kraniec czasu
    bool setNextTime(unsigned long int realTimeDelta);
};

}

#endif  //  HEADER_GUARD_TIMELINE__CONTROLLER_H__

