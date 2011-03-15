#ifndef HEADER_GUARD__TIMELINEMODEL_H__
#define HEADER_GUARD__TIMELINEMODEL_H__

#include <timeline/Types.h>
#include <utils/ObserverPattern.h>
//#include <boost/bimap.hpp>
#include <timeline/State.h>
#include <map>

////////////////////////////////////////////////////////////////////////////////
namespace timeline{
////////////////////////////////////////////////////////////////////////////////

class Model : public utils::Observable<Model>
{
public:
    typedef Streams::iterator stream_iterator;
    typedef Streams::const_iterator stream_const_iterator;
    typedef Streams::size_type stream_size_type;

    typedef Tags::iterator tag_iterator;
    typedef Tags::const_iterator tag_const_iterator;
    typedef Tags::size_type tag_size_type;

    typedef Selections::iterator selection_iterator;
    typedef Selections::const_iterator selection_const_iterator;
    typedef Selections::size_type selection_size_type;

private:
    //! struktura opisujaca rzeczywisty strumien dostarczony przez uzytkownika
    //! w sposob w jaki widzi i opisuje go timeline
    typedef struct StreamDescriptor{
        //! \param strumien timeline opakowujacy rzeczywisty strumien uzytkownika
        StreamPtr stream;

        //! \param stan strumienia podczas odtwarzania
        bool active;
    }StreamDescriptor;

private:

    //! \param zbior strumieni glownych w timeline
    Streams rootStreams;

    //! \param zbior wszystkich strumieni w timeline
    Streams allStreams;

    //! \param mapowanie rzeczywistych strumieni dostarczanych przez uzytkownika
    //! do strumieni obudowanych na potrzeby timeline
    //boost::bimap<IStreamPtr, StreamPtr> streamsMapping;

    std::map<IStreamPtr, StreamDescriptor> streamsDescriptors;

    //! \param tagi wystepujace w timeline
    Tags allTags;

    //! \param zbior zaznaczen w timeline
    Selections allSelections;

    //! \param stan timeline
    State state;

public:

    //! Konstruktor zerujacy
    Model();

    ~Model();

    //! \return aktualny czas timeline
    double getTime() const;

    //! \param aktualny czas strumienia uwzgledniajac skale
    void setTime(double time);

    //! \return //! \param double sumaryczna dlugosc timeline, od najwczesniejszego strumienia do konca ostatniego 
    double getLength() const;

    //! \return zwrca skale czasu strumienia
    double getScale() const;

    //! \param double nowa skala strumienia
    void setScale(double scale);

    //! \return czy timeline sie odtwarza
    bool isPlaying() const;

    //! \param czy odtwarzac timeline
    void setPlaying(bool play);

    //! \return zwraca stan timeline
    const State & getState() const;

    //! \param nowy stan timeline
    void getState(const State & state);


    //------- Obsluga strumieni root ---------------------

    //! \return pierwszy strumien root
    stream_iterator beginRootStreams();

    //! \return koniec strumieni root
    stream_iterator endRootStreams();

    //! \return pierwszy strumien root
    stream_const_iterator beginRootStreams() const;

    //! \return koniec strumieni root
    stream_const_iterator endRootStreams() const;

    //! \param idx indeks strumienia root
    //! \return strumien root
    IStreamPtr & getRootStream(stream_size_type idx);

    //! \param idx indeks strumienia root
    //! \return strumien root
    const IStreamPtr & getRootStream(stream_size_type idx) const;

    //! \return zwraca ilosc strumieni root
    stream_size_type getNumRootStreams() const;


    //------- Obsluga wszystkich strumieni ---------------------

    //! \return pierwszy strumien
    stream_iterator beginStreams();

    //! \return koniec strumieni
    stream_iterator endStreams();

    //! \return pierwszy strumien
    stream_const_iterator beginStreams() const;

    //! \return koniec strumieni
    stream_const_iterator endStreams() const;

    //! \return zwraca ilosc strumieni
    stream_size_type getNumStreams() const;


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


    //------- Zarzadzanie strumieniami ---------------------

    //! \param strumien do dodania, zostanie wewnetrznie obudowany w klase Stream
    void addStream(IStreamPtr stream);

    //! \param strumien do usuniecia
    void removeStream(IStreamPtr stream);

    //! \return zwraca czy strumien jest aktyny podczas odtwarzania timeline
    void isStreamActive(IStreamPtr stream) const;

    //! \param ustawia czy strumien jest aktywny podczas odtwarzania timeline
    bool setStreamActive(IStreamPtr stream, bool active);


    //------- Zarzadzanie tagami ---------------------

    //! \param tag do dodania, musi miec poprawnie ustawiony strumien
    void addTag(TagPtr tag);

    //! \param tag do usuniecia
    void removeTag(TagPtr tag);


    //------- Zarzadzanie zaznaczeniami ---------------------

    //! \param zaznaczenie do dodania, musi miec poprawnie ustawiony strumien i wartosci
    void addSelection(SelectionPtr selection);

    //! \param zaznaczenie do usuniecia
    void removeSelection(SelectionPtr selection);

};

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD__TIMELINEMODEL_H__