/********************************************************************
	created:	2012/06/05
	created:	5:6:2012   14:28
	filename: 	MultiserieColorStrategy.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__MULTISERIECOLORSTRATEGY_H__
#define HEADER_GUARD_HMM__MULTISERIECOLORSTRATEGY_H__

#include <QtGui/QColor>

//! interfejs realizujacy strategie do zmiany kolorow multiserii
class IMultiserieColorStrategy
{
public:
    virtual ~IMultiserieColorStrategy() {}

public:
    //! Na podstawie serii i wrappera dobierany jest kolor
    //! \return kolor dla dajnej serii
    virtual QColor getColor(INewChartSerie* s, core::ObjectWrapperConstPtr ow) const = 0;
};
typedef core::shared_ptr<IMultiserieColorStrategy> IMultiserieColorStrategyPtr;
typedef core::shared_ptr<const IMultiserieColorStrategy> IMultiserieColorStrategyConstPtr;

//! klasa dostarcza losowych kolorow dla serii
class RandomMultiserieColorStrategy : public IMultiserieColorStrategy
{
public:
    QColor getColor(INewChartSerie* s, core::ObjectWrapperConstPtr w) const
    {
        return QColor(rand() % 256, rand() % 256, rand() % 256);
    }
};

//! wykorzystywany jest kolor przypisany do serii
class CopyColorMultiserieColorStrategy : public IMultiserieColorStrategy
{
public:
    QColor getColor(INewChartSerie* s, core::ObjectWrapperConstPtr w) const
    {
        return s->getColor();
    }
};

//! podawane s¹ dwa kolory, strategia dostarcza losowy kolor interpolowany pomiêdzy nimi
class RandomBetweenMultiserieColorStrategy : public IMultiserieColorStrategy
{
public:
    RandomBetweenMultiserieColorStrategy(QColor c1, QColor c2) : c1(c1), c2(c2) {}

    QColor getColor(INewChartSerie* s, core::ObjectWrapperConstPtr w) const
    {
        float r = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        return QColor(
            lerp(c1.red(), c2.red(), r),
            lerp(c1.green(), c2.green(), r),
            lerp(c1.blue(), c2.blue(), r)
            );
    }

private:
    int lerp(int from, int to, float r) const
    {
        return static_cast<int>(from * (1.0f - r) + to * r);
    }
    QColor c1, c2;
};

//! kazdy wrapper dostaje kolor przypisany z zewnatrz
class ColorMapMultiserieStrategy : public IMultiserieColorStrategy
{
public:
    ColorMapMultiserieStrategy(const std::map<core::ObjectWrapperConstPtr, QColor>& colorMap) : colorMap(colorMap) {}
    virtual QColor getColor(INewChartSerie* s, core::ObjectWrapperConstPtr w) const 
    {
        auto it = colorMap.find(w);
        if (it != colorMap.end()) {
            return it->second;
        }

        return QColor();
    }

private:
    std::map<core::ObjectWrapperConstPtr, QColor> colorMap;
};


#endif
