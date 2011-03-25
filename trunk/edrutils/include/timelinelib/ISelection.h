#ifndef HEADER_GUARD__TIMELINEISELECTION_H__
#define HEADER_GUARD__TIMELINEISELECTION_H__

#include <utils/ObserverPattern.h>
#include <timeline/Types.h>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
namespace timeline{
////////////////////////////////////////////////////////////////////////////////

/* Klasa bazowa dla tworzenia zaznaczen fragmentow strumieni.
* Oferuje tylko podstawowe API, implementacja powinna pojawic sie w konkretnych
* klasa pochodnych.
*/
class ISelection : public utils::Observable<ISelection>
{
private:

    //! nazwa zaznaczenia
    std::string name;

    //! strumien ktorego dotyczy zaznaczenie
    IStreamWPtr stream;

public:

    //! konstruktor
    //! \param IStreamPtr strumien dla ktorego tworzy sie zaznaczenie
    //! \param string nazwa zaznaczenia
    ISelection(IStreamPtr stream, const std::string & name = std::string("UnnamedSelection"));

    //! \return nazwa tagu
    const std::string & getName() const;

    //! \param string nowe nazwa tagu
    void setName(const std::string & name);

    //! \return zwraca strumien ktory jest tagowany
    const IStreamWPtr & getStream() const;

    //! \return poczatek zaznaczenia - czas
    virtual double getBegin() const = 0;

    //! \return koniec zaznaczenia - czas
    virtual double getEnd() const = 0;

    //! \return czas trwania zaznaczenia
    double getLength() const;

    //! \param double poczatek zaznaczenia - czas
    virtual void setBegin(double begin) = 0;

    //! \param double koniec zaznaczenia - czas
    virtual void setEnd(double end) = 0;

    //! \param double przesuniecie zaznaczenia
    virtual void shiftSelection(double delta);

};

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD__TIMELINEISELECTION_H__