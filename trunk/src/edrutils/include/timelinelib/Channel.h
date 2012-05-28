#ifndef HEADER_GUARD__TIMELINESTREAM_H__
#define HEADER_GUARD__TIMELINESTREAM_H__

#include <timelinelib/Types.h>
#include <timelinelib/IChannel.h>
#include <utils/NamedTree.h>
#include <utils/DataChannel.h>
#include <utils/ObserverPattern.h>

namespace timeline {

class Model;

/** Klasa do obudowania kanalow, wykorzystywana wewnetrznie przez timeline, definiuje 
* dodatkowe wlasciwosci kanalow na potrzeby timeline
*/
class Channel
{
    friend class Model;

public:

    typedef ConstTags::const_iterator tag_const_iterator;
    typedef ConstTags::size_type tag_size_type;

    //! Poczatek maski i jej dlugosc
    typedef std::pair<double, double> Mask;

protected:

    typedef Tags::iterator tag_iterator;

private:

    //! Dlugosc kanalu
    double length;

    //! Maska czasu dla danego kanalu
    Mask mask;
    
    //! Offset czasu wzgledem poczatku rodzica
    double localOffset;

    //! Offset czasu wzgledem 0
    double globalOffset;

    //! Skala z jaka przetwarzamy kanal, lokalna, trzaba uwzglêdniæ lokaln¹ skalê rodzica
    //! 1 - oryginalny czas, > 1 szybciej, < 1 wolniej
    double localScale;

    //! Skala z jaka przetwarzamy kanal, globalna po uwzglêdnieniu rodzica
    //! 1 - oryginalny czas, > 1 szybciej, < 1 wolniej
    double globalScale;

    //! Czas kanalu uwzgledniajacy skale
    double time;

    //! Czy kanal jest aktywny podczas przetwarzania
    bool active;

    //! Tagi wystepujace w timeline
    Tags tags;

    //! Tagi wystepujace w timeline
    ConstTags constTags;

    //! Kanal dostarczony przez klienta
    IChannelPtr innerChannel;

    //! Kanal dostarczony przez klienta
    IChannelConstPtr constInnerChannel;

public:

    //! Konstruktor zerujacy uzywany w celu stworzenia virtualnych kanalow
    //! tutaj tworzymy root
    //! \name nazwa kanalu
    //! \param channel Kanal do opakowania jako root
    Channel(const IChannelPtr & channel = IChannelPtr());

    Channel(const Channel & channel, bool deep = true);

    virtual ~Channel();


    // ------------ Obsluga dodatkowych cech strumienia danych-------------------------------------

    //! \return Czy globalny czas jest w zakresie obslugiwanym przez ten kanal
    //! \param time Globalny czas
    bool timeInChannel(double time) const;

    //! \param time Czas który chcemy ustawiæ w kanale
    //! \return Poprawny czas w kanale - ograniczony do zakresu kana³u wraz z maska
    double makeTimeInChannel(double time) const;

    //! \param channel Kanal zdefiniowany przez uzytkownika i opakowany przez nasza klase
    void setInnerChannel(const IChannelPtr & channel);

    //! \return Kanal zdefinowany przez klienta
    const IChannelConstPtr & getInnerChannel() const;

    //! \param mask Maska czasu realizowana wewnetrznie przez nasz kanal
    void setMask(const Mask & mask);

    //! \param maskBegin Poczatek maski - wiekszy lub rowny 0
    void setMaskBegin(double maskBegin);

    //! \param maskLength Dlugosc maski - maskBegin + maskLength <= channel length
    void setMaskLength(double maskLength);

    //! \param maskEnd Koniec maski - mniejszy lub rowny length
    void setMaskEnd(double maskEnd);

    //! \param offset Czas wzgledem poczatku czasu rodzica
    void setLocalOffset(double offset);

    //! \param offset Czas wzgledem 0
    void setGlobalOffset(double offset);

    //! \param Aktualny czas kanalu uwzgledniajac skale rodzica
    void setTime(double time);

    //! \param scale Skala kanalu, jest skladana ze skala rodzica
    void setLocalTimeScale(double scale);

    //! \param scale Skala kanalu, bezwzgledna wartosc skali
    void setGlobalTimeScale(double scale);

    //! \param active Czy strumien jest aktywny podczas operacji oczasowych i odtwarzania timeline
    void setActive(bool active);    

    //! \return Wewnetrzna maska czasu kanalu
    const Mask & getMask() const;
    
    //! \return Poczatek maski
    double getMaskBegin() const;

    //! \return Dlugosc maski
    double getMaskLength() const;

    //! \return Koniec maski
    double getMaskEnd() const;

    //! \return Offset czasu - przesuniecie wzgledem poczatku rodzica
    double getLocalOffset() const;

    //! \return Offset czasu - przesuniecie wzgledem 0
    double getGlobalOffset() const;

    //! \return Dlugosc kanalu w milisekundach, uwzgledniajac skale
    double getLength() const;

    //! \param Dlugosc kanalu
    void setLength(double length);

    //! \return Aktualny czas kanalu
    double getTime() const;

    //! \return Skala czasu kanalu wzgledem skali rodzica
    double getLocalTimeScale() const;

    //! \return Skala czasu kanalu
    double getGlobalTimeScale() const;

    //! maska dla podkanalow
    //! \return Czy kanal jest aktywny podczas odtwarzania timeline i operacji czasowych
    bool isActive() const;

    //------- Obsluga tagow ---------------------

    //! \return Pierwszy tag
    tag_const_iterator beginTags() const;

    //! \return Koniec tagow
    tag_const_iterator endTags() const;

    //! \param name Nazwa szukanego taga
    //! \return Iterator znalezionego taga lub selectionsEnd() jesli nie znaleziono
    tag_const_iterator findTag(const std::string & name) const;

    //! \param idx Indeks tagu
    //! \return Tag
    const TagConstPtr & getTag(tag_size_type idx) const;

    //! \param name Nazwa tagu
    //! \return Tag
    const TagConstPtr & getTag(const std::string & name) const;

    //! \return Ilosc tagow danego kanalu
    tag_size_type getNumTags() const;

protected:

    //! \return Kanal zdefinowany przez klienta
    const IChannelPtr & getInnerChannel();

    //! Czysci liste tagow zwiazana z tym kanalem
    void clearTags();

    //! \param tag Tag do wstawienia dla danego kanalu
    void addTag(const TagPtr & tag);

    //! \param tag Tag do usuniecvia dla danego kanalu
    void removeTag(const TagPtr & tag);

    //! \param idx Indeks tagu
    //! \return Tag
    const TagPtr & getTag(tag_size_type idx);

    //! \param name Nazwa tagu
    //! \return Tag
    const TagPtr & getTag(const std::string & name);

    //! \param name Nazwa szukanego taga
    //! \return Iterator znalezionego taga lub selectionsEnd() jesli nie znaleziono
    tag_iterator findTag(const std::string & name);

    //! \return Pierwszy tag
    tag_iterator beginTags();

    //! \return Koniec tagow
    tag_iterator endTags();

    //! \param begin Pierwszy iterator do przeszukiwanego zakresu
    //! \param end Koniec zakresu
    //! \param name Szukana nazwa elementu
    //! \return Pozycja znalezionego elementu lub koniec zakresu
    template<class Iter>
    static Iter findByName(Iter begin, Iter end, const std::string & name)
    {
        while(begin != end) {
            if((*begin)->getName() == name) {
                break;
            }
            ++begin;
        }

        return begin;
    }
};

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD__TIMELINESTREAM_H__