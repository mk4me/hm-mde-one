#ifndef HEADER_GUARD__TIMELINETAGSELECTION_H__
#define HEADER_GUARD__TIMELINETAGSELECTION_H__

#include <timelinelib/SelectionBase.h>
#include <timelinelib/Types.h>
#include <boost/enable_shared_from_this.hpp>

////////////////////////////////////////////////////////////////////////////////
namespace timeline{
////////////////////////////////////////////////////////////////////////////////

//! Klasa obslugujaca zaznaczenie pomiedzy dwoma zdefiniowanymi tagami
class TagSelection : public SelectionBase, public boost::enable_shared_from_this<TagSelection>
{

    friend class Model;

private:

    //! Poczatek czasu zaznaczenia
    TagWPtr begin;

    //! Poczatek czasu zaznaczenia
    TagConstWPtr constBegin;

    //! Koniec czasu zaznaczenia
    TagWPtr end;

    //! Koniec czasu zaznaczenia
    TagConstWPtr constEnd;

public:

    ~TagSelection(void);

    //! \return Poczatek zaznaczenia - czas
    virtual double getBegin() const;

    //! \return Koniec zaznaczenia - czas
    virtual double getEnd() const;

    //! \return Poczatkowy tag zaznaczenia
    const TagConstWPtr & getBeginTag() const;

    //! \return Koncowy Tag zaznaczenia
    const TagConstWPtr & getEndTag() const;

    //! \param Wskaznik na bazowa klase zaznaczen
    //! \return Wskaznik do TagSelection lub pusty wskaznik jesli to nie ta klasa lub jej pochodna
    static TagSelectionPtr getTagSelection(const SelectionPtr & selection);

    //! \param Wskaznik na bazowa klase zaznaczen
    //! \return Wskaznik do TagSelection lub pusty wskaznik jesli to nie ta klasa lub jej pochodna
    static TagSelectionConstPtr getTagSelection(const SelectionConstPtr & selection);

    //! \return Poczatkowy tag zaznaczenia
    const TagWPtr & getBeginTag();

    //! \return Koncowy Tag zaznaczenia
    const TagWPtr & getEndTag();

    //! \param Poczatek zaznaczenia - czas
    virtual void setBegin(double begin);

    //! \param Koniec zaznaczenia - czas
    virtual void setEnd(double end);

    //! \return Poczatkowy tag zaznaczenia
    void setBeginTag(const TagPtr & begin);

    //! \return Koncowy tag zaznaczenia
    void setEndTag(const TagPtr & end);

protected:

    //! konstruktor
    //! \param begin Tag poczatkowy zaznaczenia
    //! \paran end Tag koncowy zaznaczenia
    //! \paran name Nazwa zaznaczenia
    TagSelection(const TagPtr & begin, const TagPtr & end, const std::string & name = std::string("UnnamedSelection"));
};

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD__TIMELINETAGSELECTION_H__