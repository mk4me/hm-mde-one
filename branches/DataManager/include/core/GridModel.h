/********************************************************************
	created:  2010/11/02
	created:  2:11:2010   13:09
	filename: GridModel.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__MULTIVIEW_H__
#define __HEADER_GUARD__MULTIVIEW_H__


////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

class GridModel
{
public:
    //! Definicja dok³adnego rozmiaru u¿ywanego przez kontrolkê.
    typedef osg::Vec2 Size;
    
    struct Item
    {
        int x;
        int y;
        int width;
        int height;
    };

private:
    //!
    int numItems;
    //!
    double aspectRatio;

    //! Wymiary.
    osg::Vec2s dimensions;
    //! Rozmiar grida.
    Size size;
    //! Rozmiar komórki.
    Size cellSize;
    //! Rozmiar itema w komórce.
    Size itemSize;

public:

    GridModel();

    //! 
    //! \param idx
    Item getItem(int idx);

    //! \param width
    //! \param height
    void setSize(Size::value_type width, Size::value_type height)
    {
        setSize( Size(width, height) );
    }
    //! \param size
    void setSize(Size size);
    //! \return
    Size getSize() const
    {
        return size;
    }

    //! 
    Size getItemSize() const
    {
        return itemSize;
    }

    //! \return
    double getAspectRatio() const
    { 
        return aspectRatio;
    }
    //! \param aspectRatio
    void setAspectRatio(double aspectRatio) 
    { 
        this->aspectRatio = aspectRatio; 
    }

    //! 
    int getNumItems() const
    {
        return numItems;
    }
    //! \param numItems
    void setNumItems(int numItems)
    {
        this->numItems = numItems;
        updateLayout();
    }

    //! \return
    int getNumRows() const
    { 
        return static_cast<int>(dimensions.y());
    }
    //! \return
    int getNumColumns() const
    { 
        return static_cast<int>(dimensions.x());
    }


private:

    //! 
    //! \param maxWidth
    //! \param maxHeight
    Size applyAspectRatio( Size size );
    //! Aktualizuje rozk³ad grida tak, aby elementy zajmowa³y jak najwiêcej miejsca.
    void updateLayout();
};


////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__MULTIVIEW_H__