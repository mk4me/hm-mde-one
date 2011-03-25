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

class Model : public Channel, public utils::Observable<Model>
{
private:

    //! Wszystkie tagi wystepujace w timeline
    Tags allTags;

    //! Wszystkie tagi wystepujace w timeline
    ConstTags constAllTags;

    //! Zbior wszystkich zaznaczen w timeline
    Selections allSelections;

    //! Zbior wszystkich zaznaczen w timeline
    ConstSelections constAllSelections;

    //! Stan timeline
    State state;

public:

    //! Konstruktor zerujacy
    Model(const std::string & name = "DefaultModel");

    ~Model();

    //! \param mask Maska czasu realizowana wewnetrznie przez timeline, 0 <= mask <= length
    virtual void setMask(const Channel::Mask & mask);

    //! \param maskBegin Poczatek maski - 0 <= maskBegin <= maskEnd
    virtual void setMaskBegin(double maskBegin);

    //! \param maskEnd Koniec maski - maskBegin <= maskEnd <= length
    virtual void setMaskEnd(double maskEnd);

    //! propaguje zmiane na wszystkie aktywne podkanaly
    //! sprawdza maske i offset
    //! \param Aktualny czas timeline, 0 <= time <= length
    virtual void setTime(double time);

    //! modyfikuje skale
    //! \param Aktualna dlugosc timeline
    virtual void setLength(double length);

    //! modyfikuje length
    //! \param Aktualna skala czasu timeline, timeScale <> 0
    virtual void setLocalTimeScale(double timeScale);

    //! \param active Czy strumien jest aktywny podczas operacji oczasowych i odtwarzania timeline
    virtual void setActive(bool active); 

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
    void addTag(const ChannelConstPtr & channel, double time = 0, const std::string & name = "UnnamedTag");

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
    void addSelection(const ChannelConstPtr & channel, double begin, double end, const std::string & name = "UnnamedSelection");

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
    void removeChannel(const ChannelConstPtr & channel);

    //! \param path Sciezka kanalu do usuniecia
    void removeChannel(const std::string & path);

    //------------------- Operacje na kanalach -----------------------
    // ich zmiany sa propagowane w calym timeline a obserwatorzy beda powiadomieni

    //! \param channel Kanal do zmiany
    //! \param offset Offset kanalu
    void setChannelOffset(const ChannelConstPtr & channel, double offset);
    
    //! \param channel Kanal do zmiany
    //! \param length Czas trwania kanalu
    void setChannelLength(const ChannelConstPtr & channel, double length);

    //! \param channel Kanal do zmiany
    //! \param timeScale Skala czasu kanalu
    void setChannelTimeScale(const ChannelConstPtr & channel, double timeScale);

    //! \param channel Kanal do zmiany
    //! \param name Nazwa kanalu uniklalna wzgledem jego rodzica
    void setChannelName(const ChannelConstPtr & channel, const std::string & name);

    //! \param channel Kanal do zmiany
    //! \param mask Maska czasu dla kanalu, musi byc w zakresie 0 <= mask <= length
    void setChannelMask(const ChannelConstPtr & channel, const Channel::Mask & mask);

    //! \param channel Kanal do zmiany
    //! \param maskBegin Poczatek maski czasu dla kanalu, musi byc w zakresie 0 <= maskBegin <= maskEnd
    void setChannelMaskBegin(const ChannelConstPtr & channel, double maskBegin);

    //! \param channel Kanal do zmiany
    //! \param maskEnd Koniec maski czasu dla kanalu, musi byc w zakresie maskBegin <= maskEnd <= length
    void setChannelMaskEnd(const ChannelConstPtr & channel, double maskEnd);

    //! \param channel Kanal do zmiany
    //! \param active Czy kanal jest aktywny podczas operacji czasowych (odtwarzanie timeline)
    void setChannelActive(const ChannelConstPtr & channel, bool active);

    //! \param channel Kanal do zmiany
    void clearChannelTags(const ChannelConstPtr & channel);

    //! \param channel Kanal do zmiany
    void clearChannelSelections(const ChannelConstPtr & channel);

    //! \param channel Kanal do podzialu
    //! \param time Czas kanalu w ktorym dokona sie podzial kanalu - 0 < time < length
    //! \param nameA Nazwa mlodszej czesci kanalu po podziale, musi byc unikalna wzgledem rodzica kanalu
    //! \param nameB Nazwa starszej czesci kanalu po podziale, musi byc unikalna wzgledem rodzica kanalu
    void splitChannel(const ChannelConstPtr & channel, double time, const std::string & nameA = "A", const std::string & nameB = "B");
    
    //! oba kanalu musza byc w obrebie jednego rodzica
    //! \param channelA Pierwszy kanal do laczenia
    //! \param channelB Drugi kanal do laczenia
    //! \param name Nazwa kanalu po polaczeniu
    void mergeChannels(const ChannelConstPtr & channelA, const ChannelConstPtr & channelB, const std::string & name = "Merged");

    
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
    static ChannelConstPtr getChannel(const NamedTreeBaseConstPtr & channel);

private:

    //! \param path Sciezka do kanalu wzgledem roota
    //! \return Kanal o zadanej sciezce
    ChannelPtr getChannel(const std::string & path);

    //! sprawdza czy klient nie probuje modyfikowac kanalu z innego modelu
    //! \param channel Kanal podany przez klienta do weryfikacji
    bool verifyChannel(const ChannelConstPtr & channel) const;

    //! Konwertuje stale kanaly podawane przez klienta na kanaly ktore mozna modyfikowac
    //! sciaga modyfikatro const!!
    //! \param channel Kanal do konwersji na wersje do zapisu
    static ChannelPtr getWritableChannel(const ChannelConstPtr & channel);

    //! Konwertuje stale tagi podawane przez klienta na tagi ktore mozna modyfikowac
    //! sciaga modyfikatro const!!
    //! \param tag Tag do konwersji na wersje do zapisu
    static TagPtr getWritableTag(const TagConstPtr & tag);

    //! Konwertuje stale zaznaczenia podawane przez klienta na zaznaczenia ktore mozna modyfikowac
    //! sciaga modyfikatro const!!
    //! \param selection Zaznaczenie do konwersji na wersje do zapisu
    static SelectionPtr getWritableSelection(const SelectionConstPtr & selection);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD__TIMELINEMODEL_H__