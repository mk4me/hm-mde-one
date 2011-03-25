#ifndef HEADER_GUARD_TIMELINE__ICHANNEL_H__
#define HEADER_GUARD_TIMELINE__ICHANNEL_H__

#include <timelinelib/Types.h>

namespace timeline
{

    /** Klasa bazowa dla kanalow obslugiwanych przez timeline, definiuje ich podstawowe
    * wlasciwosi i operacje
    */
    class IChannel
    {
    public:

        virtual ~IChannel() {}

        //! \return Sklonowane dane w kanale
        virtual IChannel* clone() const = 0;

        //! \return Dlugosc kanalu w sekundach
        virtual double getLength() const = 0;

        //! Czas zawiera siê miêdzy 0 a getLength()
        //! \param time Aktualny, lokalny czas kanalu w sekundach
        virtual void setTime(double time) = 0;

    };

} // namespace timeline

#endif  // HEADER_GUARD_TIMELINE__ICHANNEL_H__