#ifndef HEADER_GUARD__TIMELINEMODEL_H__
#define HEADER_GUARD__TIMELINEMODEL_H__

#include <utils/NamedTree.h>
#include <utils/PtrPolicyBoost.h>
#include <timelinelib/Types.h>
#include <timelinelib/Channel.h>
#include <utils/ObserverPattern.h>
#include <boost/tokenizer.hpp>
#include <timelinelib/State.h>
#include <map>

////////////////////////////////////////////////////////////////////////////////
namespace timeline{
////////////////////////////////////////////////////////////////////////////////

class Model : public utils::Observable<Model>
{
public:

    //! Forward declartation
    class TChannel;
    typedef boost::shared_ptr<TChannel> TChannelPtr;
    typedef boost::shared_ptr<const TChannel> TChannelConstPtr;

    typedef boost::weak_ptr<TChannel> TChannelWPtr;
    typedef boost::weak_ptr<const TChannel> TChannelConstWPtr;

    //! Klasa wprowadzaj¹ca hierarchiê kana³ów, kolejnoœæ, obs³ugê œcie¿ek
    class TChannel : public NamedTree<Channel, utils::PtrPolicyBoost>
    {
        public:
            TChannel(const std::string & name = "UnnamedChannel") : NamedTree<Channel, utils::PtrPolicyBoost>(name, Ptr(new Channel()))
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

    typedef Channel::Mask Mask;

private:

    typedef Tags::iterator tag_iterator;
    
    //! Mapowanie Kana³ów do kana³ów z obs³uga œcie¿ek i hierarchi¹
    typedef std::map<ChannelConstPtr, TChannelConstPtr> ChannelToTChannel;

    typedef std::vector<ChannelConstPtr> ConstChannels;

    //! Typ œcie¿ki w formacie œcie¿ka/liœæ
    typedef std::pair<std::string, std::string> ExtPath;

private:

    //! Wszystkie tagi wystepujace w timeline
    Tags allTags;

    //! Wszystkie tagi wystepujace w timeline
    ConstTags constAllTags;

    //! Root dla timeline
    TChannelPtr root;

    //! Root dla timeline
    TChannelConstPtr constRoot;

    //! Mapowanie kanalow logicznych do kanalow w hierarchii
    ChannelToTChannel channelToTChannel;

    //! Aktualny stan modelu
    State state;

public:

    //! Konstruktor zerujacy
    Model(const std::string & name = "DefaultModel");

    ~Model();

    //! \return Stan modelu
    const State & getState() const;

    //! \param state Stan modelu
    void setState(const State & state);

    //! \return Wewnetrzna maska czasu kanalu
    const Mask & getMask() const;
    
    //! \return Poczatek maski
    double getMaskBegin() const;

    //! \return Koniec maski
    double getMaskEnd() const;

    //! \return Offset czasu - przesuniecie wzgledem 0
    double getOffset() const;

    //! \return Dlugosc kanalu w sekundach, uwzgledniajac skale
    double getLength() const;

    //! \return Aktualny czas kanalu
    double getTime() const;

    //! \return Znormalizowany czas timeline
    double getNormalizedTime() const;

    //! \return Skala czasu kanalu
    double getTimeScale() const;

    //! maska dla podkanalow
    //! \return Czy kanal jest aktywny podczas odtwarzania timeline i operacji czasowych
    bool isActive() const;

    //! \param mask Maska czasu realizowana wewnetrznie przez timeline, 0 <= mask <= length
    void setMask(const Mask & mask);

    //! \param maskBegin Poczatek maski - 0 <= maskBegin <= maskEnd
    void setMaskBegin(double maskBegin);

    //! \param maskEnd Koniec maski - maskBegin <= maskEnd <= length
    void setMaskEnd(double maskEnd);

    //! propaguje zmiane na wszystkie aktywne podkanaly
    //! sprawdza maske i offset
    //! \param time Aktualny czas timeline, 0 <= time <= length
    //! \param lockHolder Obiekt chcacy aktualizowac czas, trzymajacy blokade
    void setTime(double time);

