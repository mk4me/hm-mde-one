#ifndef HEADER_GUARD__TIMELINEMODEL_H__
#define HEADER_GUARD__TIMELINEMODEL_H__

#include <utils/NamedTree.h>
#include <utils/PtrPolicyBoost.h>
#include <timelinelib/Types.h>
#include <timelinelib/State.h>
#include <timelinelib/Channel.h>
#include <utils/ObserverPattern.h>
#include <boost/tokenizer.hpp>
#include <map>

////////////////////////////////////////////////////////////////////////////////
namespace timeline{
////////////////////////////////////////////////////////////////////////////////

class Model : public utils::Observable<Model>
{
public:
    
    //typedef NamedTree<Channel, utils::PtrPolicyBoost> TChannel;

    class TChannel;
    typedef boost::shared_ptr<TChannel> TChannelPtr;
    typedef boost::shared_ptr<const TChannel> TChannelConstPtr;

    typedef boost::weak_ptr<TChannel> TChannelWPtr;
    typedef boost::weak_ptr<const TChannel> TChannelConstWPtr;


    class TChannel : public NamedTree<Channel, utils::PtrPolicyBoost>
    {
        public:
            TChannel(const std::string & name = "UnnamedNode") : NamedTree<Channel, utils::PtrPolicyBoost>(name, Ptr(new Channel()))
            {

            }

            virtual void addChild(const std::string & path)
            {
                NamedTreeBase::addChild<TChannel>(path, -1);
            }

            void addChild(const std::string & path, NamedTreeBase::size_type idx)
            {
                NamedTreeBase::addChild<TChannel>(path, idx);
            }
    };
    
    
    typedef TChannel::const_iterator channel_const_iterator;
    typedef TChannel::size_type channel_size_type;

    typedef ConstTags::const_iterator tag_const_iterator;
    typedef ConstTags::size_type tag_size_type;

    typedef ConstSelections::const_iterator selection_const_iterator;
    typedef ConstSelections::size_type selection_size_type;

    typedef Channel::Mask Mask;

protected:

    typedef Tags::iterator tag_iterator;
    typedef Selections::iterator selection_iterator;
    typedef std::map<ChannelConstPtr, TChannelConstPtr> ChannelToTChannel;

private:

    //! Wszystkie tagi wystepujace w timeline
    Tags allTags;

    //! Wszystkie tagi wystepujace w timeline
    ConstTags constAllTags;

    //! Zbior wszystkich zaznaczen w timeline
    Selections allSelections;

    //! Zbior wszystkich zaznaczen w timeline
    ConstSelections constAllSelections;

    //! Root dla timeline
    TChannelPtr root;

    //! Root dla timeline
    TChannelConstPtr constRoot;

    //! Stan timeline
    State state;

    ChannelToTChannel channelToTChannel;

public:

    //! Konstruktor zerujacy
    Model(const std::string & name = "DefaultModel");

    ~Model();

    //! \return Wewnetrzna maska czasu kanalu
    const Mask & getMask() const;
    
    //! \return Poczatek maski
    double getMaskBegin() const;

    //! \return Koniec maski
    double getMaskEnd() const;

    //! \return Offset czasu - przesuniecie wzgledem poczatku rodzica
    double getLocalOffset() const;

    //! \return Offset czasu - przesuniecie wzgledem 0
    double getGlobalOffset() const;

    //! \return Dlugosc kanalu w milisekundach, uwzgledniajac skale
    double getLength() const;

    //! \return Aktualny czas kanalu
    double getTime() const;

    //! \return Skala czasu kanalu wzgledem skali rodzica
    double getLocalTimeScale() const;

    //! \return Skala czasu kanalu
    double getGlobalTimeScale() const;

    //! maska dla podkanalow
    //! \return Czy kanal jest aktywny podczas odtwarzania timeline i operacji czasowych
    bool isActive() const;

    //! \param mask Maska czasu realizowana wewnetrznie przez timeline, 0 <= mask <= length
    void setMask(const Channel::Mask & mask);

    //! \param maskBegin Poczatek maski - 0 <= maskBegin <= maskEnd
    void setMaskBegin(double maskBegin);

    //! \param maskEnd Koniec maski - maskBegin <= maskEnd <= length
    void setMaskEnd(double maskEnd);

