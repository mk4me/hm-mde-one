#ifndef HEADER_GUARD__TIMELINETAGSELECTION_H__
#define HEADER_GUARD__TIMELINETAGSELECTION_H__

#include <timeline/ISelection.h>
#include <timeline/Types.h>

////////////////////////////////////////////////////////////////////////////////
namespace timeline{
////////////////////////////////////////////////////////////////////////////////

class TagSelection :
    public timeline::ISelection, public utils::Observer<Tag>
{
private:

    //! \param double poczatek czasu zaznaczenia
    TagWPtr begin;

    //! \param double koniec czasu zaznaczenia
    TagWPtr end;

public:
    //! konstruktor
    //! \param IStreamPtr strumien dla ktorego tworzy sie dane zaznaczenie
    //! \paran string nazwa zaznaczenia
    TagSelection(const std::string & name = std::string("UnnamedSelection"),
        TagPtr begin, TagPtr end);

    ~TagSelection(void);

    //! \return poczatek zaznaczenia - czas
    virtual double getBegin() const;

    //! \return koniec zaznaczenia - czas
    virtual double getEnd() const;

    //! \param double poczatek zaznaczenia - czas
    virtual void setBegin(double begin);

    //! \param double koniec zaznaczenia - czas
    virtual void setEnd(double end);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD__TIMELINETAGSELECTION_H__