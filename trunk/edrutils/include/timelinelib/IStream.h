#ifndef HEADER_GUARD__TIMELINEISTREAM_H__
#define HEADER_GUARD__TIMELINEISTREAM_H__

#include <timeline/Types.h>
#include <boost/enable_shared_from_this.hpp>

////////////////////////////////////////////////////////////////////////////////
namespace timeline{
////////////////////////////////////////////////////////////////////////////////

/* Klasa bazowa dla strumieni obslugiwanych przez timeline, definiuje ich podstawowe
* wlasciwosi i operacje
*/
class IStream : public boost::enable_shared_from_this<IStream>
{
private:

    //! \param string nazwa strumienia
    std::string name;

    //! \param Streams - lista podstrumieni danego strumienia
    IStreams subStreams;

    //! \param WStreamPtr strumien rodzic
    IStreamWPtr parent;

    //! \param WStreamPtr strumien root dla wszystkich strumieni w hierarchi
    IStreamWPtr root;

    //! \param Tags tagi zwiazane z tym strumieniem
    Tags tags;

    //! \param ISelections zaznaczenia zwiazane z tym strumieniem
    Selections selections;

public:

    typedef IStreams::iterator stream_iterator;
    typedef IStreams::const_iterator stream_const_iterator;
    typedef IStreams::size_type stream_size_type;

    typedef Tags::iterator tag_iterator;
    typedef Tags::const_iterator tag_const_iterator;
    typedef Tags::size_type tag_size_type;

    typedef Selections::iterator selection_iterator;
    typedef Selections::const_iterator selection_const_iterator;
    typedef Selections::size_type selection_size_type;

public:
    
    //! Konstruktor zerujacy
    IStream();

    virtual ~IStream();

    //! \return dlugosc strumienia w milisekundach
    virtual double getLength() const = 0;

    //! \param double - aktualny czas strumienia
    virtual void setTime(double time) = 0;

    //! \return nazwa strumienia
    const std::string & getName() const;
    
    //! \param string - ustawia nazwe strumienia
    void setName(const std::string & name);


    //------- Obsluga strumieni ---------------------

    //! \return pierwszy podstrumien
    stream_iterator beginSubStreams();

    //! \return koniec podstrumieni
    stream_iterator endSubStreams();

    //! \return pierwszy podstrumien
    stream_const_iterator beginSubStreams() const;

    //! \return koniec podstrumieni
    stream_const_iterator endSubStreams() const;

    //! \param idx indeks podstrumienia
    //! \return strumien
    IStreamPtr & getSubStream(stream_size_type idx);

    //! \param idx indeks podstrumienia
    //! \return strumien
    const IStreamPtr & getSubStream(stream_size_type idx) const;

    //! \return zwraca ilosc podstrumieni danego strumienia
    stream_size_type getNumSubStreams() const;


    //------- Obsluga tagow ---------------------

    //! \return pierwszy tag
    tag_iterator beginTags();

    //! \return koniec tagow
    tag_iterator endTags();

    //! \return pierwszy tag
    tag_const_iterator beginTags() const;

    //! \return koniec tagow
    tag_const_iterator endTags() const;

    //! \param idx indeks tagu
    //! \return tag
    TagPtr & getTag(tag_size_type idx);

    //! \param idx indeks tagu
    //! \return tag
    const TagPtr & getTag(tag_size_type idx) const;

    //! \return zwraca ilosc tagow danego strumienia
    tag_size_type getNumTags() const;


    //------- Obsluga zaznaczen ---------------------

    //! \return pierwsze zaznaczenie
    selection_iterator beginSelections();

    //! \return koniec zaznaczen
    selection_iterator endSelections();

    //! \return pierwsze zaznaczenie
    selection_const_iterator beginSelections() const;

    //! \return koniec zaznaczen
    selection_const_iterator endSelections() const;

    //! \param idx indeks zaznaczenia
    //! \return tag
    SelectionPtr & getSelection(selection_size_type idx);

    //! \param idx indeks zaznaczenia
    //! \return tag
    const SelectionPtr & getSelection(selection_size_type idx) const;

    //! \return zwraca ilosc zaznaczen danego strumienia
    selection_size_type getNumSelections() const;

protected:

    //! Konstruktor uzywany przez pochodne klasy IStream w celu dodawania podstrumieni
    //! \param StreamPtr strumien rodzic dla aktualnego strumienia
    IStream(IStreamPtr parent, IStreamPtr root);

    //! \param substream podstrumien do dodania
    void addInternalSubStream(IStreamPtr substream);

    //! \param substream podstrumien do usuniecia
    void removeInternalSubStream(IStreamPtr substream);

    //! \return strumien rodzica
    const IStreamWPtr & getParentStream() const;

    //! \return strumien root dla calej hierarchi
    const IStreamWPtr & getRootStream() const;

};

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD__TIMELINEISTREAM_H__