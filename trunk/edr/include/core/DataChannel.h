/********************************************************************
	created:  2011/02/17
	created:  17:2:2011   13:57
	filename: DataChannel.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_CORE__DATACHANNEL_H__
#define __HEADER_GUARD_CORE__DATACHANNEL_H__

#include <vector>
#include <utility>
#include <stdexcept>
#include <sstream>
#include <string>
#include <utils/Debug.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

    //! Manipulator danych bazuj¹cy na operatorach typu. Interpoluje liniowo.
    template <class PointType, class TimeType = double, class PointParamType = const PointType&>
    struct DataOperatorsManipulator
    {
        bool isLower(PointParamType p1, PointParamType p2)
        {
            return p1 < p2;
        }
        PointType normalize(PointParamType p, PointParamType min, PointParamType max) const
        {
            return (p - min) / (max - min);
        }
        PointType interpolate(PointParamType p1, PointParamType p2, TimeType t) const
        {
            return p1 + (p2 - p1) * t;
        }
    };

    //! Uniwersalny kontener na dane dyskretno-czasowe.
    template <class PointType, class TimeType = double, 
        class Manipulator = DataOperatorsManipulator<PointType, TimeType> >
    class DataChannel : public Manipulator
    {
    public:
        typedef TimeType time_type;
        typedef PointType point_type;
        //! Typ wpisu.
        struct Entry {
            const time_type time;
            point_type value;
            point_type normalizedValue;
        };

    private:
        typedef std::vector<Entry> Data;

    public:
        typedef typename Data::iterator iterator;
        typedef typename Data::const_iterator const_iterator;
        typedef typename Data::reverse_iterator reverse_iterator;
        typedef typename Data::const_reverse_iterator const_reverse_iterator;
        typedef typename Data::value_type value_type;
        typedef typename Data::reference reference;
        typedef typename Data::const_reference const_reference;
        

    protected:
        //! W³aœciwe dane.
        Data data;
        //! Liczba próbek na sekundê.
        const int samplesPerSec;
        //! 1 / liczba próbek na sekundê.
        const time_type samplesPerSecInv;
        //! Maksymalna wartoœæ.
        point_type maxValue;
        //!
        time_type maxValueTime;
        //! Minimalna wartoœæ.
        point_type minValue;
        //!
        time_type minValueTime;
        //! D³ugoœæ czasowa.
        time_type length;
        //! Opis jednostki X.
        std::string xUnit;
        //! Opis jednostki Y.
        std::string yUnit;
        //! Nazwa 
        std::string name;
        //! Bie¿¹cy czas.
        time_type time;

    public:
        //! \param samplesPerSec Liczba próbek na sekundê.
        explicit DataChannel(int samplesPerSec) :
        samplesPerSec(samplesPerSec),
        samplesPerSecInv( time_type(1.0) / time_type(samplesPerSec) ),
        time(0), length(0)
        {
            if ( samplesPerSec <= 0 ) {
                throw std::runtime_error( static_cast<std::ostringstream&>(std::ostringstream() 
                    << "Invalid samples per second number: " << 10).str() );
            }
        }

        //! Konstruktor kopiuj¹cy
        //! \param channel
        DataChannel(const DataChannel& channel) :
        samplesPerSec(channel.samplesPerSec),
        samplesPerSecInv(channel.samplesPerSecInv),
        data(channel.data),
        time(channel.time), length(channel.length)
        {
            if ( samplesPerSec <= 0 ) {
                throw std::runtime_error( static_cast<std::ostringstream&>(std::ostringstream() 
                    << "Invalid samples per second number: " << 10).str() );
            }
        }

        //! Pusty polimorficzny destruktor.
        virtual ~DataChannel()
        {}

    public:
        //! Klonuje kana³.
        virtual DataChannel* clone()
        {
            return new DataChannel(*this);
        }

        //! \return Bie¿¹cy czas.
        time_type getTime() const
        { 
            return time;
        }
        //! \param time Bie¿¹cy czas.
        void setTime(time_type time) 
        { 
            this->time = time; 
        }


        //! \return
        const std::string& getXUnit() const
        { 
            return xUnit;
        }
        //! \param xUnit
        void setXUnit(const std::string& xUnit) 
        { 
            this->xUnit = xUnit; 
        }
        //! \return
        const std::string& getYUnit() const
        { 
            return yUnit;
        }
        //! \param yUnit
        void setYUnit(const std::string& yUnit) 
        { 
            this->yUnit = yUnit; 
        }
        //! \return
        const std::string& getName() const
        { 
            return name;
        }
        //! \param name
        void setName(const std::string& name) 
        { 
            this->name = name; 
        }
        //! \return Liczba próbek na sekundê.
        int getSamplesPerSec() const
        { 
            return samplesPerSec;
        }
        //! \return D³ugoœæ kana³u w sekundach.
        time_type getLength() const
        {
            return length;
        }
        //! \return Liczba punktów pomiarowych.
        int getNumPoints() const
        {
            return static_cast<int>(data.size());
        }
        //! \return
        point_type getMaxValue() const
        { 
            if ( !getNumPoints() ) {
                throw std::runtime_error("No data points yet.");
            }
            return maxValue;
        }
        //! \return
        point_type getMinValue() const
        { 
            if ( !getNumPoints() ) {
                throw std::runtime_error("No data points yet.");
            }
            return minValue;
        }
        //! \return
        time_type getMaxValueTime() const
        {
            if ( !getNumPoints() ) {
                throw std::runtime_error("No data points yet.");
            }
            return maxValueTime;
        }
        //! \return
        time_type getMinValueTime() const
        {
            if ( !getNumPoints() ) {
                throw std::runtime_error("No data points yet.");
            }
            return minValueTime;
        }
        //! \param point Punkt pomiarowy do dodania.
        void addPoint(point_type point)
        {
            if ( data.size() == 0 ) {
                length = 0;
                value_type added = { 0, point, point_type() };
                maxValue = minValue = added.value;
                maxValueTime = minValueTime = added.time;
                data.push_back( added );
            } else {
                length += samplesPerSecInv;
                value_type added = { length, point, point_type() };
                if ( Manipulator::isLower(maxValue, added.value) ) {
                    maxValue = added.value;
                    minValueTime = added.time;
                } else if ( Manipulator::isLower(added.value, minValue) ) {
                    minValue = added.value;
                    minValueTime = added.time;
                }
                data.push_back( added );
            }
        }

        //! \param first
        //! \param last
        template <class It>
        void addPoints(It first, It last)
        {
            while ( first != last ) {
                addPoint(*first++);
            }
        }
        //! Usuwa wszystkie punkty pomiarowe.
        void removePoints()
        {
            data.clear();
            length = 0;
            time = 0;
        }

        //! Zapisuje znormalizowane dane do pól normalizedValue.
        void normalize()
        {
            if ( !getNumPoints() ) {
                throw std::runtime_error("No data points yet.");
            }
            // w ten sposób istnieje szansa, ¿e bêdzie zrobione z u¿yciem instrukcji
            // wektorowych...
            iterator last = end();
            for (iterator it = begin(); it != last; ++it) {
                it->normalizedValue = Manipulator::normalize(it->value, minValue, maxValue);
            }
        }
        
        //! \param time
        //! \return Zinterpolowana wartoœæ dla zadanego czasu.
        point_type getValue(time_type time) const
        {
            auto points = getValueHelper(time);
            return Manipulator::interpolate(points.first.value, points.second.value, (time - points.first.time) * samplesPerSec );
        }

        //! \param renormalize Czy najpierw interpolowaæ oryginalne wartoœci a potem dopiero znormalizowaæ?
        //!        Ten parametr mo¿e byæ przydatny, gdy interpolator mo¿e dzia³aæ wy³¹cznie w oparciu o oryginalne wartoœci.
        //! \return Znormalizowana wartoœæ dla zadanego czasu.
        point_type getNormalizedValue(time_type time, bool renormalize = false) const
        {
            if ( !renormalize ) {
                auto points = getValueHelper(time);
                return Manipulator::interpolate(points.first.normalizedValue, points.second.normalizedValue, (time - points.first.time) * samplesPerSec );
            } else {
                return Manipulator::normalize( getValue(time), minValue, maxValue );
            }
        }

        //! \return Bie¿¹ca wartoœæ.
        point_type getCurrentValue() const
        {
            return getValue(time);
        }

        //! \return Bie¿¹ca znomalizowana wartoœæ.
        point_type getCurrentNormalizedValue(bool renormalize = false) const
        {
            return getCurrentValue(time);
        }

        //! \param size Miejsce na ile elementów zarezerwowaæ?
        void reserve(int size)
        {
            data.reserve(size);
        }

    public:
        //! \return Czas punktu.
        //! \return Wartoœæ na podstawie punktu czasowego.
        point_type operator[](time_type time) const
        {
            return getValue(time);
        }
        //! \return Indeks punktu.
        //! \return Wartoœæ na podstawie indeksu punktu czasowego.
        reference operator[](int idx)
        {
            return data[idx];
        }
        //! \return Indeks punktu.
        //! \return Wartoœæ na podstawie indeksu punktu czasowego.
        const_reference operator[](int idx) const
        {
            return data[idx];
        }

        //! \return
        iterator begin()
        {
            return data.begin();
        }
        //! \return
        const_iterator begin() const
        {
            return data.begin();
        }
        //! \return
        iterator end()
        {
            return data.end();
        }
        //! \return
        const_iterator end() const
        {
            return data.end();
        }
        //! \return
        reverse_iterator rbegin()
        {
            return data.rbegin();
        }
        //! \return
        const_reverse_iterator rbegin() const
        {
            return data.begin();
        }
        //! \return
        reverse_iterator rend()
        {
            return data.rend();
        }
        //! \return
        const_reverse_iterator rend() const
        {
            return data.end();
        }

    protected:
        //! \return Para punktów pomiarowych pomiêdzy którymi nale¿y wykonaæ interpolacjê.
        std::pair<const_reference, const_reference> getValueHelper(time_type time) const
        {
            if ( time > getLength() ) {
                throw std::runtime_error("Time overflow.");
            }
            int idx = static_cast<int>(time * samplesPerSec);
            UTILS_ASSERT( idx <= getNumPoints() );
            if ( idx == getNumPoints()-1 ) {
                const_reference p = operator[](getNumPoints()-1);
                return std::make_pair(p, p);
            } else {
                return std::make_pair( operator[](idx), operator[](idx+1) );
            }
        }

    };

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

#endif  // __HEADER_GUARD_CORE__DATACHANNEL_H__