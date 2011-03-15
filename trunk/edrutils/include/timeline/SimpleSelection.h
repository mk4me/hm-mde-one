#ifndef HEADER_GUARD__TIMELINESIMPLESELECTION_H__
#define HEADER_GUARD__TIMELINESIMPLESELECTION_H__

////////////////////////////////////////////////////////////////////////////////
namespace timeline{
////////////////////////////////////////////////////////////////////////////////

class SimpleSelection :
    public timeline::ISelection
{

private:

    //! \param double poczatek czasu zaznaczenia
    double begin;

    //! \param double koniec czasu zaznaczenia
    double end;

public:
    //! konstruktor
    //! \param IStreamPtr strumien dla ktorego tworzy sie dane zaznaczenie
    //! \paran string nazwa zaznaczenia
    SimpleSelection(IStreamPtr stream, const std::string & name = std::string("UnnamedSelection"));

    ~SimpleSelection(void);

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

#endif  // HEADER_GUARD__TIMELINESIMPLESELECTION_H__