/********************************************************************
	created:  2010/10/07
	created:  7:10:2010   11:55
	filename: Stream.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__TIMELINESTREAM_H__
#define __HEADER_GUARD__TIMELINESTREAM_H__

#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

////////////////////////////////////////////////////////////////////////////////
namespace timeline {
////////////////////////////////////////////////////////////////////////////////

/**
 * Klasa bazowa dla strumieni u�ywanych w kontrolce timeline.
 * Typ jest do u�ytku wewn�trznego przez timeline::Model i 
 * nie nale�y utrzymywa� do niego referencji.
 */
class Stream
{
private:
    //! Nazwa strumienia.
    std::string name;
    //!
    double startOffset;

public:
    //! Pomocnicza funkcja do enkapsulacji strumieni i podpinania ich do timeline'a.
    //! W kt�rym� miejscu nale�y po prostu zdefiniowa� specjalizacj� tej metody.
    //! Dla wersji tutaj podanej nie ma definicji, wi�c wi�zanie odbywa si� tylko
    //! przez specjalizacj�.
    //! \tparam InnerStream Strumie� do enkapsulacji.
    //! \param stream Strumie� do enkapsulacji.
    template <class InnerStream>
    static Stream* encapsulate(InnerStream * stream);

    //! Konstruktor zeruj�cy.
    Stream() : startOffset(0.0f), name("UnnamedStream") {}
    //! Destruktor wirtualny.
    virtual ~Stream() {}

    //! \param time Bie��cy czas.
    virtual void setTime(double time) = 0;
    //! \return Bie��cy czas.
    virtual double getTime() const = 0;
    //! \return D�ugo�� strumienia.
    virtual double getLength() const = 0;

    //! \return Nazwa strumienia.
    virtual const std::string & getName() const
    {
        return name;
    }
    //! \param name Nazwa strumienia.
    virtual void setName(const std::string & name)
    {
        this->name = name;
    }
    //! \return Offset w sekundach.
    virtual double getStartOffset() const
    { 
        return startOffset;
    }
    //! \param startOffset Offset w sekundach.
     virtual void setStartOffset(double startOffset) 
    { 
        this->startOffset = startOffset; 
    }
};

//! Wska�nik na strumie�.
typedef boost::shared_ptr<Stream> StreamPtr;
//! Zbi�r strumieni.
typedef std::vector<StreamPtr> Streams;

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__TIMELINESTREAM_H__