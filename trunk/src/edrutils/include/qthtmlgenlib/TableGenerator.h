/********************************************************************
    created:  2012/03/01
    created:  1:3:2012   9:25
    filename: TableGenerator.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___TABLEGENERATOR_H__
#define HEADER_GUARD___TABLEGENERATOR_H__

#include <vector>
#include <map>
#include <QtCore/QString>
#include <QtGui/QTextTable>
#include <boost/any.hpp>
#include <utils/Debug.h>
#include <boost/array.hpp>

template<class T>
class DataTable
{

public:
    //! Typ wpisu wiersza
    typedef std::vector<T> Row;
    //! Typ wpisu kolumny
    typedef Row Column;
    //! Zawarto�� tabeli
    typedef std::vector<Row> TableContent;

public:

    DataTable() : rows_(0), columns_(0)
    {

    }

    DataTable(int rows, int columns) : rows_(rows), columns_(columns)
    {
        UTILS_ASSERT(rows > 0, "Wrong rows count");
        UTILS_ASSERT(columns > 0, "Wrong columns count");
        rebuildContent();
    }

    DataTable(const DataTable & htmlTableContent) : rows_(htmlTableContent.rows_), columns_(htmlTableContent.columns_), content_(htmlTableContent.content_)
    {

    }

    ~DataTable() {}

    void setRows(int rows)
    {
        UTILS_ASSERT(rows > 0, "Wrong rows count");
        rows_ = rows;
        rebuildContent();
    }

    void setColumns(int columns)
    {
        UTILS_ASSERT(columns > 0, "Wrong columns count");
        columns_ = columns;
        for(int i = 0; i < rows_; ++i){
            content_[i].resize(columns_);
        }
    }

    void setDimensions(int rows, int columns)
    {
        UTILS_ASSERT(rows > 0, "Wrong rows count");
        UTILS_ASSERT(columns > 0, "Wrong columns count");

        rows_ = rows;
        columns_ = columns;

        rebuildContent();
    }

    int rows() const
    {
        return rows_;
    }

    int columns() const
    {
        return columns_;
    }

    void setRow(int row, const Row & rowContent)
    {
        UTILS_DEBUG(row >= 0 && row < rows_, "Row out of range");
        int columnsToCopy = std::max(columns_, rowContent.size());
        std::copy(rowContent.begin(), rowContent.begin() + columnsToCopy, content_[row].begin());
    }

    void setColumn(int column, const Column & columnContent)
    {
        UTILS_DEBUG(column >= 0 && column < columns_, "Column out of range");
        int rowsToCopy = std::max(rows_, columnContent.size());
        for(int i = 0; i < rowsToCopy; ++i){
            content_[i][column] = columnContent[i];
        }
    }

    void clearRow(int row)
    {
        UTILS_DEBUG(row >= 0 && row < rows_, "Row out of range");
        std::fill(content_[row].begin(), content_[row].end(), T());
    }

    void clearColumn(int column)
    {
        UTILS_DEBUG(column >= 0 && column < columns_, "Column out of range");
        for(int i = 0; i < rows_; ++i){
            content_[i][column] = T();
        }
    }

    const T & cell(int row, int column) const
    {
        UTILS_ASSERT(row >= 0 && row < rows_, "Row out of range");
        UTILS_ASSERT(column >= 0 && column < columns_, "Column out of range");
        return content_[row][column];
    }

    T & cell(int row, int column)
    {
        UTILS_ASSERT(row >= 0 && row < rows_, "Row out of range");
        UTILS_ASSERT(column >= 0 && column < columns_, "Column out of range");
        return content_[row][column];
    }

    void setCell(int row, int column, const T & cellContent)
    {
        UTILS_ASSERT(row >= 0 && row < rows_, "Row out of range");
        UTILS_ASSERT(column >= 0 && column < columns_, "Column out of range");
        content_[row][column] = cellContent;
    }

    void clearCell(int row, int column)
    {
        UTILS_ASSERT(row >= 0 && row < rows_, "Row out of range");
        UTILS_ASSERT(column >= 0 && column < columns_, "Column out of range");
        content_[row][column] = QString();
    }

    const Row & row(int row) const
    {
        UTILS_ASSERT(row >= 0 && row < rows_, "Row out of range");
        return content_[row];
    }

    const Column column(int column) const
    {
        UTILS_ASSERT(column >= 0 && column < columns_, "Column out of range");
        Column col;

        for(int i = 0; i < rows_; ++i){
            col.push_back(content_[i][column]);
        }

        return col;
    }

    const TableContent & content() const
    {
        return content_;
    }

private:

    void rebuildContent()
    {
        content_.resize(rows_);
        for(int i = 0; i < rows_; ++i){
            content_[i].resize(columns_);
        }
    }

private:
    //! Ilo�� wierszy
    int rows_;
    //! Ilo�� kolumn
    int columns_;
    //! Zawarto�� tabeli
    TableContent content_;
};

typedef DataTable<QString> HtmlDataTableContent;

//! Typ do rozszerzania kom�rek
typedef int CellSpan;

//! Typ wyr�wnania zawarto�ci kom�rek w poziomie
enum HAlign {
    HALeft,
    HARight,
    HACenter,
    HAJustify
};

//! Typ wyr�wnania zawarto�ci kom�rek w pionie
enum VAlign {
    VATop,
    VAMiddle,
    VABottom
};

enum HtmlWidthType {
    WRelative,
    WAbsolute
};

struct HtmlWidth
{
    float width;
    HtmlWidthType type;
};

//! Lista atrybut�w kom�rki
enum HtmlCellAttribute {
    CellWidth,      //! Szeroko��
    CellBgColor,    //! Kolor t�a
    CellColSpan,    //! Na ile kom�rek w pionie rozszerzamy
    CellRowSpan,    //! Na ile kom�rek w poziomie rozszerzamy
    CellHAlign,     //! Wyr�wnanie tekstu kom�rki w poziomie
    CellVAlign,     //! Wyr�wnanie tekstu w pionie
};

class HtmlCellAttributes
{
public:

    HtmlCellAttributes();
    ~HtmlCellAttributes();

public:

    void generateHtmlAttributes(QString & html) const;

    void setWidth(float width, HtmlWidthType type);

    void clearWidth();

    void setBgColor(const QString & bgColor);

    void clearBgColor();

    void setColSpan(CellSpan colSpan);

    void clearColSpan();

    void setRowSpan(CellSpan rowSpan);

    void clearRowSpan();

    void setHAlign(HAlign hAlign);

    void clearHAlign();

    void setVAlign(VAlign vAlign);

    void clearVAlign();

private:

    std::map<HtmlCellAttribute, boost::any> attributes;
};

//! Lista atrybut�w tabeli
enum HtmlTableAttribute {
    TableWidth,			//! Szeroko��
	TableHeight,		//! Wysoko��
	TableBorder,		//! Grubo�� ramki
    TableBgColor,		//! Kolor t�a
    TableCellSpacing,	//! Odst�p pomi�dzy kom�rkami
    TableCellPadding    //! Padding kom�rek
};

class HtmlTableAttributes
{
public:

    HtmlTableAttributes();
    ~HtmlTableAttributes();

public:

    void generateHtmlAttributes(QString & html) const;

    void setWidth(float width, HtmlWidthType type);

    void clearWidth();

	void setHeight(float width, HtmlWidthType type);

    void clearHeight();

    void setBgColor(const QString & bgColor);

    void clearBgColor();

	void setBorder(int border);

    void clearBorder();

    void setCellSpacing(int spacing);

    void clearCellSpacing();

    void setCellPadding(int padding);

    void clearCellPadding();

private:

    std::map<HtmlTableAttribute, boost::any> attributes;
};

typedef DataTable<HtmlCellAttributes> HtmlDataTableCellAttributes;

enum StyleStatus {
    Single  = 0, //! Pojedynczy element
    First   = 1, //! Pierwszy element
    Middle  = 2, //! �rodkowy element
    Last    = 3  //! Ostatni element
};

StyleStatus styleStatus(int value, int min, int max);

typedef QString HtmlRowStyles[4];
typedef QString HtmlColumnStyles[4];

typedef QString HtmlStyles[4][4];

//! Agregat styli kom�rek
struct HtmlDataTableStyles
{
    //! Styl tabeli
    QString tableStyle_;

	//! Atrybuty tabeli
	HtmlTableAttributes tableAttributes;

    //! Styl wierszy gdy poziome na��wki
    HtmlRowStyles headerRowStyle_;

    //! Style wierszy danych
    HtmlRowStyles contentRowStyle_;

    //! Macierz styli kom�rek nag��wk�w
    HtmlStyles headersStyles;

    //! Macierz styli kom�rek danych
    HtmlStyles contentStyles;
};

//Opisuje struktur� nag��wk�w tabeli
struct TableHeadersStructureDescriptor {
    //! Ilo�� wierszy nag��wkowych
    int headerRows;
    //! Ilos� kolumn nag��wkowych
    int headerColumns;
};

//! Klasa generuj�ca tabele HTML z danymi
class HtmlDataTableGenerator
{
public:
    //! \param content Zawarto�� tabeli wiersz-kolumna
    //! \param structure Struktura tabeli
    //! \param styles Style poszczeg�lnycvh kom�rek
    //! \param rows Ca�kowita ilo�� wierszy tabeli (utniemy lub dodamy je�li b�dzie trzeba
    //! \param columns Ca�kowita ilo�� kolumn tabeli (utniemy lub dodamy je�li b�dzie trzeba
    static void generateHtmlTable(QString & table, const HtmlDataTableContent & content, TableHeadersStructureDescriptor structure,
        const HtmlDataTableCellAttributes & cellAttributes = HtmlDataTableCellAttributes(), const HtmlDataTableStyles & styles = HtmlDataTableStyles(), int rows = 0, int columns = 0);

private:

    static void buildHeaders(HtmlDataTableContent & content, const HtmlDataTableCellAttributes & cellAttributes,
        HtmlDataTableContent::Column & rowsData, const TableHeadersStructureDescriptor & structure,
        const HtmlDataTableStyles & styles, int rows, int columns);

    static void buildCell(QString & content, const QString & style = QString(), const QString & cellAtributes = QString());

    static void buildContent(HtmlDataTableContent & content, const HtmlDataTableCellAttributes & cellAttributes,
        const TableHeadersStructureDescriptor & structure, const HtmlDataTableStyles & styles, int rows, int columns);

    static void openTable(QString & table, const QString & style, const HtmlTableAttributes & attributes);

    static void closeTable(QString & table);

    static void openRow(QString & table, const QString & style);

    static void closeRow(QString & table);
};

typedef boost::array<boost::array<std::pair<QTextTableCellFormat, QTextBlockFormat>,4>,4> TableCellsFormat;

class QTextTableViewHelper
{
public:
	QTextTableViewHelper();
	~QTextTableViewHelper();

	void setHeadersCellsFormat(const TableCellsFormat & format);
	void setDataCellsFormat(const TableCellsFormat & format);
	void setTableFormat(const QTextTableFormat & format);

	void setHeaderRows(int headerRows);
	void setHeaderColumns(int headerColumns);

	const TableCellsFormat & headersCellsFormat() const;
	const TableCellsFormat & dataCellsFormat() const;
	const QTextTableFormat & tableFormat() const;

	int headerRows() const;
	int headerColumns() const;

	void update(QTextTable * table);
	void updateContent(QTextTable * table);
	void updateHeaders(QTextTable * table);
	void updateTable(QTextTable * table);

private:

	int headerRows_;
	int headerColumns_;

	TableCellsFormat headersCellsFormat_;
	TableCellsFormat dataCellsFormat_;
	QTextTableFormat tableFormat_;
};

#endif	//	HEADER_GUARD___TABLEGENERATOR_H__