    //! propaguje zmiane na wszystkie aktywne podkanaly
    //! sprawdza maske i offset
    //! \param normTime Aktualny czas timeline, 0 <= time <= length
    //! \param lockHolder Obiekt chcacy aktualizowac czas, trzymajacy blokade
    void setNormalizedTime(double normTime);

    //! modyfikuje skale
    //! \param Aktualna dlugosc timeline
    //virtual void setLength(double length);

    //! \param offset Czas wzgledem 0
    void setOffset(double offset);

    //! \param scale Skala kanalu, bezwzgledna wartosc skali
    void setTimeScale(double scale);

    //! \param active Czy strumien jest aktywny podczas operacji oczasowych i odtwarzania timeline
    void setActive(bool active); 

    //! Czysci wszystkie tagi w timeline
    void clearAllTags();

    //! Resetuje skale czasu dla wszystkich kanalow do ich globalnej wartosci rownej 1
    void resetAllScales();

    //! Resetuje offsety dla wszystkich kanalow do ich globalnej wartosci rownej 0
    void resetAllOffsets();

    //! Resetuje wszystkie offsety (0) i skale (1) w timeline
    void resetAllOffsetsAndScales();

    //! \param path Sciezka do taga w postaci sciezka do kanalu sciezki / nazwa taga
    //! \param time Czas wystapienia taga
    void addTag(const std::string & path, double begin = 0, double length = 0);

    //! \param path Sciezka do taga w postaci sciezka do kanalu sciezki / nazwa taga
    //! \return Tag
    const TagConstPtr & getTag(const std::string & path) const;

    //! \param path Sciezka do taga w postaci sciezka do kanalu sciezki / nazwa taga
    void removeTag(const std::string & path);

    //! \return Pierwszy tag wsrod wszystkich obecnych w timeline
    tag_const_iterator beginAllTags() const;

    //! \return Koniec tagow wsrod wszystkich obecnych w timeline
    tag_const_iterator endAllTags() const;

    //! \param idx Indeks tagu wsrod wqszystkich tagow w timeline
    //! \return Tag o zadanym indeksie wsrod wszystkich obecnych w timeline
    const TagConstPtr & getAllTags(tag_size_type idx) const;

    //! \return Ilosc wszystkich tagow w timeline
    tag_size_type sizeAllTags() const;

    //! \param path Sciezka nowego kanalu
    //! \param channel fatyczny kanal dotarczony przez klienta
    void addChannel(const std::string & path, const IChannelPtr & channel = IChannelPtr());

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

    //------------------- Operacje na kanalach -----------------------
    // ich zmiany sa propagowane w calym timeline a obserwatorzy beda powiadomieni

    //! \param path Sciezka do kanalu
    //! \param offset Offset kanalu wzgledem rodzica
    void setChannelLocalOffset(const std::string & path, double offset);

    //! \param path Sciezka do kanalu
    //! \param offset Offset kanalu wzgledem 0
    void setChannelGlobalOffset(const std::string & path, double offset);
   
    //! \param path Sciezka do kanalu
    //! \param timeScale Skala czasu kanalu, lokalna wzgledem rodzica
    void setChannelLocalTimeScale(const std::string & path, double timeScale);

    //! \param path Sciezka do kanalu
    //! \param timeScale Skala czasu kanalu, globalna
    void setChannelGlobalTimeScale(const std::string & path, double timeScale);

    //! \param path Sciezka do kanalu
    //! \param name Nazwa kanalu uniklalna wzgledem jego rodzica
    void setChannelName(const std::string & path, const std::string & name);

    //! \param path Sciezka do kanalu
    //! \param mask Maska czasu dla kanalu, musi byc w zakresie 0 <= mask <= length
    void setChannelMask(const std::string & path, const Channel::Mask & mask);

