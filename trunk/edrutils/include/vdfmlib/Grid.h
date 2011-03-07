/********************************************************************
        created:  2010/11/23
        created:  23:11:2010   13:45
        filename: Grid.h
        author:   Piotr Gwiazdowski
        
        purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__GRID_H__
#define __HEADER_GUARD__GRID_H__

#include <osgWidget/Window>
#include <osgWidget/Table>
#include <vdfmlib/macroHacks.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgUI {
////////////////////////////////////////////////////////////////////////////////

/**
 *      Usprawnienia w stosunku do osgWidget::Table:
 *  - zmienne wymiary
 *  - definiowane wagi wierszy/kolumn, dzi�ki kt�rym mo�na konfigurowa�

 *    spos�b zmiany rozmiaru
 */
class Grid : public osgWidget::Table
{
private:
    CellSizes columnsWeights;
    CellSizes rowsWeights;
    bool dirtyMode;

public:
    META_Window(osgUI, Grid);
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

    //! Nadaje wszystkim kom�rkom z flag� canFill zerowy rozmiar, na nast�pnie wywo�uje resize().
    void resetFillables();
   
    //! Dopasowuje rozmiar kom�rek tak, aby zgodnie z zadanym wsp�czynnikiem proporcji
    //! kom�rki zajmowa�y jak najwi�kszy obszar.
    //! Wype�nia jedynkami rows/columnsWeights!
    void adjustDimensions(osgWidget::point_type width, osgWidget::point_type height, const std::vector<osgWidget::point_type>& aspectRatios);

    //! Wype�nia puste kom�rki grida przekazanym widgetem.
    //! \param blank Widget do wype�niania. NULL spowoduje stworzenie w�asnego widgeta o zerowym rozmiarze z flag� canFill. Gdy nie jest NULL
    //!        do grida dok�adanego s� jego klony (dlatego wska�nik jest ref_ptr)
    void fillEmpty( osg::ref_ptr<osgWidget::Widget> prototype = osg::ref_ptr<osgWidget::Widget>() );

    //! Ustawia wszystkie widgety
    void flattenHorizontally();
    void flattenVertically();

    //! \return
    bool isDirtyMode() const
    { 
        return dirtyMode;
    }
    //! \param dirtyMode
    void setDirtyMode(bool dirtyMode);

    // JESZCZE NIE ZAIMPLEMENTOWANE
    //! Usuwa wiersz.
    //! \param row
    bool removeRow(unsigned row);
    //! Usuwa kolumn�.
    //! \param column
    bool removeColumn(unsigned column);
    //! Wstawia wiersz.
    //! \param row
    bool insertRow(unsigned row);
    //! Wstawia kolumn�. 
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