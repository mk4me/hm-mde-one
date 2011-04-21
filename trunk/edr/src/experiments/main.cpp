#include <iostream>
#include <core/C3DChannels.h>
#include <core/C3DParserEx.h>
using namespace std;
using namespace core;
using namespace osg;

template <class PointType>
class DataChannel2
{
public:
    typedef float time_type;
    typedef PointType point_type;
    
    struct Entry {
        const time_type time;
        point_type value;
    };

    typedef std::vector<Entry> Data;
    typedef typename Data::const_reference const_reference;
    typedef typename Data::reference reference;

private:
    Data data;
    const int samplesPerSec;
    const time_type samplesPerSecInv;

public:
    //! \param samplesPerSec Liczba próbek na sekundê.
    explicit DataChannel2(int samplesPerSec) : samplesPerSec(samplesPerSec), samplesPerSecInv(time_type(1)/time_type(samplesPerSec))
    {}

    //! \return Liczba punktów pomiarowych.
    int getNumPoints() const
    {
        return static_cast<int>(data.size());
    }

    //! \param point Punkt pomiarowy do dodania.
    void addPoint(point_type point)
    {
        Entry added = { data.size() * samplesPerSecInv, point };
        data.push_back(added);
    }

    //! \return Indeks punktu.
    //! \return Wartoœæ na podstawie indeksu punktu czasowego.
    const_reference operator[](int idx) const
    {
        return data[idx];
    }

    //! \return Indeks punktu.
    //! \return Wartoœæ na podstawie indeksu punktu czasowego.
    reference operator[](int idx)
    {
        return data[idx];
    }

    //! \param time
    //! \return Zinterpolowana wartoœæ dla zadanego czasu.
    point_type getValue(time_type time) const
    {
        auto points = getValueHelper(time);
        // lerp
        float t = (time - points.first.time) * samplesPerSec;
        return points.first.value + (points.second.value - points.first.value) * t;
    }

    //! \return Para punktów pomiarowych pomiêdzy którymi nale¿y wykonaæ interpolacjê.
    std::pair<const_reference, const_reference> getValueHelper(time_type time) const
    {
        // wyznaczenie indeksów
        int idx = static_cast<int>(time * samplesPerSec);
        if ( idx == getNumPoints()-1 ) {
            const_reference p = operator[](getNumPoints()-1);
            return std::make_pair(p, p);
        } else {
            return std::make_pair( operator[](idx), operator[](idx+1) );
        }
    }

};

template <class Collection, class IndexType>
std::pair<typename Collection::const_reference, typename Collection::const_reference> getPair(const Collection& collection, IndexType i)
{
    typedef std::pair<typename Collection::const_reference, typename Collection::const_reference> pair;
    if ( i < collection.size() ) {
        if ( i == collection.size() - 1 ) {
            Collection::const_reference p = collection[i];
            //return std::make_pair(std::cref(p), std::cref(p));
            return std::make_pair(p, p);
            //return pair(p, p);
        } else {
            //return std::make_pair(std::cref(collection[i]), std::cref(collection[i+1]));
            return std::make_pair(collection[i], collection[i+1]);
            //return pair(collection[i], collection[i+1]);
        }
    } else {
        throw std::out_of_range("out of range");
    }
}

int main()
{
    const int markersCount = 10;
    const int perSecond = 1000;

    std::vector<int> vec;
    for ( int i = 0; i < 100; ++i ) {
        vec.push_back(i);
    }
    auto p = getPair(vec, 10);
    cout << p.first << " " << p.second << endl;


    {
        DataChannel2<osg::Vec3> m(perSecond);
        for (int j = 0; j < perSecond; j++) {
            osg::Vec3 t(j / 1000.0, j / 500.0, j / 33.0);
            
            m.addPoint(t);
        }
        Vec3 v =  m.getValue(0.997);
        cout << v[0] << endl;
    }



    MarkerSetPtr msp(new MarkerSet);
    for (int i = 0; i < markersCount; i++) {
        MarkerChannelPtr m(new MarkerChannel(perSecond));
        for (int j = 0; j < perSecond; j++) {
            osg::Vec3 t(j / 1000.0, j / 500.0, j / 33.0);
            m->addPoint(t);
        }
        Vec3 val = m->getValue(0.997);
        cout << i << "->" << val[0] << endl;

        msp->addMarker(m);
    }

    cout << "scale: " << msp->getScale() << endl;
    for (int i = 0; i < markersCount; i++) {
        Vec3 v =  msp->getPosition(i, 1);
        cout << i << " : " << v[0] << endl;
    }
    cout << "test" << endl;
    system("pause");
    return 0;
}