    //! propaguje zmiane na wszystkie aktywne podkanaly
    //! sprawdza maske i offset
    //! \param Aktualny czas timeline, 0 <= time <= length
    void setTime(double time);

    //! modyfikuje skale
    //! \param Aktualna dlugosc timeline
    //virtual void setLength(double length);

    //! \param offset Czas wzgledem poczatku czasu rodzica
    void setLocalOffset(double offset);

    //! \param offset Czas wzgledem 0
    void setGlobalOffset(double offset);

    //! modyfikuje length
    //! \param Aktualna skala czasu timeline, timeScale <> 0
    void setLocalTimeScale(double timeScale);

    //! \param scale Skala kanalu, bezwzgledna wartosc skali
    void setGlobalTimeScale(double scale);

    //! \param active Czy strumien jest aktywny podczas operacji oczasowych i odtwarzania timeline
    void setActive(bool active); 

    //! \return czy timeline sie odtwarza
    bool isPlaying() const;

    //! \param czy odtwarzac timeline
    void setPlaying(bool play);

    //! \return zwraca stan timeline
    const State & getState() const;

    //! \param nowy stan timeline
    void setState(const State & state);

    //! \param channel Kanal ktoremu dodaje sie tag
    //! \param time Czas wystapienia taga
    //! \param name Nazwa taga unikala w obrebie kanalu
    void addTag(const TChannelConstPtr & channel, double time = 0, const std::string & name = "UnnamedTag");

    //! \param tag Tag do usuniecia
    void removeTag(const TagConstPtr & tag);

    //! \return Pierwszy tag wsrod wszystkich obecnych w timeline
    tag_const_iterator beginAllTags() const;

    //! \return Koniec tagow wsrod wszystkich obecnych w timeline
    tag_const_iterator endAllTags() const;

    //! \param idx Indeks tagu wsrod wqszystkich tagow w timeline
    //! \return Tag o zadanym indeksie wsrod wszystkich obecnych w timeline
    const TagConstPtr & getAllTags(tag_size_type idx) const;

    //! \return Ilosc wszystkich tagow w timeline
    tag_size_type sizeAllTags() const;

    //! \param channel Kanal ktoremu dodaje sie zaznaczenie
    //! \param begin Czas poczatku zaznaczenia kanalu - 0 <= begin <= end
    //! \param end Czas konca zaznaczenia kanalu - begin <= end <= length
    //! \param name Nazwa zaznaczenia unikala w obrebie kanalu
    void addSelection(const TChannelConstPtr & channel, double begin, double end, const std::string & name = "UnnamedSelection");

    //! \param Zaznaczenie do usunuiecia
    void removeSelection(const SelectionConstPtr & selection);

    //! \return Zwraca pierwsze zaznaczenie wsrod wszystkich zaznaczen w timeline
    selection_const_iterator beginAllSelections() const;

    //! \return Zwraca koniec wsrod wszystkich zaznaczen w timeline
    selection_const_iterator endAllSelections() const;

    //! \param idx Indeks zaznaczenia wsrod wszystkich zaznaczen w timeline
    //! \return Zaznaczenie
    const SelectionConstPtr & getAllSelection(selection_size_type idx) const;

    //! \return Ilosc wszystkich zaznaczen w timeline
    selection_size_type sizeAllSelections() const;

    //! \param path sciezka nowego kanalu
    //! \param channel fatyczny kanal dotarczony przez klienta
    void addChannel(const std::string & path, const IChannelPtr & channel = IChannelPtr());

    //! \param channel Kanal do usuniecia
    void removeChannel(const TChannelConstPtr & channel);

    //! \param path Sciezka kanalu do usuniecia
    void removeChannel(const std::string & path);

    //! \return pierwszy kanal timeline
    channel_const_iterator beginChannels() const;
    
    //! \return koniec kanalow timeline
    channel_const_iterator endChannels() const;

    //! \param idx Indeks kanalu w timeline
    //! \return Kanal w timeline o zadanym indeksie
    TChannelConstPtr getChannel(channel_size_type idx) const;

    //! \param path Sciezka do kanalu wzgledem roota
    //! \return Kanal o zadanej sciezce
    TChannelConstPtr getChannel(const std::string & path) const;

