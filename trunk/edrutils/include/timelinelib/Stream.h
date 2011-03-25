#ifndef HEADER_GUARD__TIMELINESTREAM_H__
#define HEADER_GUARD__TIMELINESTREAM_H__

#include <timeline/Types.h>
#include <boost/enable_shared_from_this.hpp>

////////////////////////////////////////////////////////////////////////////////
namespace timeline{
////////////////////////////////////////////////////////////////////////////////

/* Klasa do obudowania strumieni, wykorzystywana wewnetrznie przez timeline, definiuje 
* dodatkowe wlasciwosci strumieni na potrzebu timeline
*/
class Stream
{
private:

    //! \param double, offset czasu wzgledem poczatku rodzica
    double offset;

    //! \param IStreamPtr - wewnetrzny strumien danych
    IStreamPtr innerStream;

    //! \param double skala z jaka przetwarzamy strumien
    //! 1 - oryginalny czas, > 1 szybciej, < 1 wolniej
    double scale;

public:

    //! Konstruktor zerujacy
    Stream();

    virtual ~Stream();

    const IStreamPtr & getInnerStream() const;

    //! \return offset czasu - przesuniecie wzgledem poczatku rodzica, uwzglednia skale
    double getOffset() const;

    //! \param double nowy offset czasu wzglede poczatku czasu rodzica, uwzglednia skale
    void setOffset(double offset);

    //! \return dlugosc strumienia w milisekundach, uwzgledniajac skale
    double getLength() const;

    //! \return aktualny czas strumienia
    double getTime() const;

    //! \param aktualny czas strumienia uwzgledniajac skale
    void setTime(double time);

    //! \return zwrca skale czasu strumienia
    double getScale() const;

    //! \param double nowa skala strumienia
    void setScale(double scale);

protected:

    //! Konstruktor uzywany przez pochodne klasy Stream w celu opakowania faktycznych strumieni IStream
    //! \param IStreamPtr strumien do opakowania
    IStream(IStreamPtr parent);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD__TIMELINESTREAM_H__