    //! \param path Sciezka do kanalu
    //! \param maskBegin Poczatek maski czasu dla kanalu, musi byc w zakresie 0 <= maskBegin <= maskEnd
    void setChannelMaskBegin(const std::string & path, double maskBegin);

    //! \param path Sciezka do kanalu
    //! \param maskEnd Koniec maski czasu dla kanalu, musi byc w zakresie maskBegin <= maskEnd <= length
    void setChannelMaskEnd(const std::string & path, double maskEnd);

    //! \param path Sciezka do kanalu
    //! \param active Czy kanal jest aktywny podczas operacji czasowych (odtwarzanie timeline)
    void setChannelActive(const std::string & path, bool active);

    //! \param path Sciezka do kanalu
    void clearChannelTags(const std::string & path, bool deeper = false);

    ////! \param path Sciezka do kanalu
    //void resetChannelOffsets(const std::string & path, bool deeper = false);

    ////! \param path Sciezka do kanalu
    //void resetChannelTimeScales(const std::string & path, bool deeper = false);

    ////! \param path Sciezka do kanalu
    //void resetChannelOffsetsAndTimeScales(const std::string & path, bool deeper = false);

    //! \param path Sciezka do kanalu
    //! \param time Czas kanalu w ktorym dokona sie podzial kanalu - 0 < time < length
    //! \param nameA Nazwa mlodszej czesci kanalu po podziale, musi byc unikalna wzgledem rodzica kanalu
    //! \param nameB Nazwa starszej czesci kanalu po podziale, musi byc unikalna wzgledem rodzica kanalu
    void splitChannel(const std::string & path, double time, const std::string & nameA = "A", const std::string & nameB = "B");
    
    //! oba kanalu musza byc w obrebie jednego rodzica
    //! \param pathA Pierwszy kanal do scalenia
    //! \param pathB Drugi kanal do scalenia
    //! \param name Nazwa kanalu po polaczeniu
    void mergeChannels(const std::string & pathA, const std::string & pathB, const std::string & name = "Merged");

    
    //--------------------------- Zarzadzanie tagami ------------------------------------------------

    //! \param path Sciezka do taga w postaci sciezka do kanalu sciezki / nazwa taga
    //! \param name Nazwa tagu
    void setTagName(const std::string & path, const std::string & name);

    //! \param path Sciezka do taga w postaci sciezka do kanalu sciezki / nazwa taga
    //! \param time Czas wystapienia tagu w kanale - 0 <= time <= length
    void setTagTime(const std::string & path, double time);

    //! \param path Sciezka do taga w postaci sciezka do kanalu sciezki / nazwa taga
    //! \param length Dlugosc trwania tagu tak aby begin >= 0 && begin + length <= channel->length
    void setTagLength(const std::string & path, double length);

    //! \param path Sciezka do taga w postaci sciezka do kanalu sciezki / nazwa taga
    //! \param dTime Czas przesuniecia taga w kanale tak aby po zastosowaniu begin >= 0 && begin + length <= channel->length
    void shiftTag(const std::string & path, double dTime);

    //-------------------- Pomocnicze funkcje ----------------------------

    //! \param channel Reprezentacja wezla w postaci NamedTreeBase
    //! \return Wskaznik na Channel
    static TChannelConstPtr toTChannel(const NamedTreeBaseConstPtr & channel);

    //! \param channel Kanal wynikajacy z elementow jak tag czy selection
    //! \return Kanal timeline opakowujacy zadany kanal logiczny
    TChannelConstPtr getTChannelForChannel(const ChannelConstPtr & channel) const;

    //! \param path Sciezka do szukanego kanalu
    //! \return Wskaznik do szukanego kanalu lub pusty wskaznik jesli nie znaleziono
    TChannelConstPtr findChannel(const std::string & path) const;

private:

    static void getAllChildrenData(const TChannelConstPtr & channel, ConstTags & tags, ConstChannels & channels);