    //! \return Ilosc kanalow dzieci w timeline
    channel_size_type sizeChannels() const;

    //! \param channel Kanal wynikajacy z elementow jak tag czy selection
    //! \return Kanal timeline opakowujacy zadany kanal logiczny
    TChannelConstPtr getTChannelForChannel(const ChannelConstPtr & Channel) const;


    //------------------- Operacje na kanalach -----------------------
    // ich zmiany sa propagowane w calym timeline a obserwatorzy beda powiadomieni

    //! \param channel Kanal do zmiany
    //! \param offset Offset kanalu wzgledem rodzica
    void setChannelLocalOffset(const TChannelConstPtr & channel, double offset);

    //! \param channel Kanal do zmiany
    //! \param offset Offset kanalu wzgledem 0
    void setChannelGlobalOffset(const TChannelConstPtr & channel, double offset);
   
    //! \param channel Kanal do zmiany
    //! \param timeScale Skala czasu kanalu, lokalna wzgledem rodzica
    void setChannelLocalTimeScale(const TChannelConstPtr & channel, double timeScale);

    //! \param channel Kanal do zmiany
    //! \param timeScale Skala czasu kanalu, globalna
    void setChannelGlobalTimeScale(const TChannelConstPtr & channel, double timeScale);

    //! \param channel Kanal do zmiany
    //! \param name Nazwa kanalu uniklalna wzgledem jego rodzica
    void setChannelName(const TChannelConstPtr & channel, const std::string & name);

    //! \param channel Kanal do zmiany
    //! \param mask Maska czasu dla kanalu, musi byc w zakresie 0 <= mask <= length
    void setChannelMask(const TChannelConstPtr & channel, const Channel::Mask & mask);

    //! \param channel Kanal do zmiany
    //! \param maskBegin Poczatek maski czasu dla kanalu, musi byc w zakresie 0 <= maskBegin <= maskEnd
    void setChannelMaskBegin(const TChannelConstPtr & channel, double maskBegin);

    //! \param channel Kanal do zmiany
    //! \param maskEnd Koniec maski czasu dla kanalu, musi byc w zakresie maskBegin <= maskEnd <= length
    void setChannelMaskEnd(const TChannelConstPtr & channel, double maskEnd);

    //! \param channel Kanal do zmiany
    //! \param active Czy kanal jest aktywny podczas operacji czasowych (odtwarzanie timeline)
    void setChannelActive(const TChannelConstPtr & channel, bool active);

    //! \param channel Kanal do zmiany
    void clearChannelTags(const TChannelConstPtr & channel);

    //! \param channel Kanal do zmiany
    void clearChannelSelections(const TChannelConstPtr & channel);

    //! \param channel Kanal do podzialu
    //! \param time Czas kanalu w ktorym dokona sie podzial kanalu - 0 < time < length
    //! \param nameA Nazwa mlodszej czesci kanalu po podziale, musi byc unikalna wzgledem rodzica kanalu
    //! \param nameB Nazwa starszej czesci kanalu po podziale, musi byc unikalna wzgledem rodzica kanalu
    void splitChannel(const TChannelConstPtr & channel, double time, const std::string & nameA = "A", const std::string & nameB = "B");
    
    //! oba kanalu musza byc w obrebie jednego rodzica
    //! \param channelA Pierwszy kanal do laczenia
    //! \param channelB Drugi kanal do laczenia
    //! \param name Nazwa kanalu po polaczeniu
    void mergeChannels(const TChannelConstPtr & channelA, const TChannelConstPtr & channelB, const std::string & name = "Merged");

    
    //--------------------------- Zarzadzanie tagami ------------------------------------------------

    //! \param tag Tag ktoremu zmieniamy nazwe
    //! \param name Nazwa tagu
    void setTagName(const TagConstPtr & tag, const std::string & name);

    //! \param tag Tag ktoremu zmieniamy czas
    //! \param time Czas wystapienia tagu w kanale - 0 <= time <= length
    void setTagTime(const TagConstPtr & tag, double time);


    //--------------------------- Zarzadzanie zaznaczeniami ------------------------------------------------

    //! \param selection Zaznaczenie ktoremu zmieniamy nazwe
    //! \param name Nazwa zaznaczenia
    void setSelectionName(const SelectionConstPtr & selection, const std::string & name);

