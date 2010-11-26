/********************************************************************
	created:  2010/11/23
	created:  23:11:2010   13:45
	filename: Grid.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__GRID_H__
#define __HEADER_GUARD__GRID_H__

#include <osgWidget/Window>
#include <osgWidget/Table>

////////////////////////////////////////////////////////////////////////////////
namespace osgUI {
////////////////////////////////////////////////////////////////////////////////

/**
 *	Usprawnienia w stosunku do osgWidget::Table:
 *  - zmienne wymiary
 *  - definiowane wagi wierszy/kolumn, dziêki którym mo¿na konfigurowaæ
 *    sposób zmiany rozmiaru
 */
class Grid : public osgWidget::Table
{
private:
    CellSizes columnsWeights;
    CellSizes rowsWeights;

public:
    META_Object(osgUI, Grid)
    //! \param name
    //! \param width
    //! \param height
    Grid(const std::string& name = "", unsigned rows = 0, unsigned columns = 0);
    //! \param grid
    //! \param copyop
    Grid(const Grid& grid, const osg::CopyOp& copyop);

public:
    //! \param row
    osgWidget::point_type getRowWeight(unsigned row) const;
    //! \param row
    //! \param weight
    void setRowWeight(unsigned row, osgWidget::point_type weight);
    //! \return
    void getRowsWeights(CellSizes& weights) const;

    //! \param row
    osgWidget::point_type getColumnWeight(unsigned column) const;
    //! \param row
    //! \param weight
    void setColumnWeight(unsigned column, osgWidget::point_type weight);
    //! \return
    void getColumnsWeights(CellSizes& weights) const;

    //! \return Liczba wierszy.
    unsigned getNumRows() const
    {
        return _rows;
    }
    //! \param rows Liczba wierszy.
    void setNumRows(unsigned rows);

    //! \return Liczba kolumn.
    unsigned getNumColumns() const
    {
        return _cols;
    }
    //! \param columns Liczba kolumn.
    void setNumColumns(unsigned columns);
    
    //! \param rows Liczba wierszy.
    //! \param columns Liczba kolumn.
    void setDimensions(unsigned rows, unsigned columns);

    //! Nadaje wszystkim komórkom z flag¹ canFill zerowy rozmiar, na nastêpnie wywo³uje resize().
    void resetFillable();
   
    //! Dopasowuje rozmiar komórek tak, aby zgodnie z zadanym wspó³czynnikiem proporcji
    //! komórki zajmowa³y jak najwiêkszy obszar.
    void adjustDimensions(osgWidget::point_type width, osgWidget::point_type height, osgWidget::point_type aspectRatio = -1);

    // JESZCZE NIE ZAIMPLEMENTOWANE
    //! Usuwa wiersz.
    //! \param row
    bool removeRow(unsigned row);
    //! Usuwa kolumnê.
    //! \param column
    bool removeColumn(unsigned column);
    //! Wstawia wiersz.
    //! \param row
    bool insertRow(unsigned row);
    //! Wstawia kolumnê. 
    //! \param column
    bool insertColumn(unsigned column);

protected:
    //! 
    //! \param diffWidth
    //! \param diffHeight
    virtual void _resizeImplementation(osgWidget::point_type diffWidth, osgWidget::point_type diffHeight);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace osgUI
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__GRID_H__