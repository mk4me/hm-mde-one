//#ifndef HEADER_GUARD__TIMELINECONTROLLER_H__
//#define HEADER_GUARD__TIMELINECONTROLLER_H__
//
//#include <timeline/Types.h>
//#include <timeline/State.h>
//#include <utils/ObserverPattern.h>
//#include <OpenThreads/Thread>
//#include <OpenThreads/Mutex>
//
//////////////////////////////////////////////////////////////////////////////////
//namespace timeline{
//////////////////////////////////////////////////////////////////////////////////
//
///** Klasa kontrolera odpowiedzialnego za zarzadzanie modelem danych.
//* Wzorzec projektowy MVC - Model - View - Controller, gdzie widok, operacje wejscia i dane sa rozdzielone.
//*/
//class Controller :
//    public OpenThreads::Thread, public utils::Observable<State>
//{
//private:
//
//    //! model ktorzym zarzadzamy
//    ModelPtr model;
//
//    //! kopia stanu modelu
//    State dirtyState;
//
//    //! stan zajetosci kontrolera
//    volatile bool busy;
//
//    //! czy stan sie zmienil ale nie zostal zaakceptoany
//    bool dirty;
//
//    //! opis stanu czasu
//    bool timeDirty;
//
//    //! czy kontroler dziala w trybie asynchronicznym
//    bool asynchronous;
//
//    //! muteks blokujace modyfikacje stanu modelu
//    OpenThreads::Mutex modelMutex;
//
//    //! muteks blokujace modyfikacje stanu
//    OpenThreads::Mutex stateMutex;
//
//public:
//    //! konstruktor zerujacy
//    Controller();
//
//    ~Controller(void);
//
//    //------- Zarzadzanie kontrolerem ---------------------
//
//    //! rozpoczyna odtwarzanie strumieni
//    void play();
//
//    //! wstrzymuje odtwarzanie strumieni
//    void pause();
//
//    //! zwraca czas odtwarzania
//    double getTime() const;
//
//    //! \param nowy czas strumieni
//    void setTime(double time);
//
//    //! \return zwraca skale czasu timeline
//    double getTimeScale() const;
//
//    //! \param nowa skala czasu timeline
//    void setTimeScale(double timeScale);
//
//    //! \return zwraca sumaryczna dlugosc strumieni - od najwczesniejszego poczatku do najpozniejszego konca
//    double getLength() const;
//
//    //! \return zwraca aktualny czas timeline w formacie znormalizowanym [0;1]
//    double getNormalizedTime() const;
//
//    //! \param nowy czas timeline w formacie znormalizowanym
//    void setNormalizedTime(double normalizedTime);
//
//    //! \return zwraca model danych zwiazany z tym kontrolerem
//    ModelPtr getModel();
//
//
//    //------- Zarzadzanie strumieniami ---------------------
//
//    //! \param strumien danych uzytkownika do dodania
//    void addStream(IChannelPtr stream);
//
//    //! \param strumien danych uzytkownika do usuniecia
//    void removeStream(IChannelPtr stream);
//
//    //! \return zwraca czy strumien jest aktyny podczas odtwarzania timeline
//    void isStreamActive(IChannelPtr stream) const;
//
//    //! \param czy strumien jest aktywny podczas odtwarzania timeline
//    bool setStreamActive(IChannelPtr stream, bool active);
//
//
//    //------- Zarzadzanie tagami ---------------------
//
//    //! \param tag do dodania, musi miec poprawnie ustawiony strumien
//    void addTag(TagPtr tag);
//
//    //! \param tag do usuniecia
//    void removeTag(TagPtr tag);
//
//
//    //------- Zarzadzanie zaznaczeniami ---------------------
//
//    //! \param zaznaczenie do dodania, musi miec poprawnie ustawiony strumien i wartosci
//    void addSelection(SelectionPtr selection);
//
//    //! \param zaznaczenie do usuniecia
//    void removeSelection(SelectionPtr selection);
//
//
//    //------- Stan kontrolera ---------------------
//
//    //! \return czy kontroler jest zajety
//    bool isBusy() const;
//
//    //! \return czy stan jest "brudny" - zmieniony ale nie zatwierdzony/zaakceptowany
//    bool isDirty() const;
//
//    //! \return czy timeline jest odtwarzany
//    bool isPlaying() const;
//
//    //! \return czy czas sie zmienil ale nie zostal zatwierdzony/zaakceptowany
//    bool isTimeDirty() const;
//
//    //! \return czy timeline dziala asynchronicznie
//    bool isAsynchronous() const;
//
//
//    //------- Akcje kontrolera ---------------------
//
//    bool compute();
//
//private:
//
//    //! watek odtwarzajacy timeline
//    virtual void run();
//
//    //! \return czy mozna wykonac czysty zapis stanu
//    bool isWriteEnable() const;
//
//    //! \return aktualny stan modelu lub jego brudna kopie
//    const State & getState() const;
//
//    //! \param nowy stan modelu
//    void setState(const State & state);
//};
//
//////////////////////////////////////////////////////////////////////////////////
//} // namespace timeline
//////////////////////////////////////////////////////////////////////////////////
//
//#endif  // HEADER_GUARD__TIMELINECONTROLLER_H__