    //! \param selection Zaznaczenie ktoremu zmieniamy czas poczatku
    //! \param beginTime Czas poczatku zaznaczenia - 0 <= beginTime <= endTime
    void setSelectionBegin(const SelectionConstPtr & selection, double beginTime);

    //! \param selection Zaznaczenie ktoremu zmieniamy czas poczatku
    //! \param endTime Czas konca zaznaczenia - beginTime <= endTime <= length
    void setSelectionEnd(const SelectionConstPtr & selection, double endTime);

    //! \param selection Zaznaczenie ktore edytujemy
    ////! \param dTime Przesuniecie zaznaczenia takie ze begin + dTime >= 0 && end + dTime <= length
    void shiftSelection(const SelectionConstPtr & selection, double dTime);

    //-------------------- Pomocnicze funkcje ----------------------------

    //! \param channel Reprezentacja wezla w postaci NamedTreeBase
    //! \return Wskaznik na Channel
    static TChannelConstPtr toChannel(const NamedTreeBaseConstPtr & channel);

private:

    //! \param path Sciezka do kanalu wzgledem roota
    //! \return Kanal o zadanej sciezce
    TChannelPtr findChannel(const std::string & path);

    //! sprawdza czy klient nie probuje modyfikowac kanalu z innego modelu
    //! \param channel Kanal podany przez klienta do weryfikacji
    bool verifyChannel(const TChannelConstPtr & channel) const;

    //! sprawdza czy klient nie probuje modyfikowac kanalu z innego modelu
    //! \param channel Kanal podany przez klienta do weryfikacji
    bool verifyChannel(const ChannelConstPtr & channel) const;

    //! Konwertuje stale kanaly podawane przez klienta na kanaly ktore mozna modyfikowac
    //! sciaga modyfikatro const!!
    //! \param channel Kanal do konwersji na wersje do zapisu
    static TChannelPtr getWritableTChannel(const TChannelConstPtr & channel);

    //! Konwertuje stale kanaly podawane przez klienta na kanaly ktore mozna modyfikowac
    //! sciaga modyfikatro const!!
    //! \param channel Kanal do konwersji na wersje do zapisu
    static TChannelPtr getWritableTChannel(const NamedTreeBaseConstPtr & channel);

    //! Konwertuje stale kanaly podawane przez klienta na kanaly ktore mozna modyfikowac
    //! sciaga modyfikatro const!!
    //! \param channel Kanal do konwersji na wersje do zapisu
    static ChannelPtr getWritableChannel(const ChannelConstPtr & channel);

    //! Konwertuje stale kanaly podawane przez klienta na kanaly ktore mozna modyfikowac
    //! sciaga modyfikatro const!!
    //! \param channel Kanal do konwersji na wersje do zapisu
    static ChannelPtr getWritableChannel(const TChannelConstPtr & channel);


    //! Konwertuje stale tagi podawane przez klienta na tagi ktore mozna modyfikowac
    //! sciaga modyfikatro const!!
    //! \param tag Tag do konwersji na wersje do zapisu
    static TagPtr getWritableTag(const TagConstPtr & tag);

    //! Konwertuje stale zaznaczenia podawane przez klienta na zaznaczenia ktore mozna modyfikowac
    //! sciaga modyfikatro const!!
    //! \param selection Zaznaczenie do konwersji na wersje do zapisu
    static SelectionPtr getWritableSelection(const SelectionConstPtr & selection);

    //! \param ratio Wzgledna zmiana skali do zastosowania na skalach dzieci
    void updateChildrenScale(const TChannelConstPtr & child, double ratio);

    //! \param child Dziecko ktorego dlugosc ulegla zmianie
    static void updateParentLength(const TChannelConstPtr & parent, const TChannelConstPtr & child);

    //! \param dOffset Przyrost offsetu globalnego dzieci
    static void updateChildrenOffset(const TChannelConstPtr & child, double dOffset);

    static void updateParentOffset(const TChannelConstPtr & parent, const Model::TChannelConstPtr & child);

    //! \param ratio Zamiana skali wplywajaca na pozycje Tagow, zaznaczen i masek
    static void updateForScaleRatio(const ChannelPtr & channel, double ratio);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD__TIMELINEMODEL_H__