    //! \param channel Kanal danych
    //! \param offset Offset kanalu wzgledem rodzica
    void innerSetChannelLocalOffset(const TChannelConstPtr & channel, double offset);

    //! \param channel Kanal danych
    //! \param offset Offset kanalu wzgledem 0
    void innerSetChannelGlobalOffset(const TChannelConstPtr & channel, double offset);

    //! \param channel Kanal danych
    void innerRemoveChannel(const TChannelConstPtr & channel);

    //! \param channel Kanal danych
    //! \param timeScale Skala czasu kanalu, lokalna wzgledem rodzica
    void innerSetChannelLocalTimeScale(const TChannelConstPtr & channel, double timeScale);

    //! \param channel Kanal danych
    //! \param timeScale Skala czasu kanalu, globalna
    void innerSetChannelGlobalTimeScale(const TChannelConstPtr & channel, double timeScale);

    //! \param channel Kanal danych
    //! \param name Nazwa kanalu uniklalna wzgledem jego rodzica
    void innerSetChannelName(const TChannelConstPtr & channel, const std::string & name);

    //! \param channel Kanal danych
    //! \param mask Maska czasu dla kanalu, musi byc w zakresie 0 <= mask <= length
    void innerSetChannelMask(const TChannelConstPtr & channel, const Channel::Mask & mask);

    //! \param channel Kanal danych
    //! \param maskBegin Poczatek maski czasu dla kanalu, musi byc w zakresie 0 <= maskBegin <= maskEnd
    void innerSetChannelMaskBegin(const TChannelConstPtr & channel, double maskBegin);

    //! \param channel Kanal danych
    //! \param maskEnd Koniec maski czasu dla kanalu, musi byc w zakresie maskBegin <= maskEnd <= length
    void innerSetChannelMaskEnd(const TChannelConstPtr & channel, double maskEnd);

    //! \param channel Kanal danych
    //! \param active Czy kanal jest aktywny podczas operacji czasowych (odtwarzanie timeline)
    void innerSetChannelActive(const TChannelConstPtr & channel, bool active);

    //! \param channel Kanal danych
    void innerClearChannelTags(const TChannelConstPtr & channel, bool deeper = false);

    //! \param channel Kanal danych
    //! \param time Czas kanalu w ktorym dokona sie podzial kanalu - 0 < time < length
    //! \param nameA Nazwa mlodszej czesci kanalu po podziale, musi byc unikalna wzgledem rodzica kanalu
    //! \param nameB Nazwa starszej czesci kanalu po podziale, musi byc unikalna wzgledem rodzica kanalu
    void innerSplitChannel(const TChannelConstPtr & channel, double time, const std::string & nameA = "A", const std::string & nameB = "B");

    //! oba kanalu musza byc w obrebie jednego rodzica
    //! \param channelA Pierwszy kanal do scalenia
    //! \param channelB Drugi kanal do scalenia
    //! \param name Nazwa kanalu po polaczeniu
    void innerMergeChannels(const TChannelConstPtr & channelA, const TChannelConstPtr & channelB, const std::string & name = "Merged");

    //! \param path Sciezka rozszerzona, gdzie ostatni element sciezki to nazwa, prawdziwa sciezka do kanalu znajduje sie wczesniej
    //! \return Podzielona sciezka na nazwe i sciezke
    static ExtPath getExtPath(const std::string & path);

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

    //! \param ratio Wzgledna zmiana skali do zastosowania na skalach dzieci
    void updateChildrenScale(const TChannelConstPtr & child, double ratio);

    //! \param child Dziecko ktorego dlugosc ulegla zmianie
    static void updateParentLength(const TChannelConstPtr & parent, const TChannelConstPtr & child);

    //! \param channel Kanal ktoremu uaktualniamy dlugosc
    //! \return Czy dlugosc kanalu ulegla zmianie
    static bool refreshChannelLength(const TChannelConstPtr & channel);

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