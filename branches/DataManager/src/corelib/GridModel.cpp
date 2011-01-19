#include "CorePCH.h"
#include <core/GridModel.h>
#include <boost/optional.hpp>
#include <osgWidget/Window>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

GridModel::GridModel()
:   numItems(0),
    aspectRatio(1.0)
{
}

GridModel::Item GridModel::getItem(int idx)
{
    if ( dimensions.x() == 0 || dimensions.y() == 0 ) 
    {
        Item result = { 0, 0, 1, 1 };
        return result;
    }
    // pozycja
    int row = idx / dimensions.x();
    int column = idx % dimensions.x();

    // obliczenie maksymalnego rozmiaru przypadaj¹cego na komórkê
    Size corner;
    corner.x() = cellSize.x() * column;
    corner.y() = cellSize.y() * row;
    corner += (cellSize - itemSize) / 2.0;

    // pozycja
    Item result;
    result.x = static_cast<int>(corner.x());
    result.y = static_cast<int>(corner.y());
    result.width = static_cast<int>(itemSize.x());
    result.height = static_cast<int>(itemSize.y());
    return result;
}

//!
void GridModel::setSize(Size size)
{
    this->size = Size(size.x(), size.y());
    updateLayout();
}

GridModel::Size GridModel::applyAspectRatio( Size size ) 
{
    double ratio = size.x() / size.y();
    if ( aspectRatio > ratio ) 
    {
        size.y() = size.x() / aspectRatio;
    } 
    else 
    {
        size.x() = size.y() * aspectRatio;
    }
    return size;
}

void GridModel::updateLayout()
{
    // obliczenie optymalnego rozmiaru
    // TODO: da siê to zapewne zrobiæ analitycznie, ale na razie
    // mamy tutaj prosty algorytm empiryczny
    osg::Vec2s optimalDimensions(0, 0);
    double optimalField = 0.0;
    for (int r = 1; r <= getNumItems(); ++r)
    {
        int c = static_cast<int>(ceil(static_cast<double>(getNumItems()) / r));
        // obliczenie maksymalnego rozmiaru przypadaj¹cego na komórkê
        Size newCellSize = Size( size.x()/c, size.y()/r );
        Size newItemSize = applyAspectRatio( newCellSize );
        // czy zajêto ³¹cznie wiêcej miejsca ni¿ poprzednio?
        double usedField = newItemSize.x() * newItemSize.y();
        if ( usedField > optimalField ) 
        {
            optimalField = usedField;
            optimalDimensions = osg::Vec2s(c, r);
        }
    }
    // ustawienie rozmiaru
    dimensions = optimalDimensions;
    cellSize = Size( size.x() / dimensions.x(), size.y() / dimensions.y() );
    itemSize = applyAspectRatio( cellSize );
}

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////
