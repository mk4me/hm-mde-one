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

namespace htmlGen {

//! Wzorzec klasy z danymi do wy�wietlania - najcz�ciej s� to ci�gi znak�w: stD::String lub QString
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
	//! Konstruktor domy�lny
    DataTable() : rows_(0), columns_(0)
    {

    }

	//! Konstruktor
	//! \param rows Ilo�c wierszy tabeli danych
	//! \param column Ilo�� kolumn tabeli danych
    DataTable(int rows, int columns) : rows_(rows), columns_(columns)
    {
        UTILS_ASSERT(rows > 0, "Wrong rows count");
        UTILS_ASSERT(columns > 0, "Wrong columns count");
        rebuildContent();
    }

	//! Konstruktor kopiuj�cy
	//! \param htmlTableContent Tabela do skopiowania
    DataTable(const DataTable & htmlTableContent) : rows_(htmlTableContent.rows_), columns_(htmlTableContent.columns_), content_(htmlTableContent.content_)
    {

    }

	//! Destruktor
    ~DataTable() {}

	//! \param rows Ilo�� wierszy tabeli
    void setRows(int rows)
    {
        UTILS_ASSERT(rows > 0, "Wrong rows count");
        rows_ = rows;
        rebuildContent();
    }

	//! \param columns Ilo�� kolumn tabeli
    void setColumns(int columns)
    {
        UTILS_ASSERT(columns > 0, "Wrong columns count");
        columns_ = columns;
        for(int i = 0; i < rows_; ++i){
            content_[i].resize(columns_);
        }
    }

	//! \param rows Ilo�� wierszy tabeli
	//! \param columns Ilo�� kolumn tabeli
    void setDimensions(int rows, int columns)
    {
        UTILS_ASSERT(rows > 0, "Wrong rows count");
        UTILS_ASSERT(columns > 0, "Wrong columns count");

        rows_ = rows;
        columns_ = columns;

        rebuildContent();
    }

	//! \return Ilo�� wierszy tabeli
    int rows() const
    {
        return rows_;
    }
	//! \return Ilo�� kolumn tabeli
    int columns() const
    {
        return columns_;
    }

	//! \param row Wiersz kt�ry ustawiamy
	//! \param rowContent Zawarto�� wiersza kt�r� ustawiamy
    void setRow(int row, const Row & rowContent)
    {
        UTILS_DEBUG(row >= 0 && row < rows_, "Row out of range");
        int columnsToCopy = std::max(columns_, rowContent.size());
        std::copy(rowContent.begin(), rowContent.begin() + columnsToCopy, content_[row].begin());
    }
	//! \param column Kolumna kt�ry ustawiamy
	//! \param columnContent Zawarto�� kolumny kt�r� ustawiamy
    void setColumn(int column, const Column & columnContent)
    {
        UTILS_DEBUG(column >= 0 && column < columns_, "Column out of range");
        int rowsToCopy = std::max(rows_, columnContent.size());
        for(int i = 0; i < rowsToCopy; ++i){
            content_[i][column] = columnContent[i];
        }
    }

	//! \param row Wiersz kt�ry czy�cimy
	//! \param val Warto�� kt�r� ustawiamy w ca�ym wierszu
    void clearRow(int row, const T & val = T())
    {
        UTILS_DEBUG(row >= 0 && row < rows_, "Row out of range");
        std::fill(content_[row].begin(), content_[row].end(), val);
    }
	//! \param column Kolumna kt�r� czy�cimy
	//! \param val Warto�� kt�r� ustawiamy w ca�ej kolumnie
    void clearColumn(int column, const T & val = T())
    {
        UTILS_DEBUG(column >= 0 && column < columns_, "Column out of range");
        for(int i = 0; i < rows_; ++i){
            content_[i][column] = val;
        }
    }

	//! \param row Wiersz
	//! \param column Kolumna
	//! \return Warto�� kom�rki o zadanych wsp�rz�dnych
    const T & cell(int row, int column) const
    {
        UTILS_ASSERT(row >= 0 && row < rows_, "Row out of range");
        UTILS_ASSERT(column >= 0 && column < columns_, "Column out of range");
        return content_[row][column];
    }
	//! \param row Wiersz
	//! \param column Kolumna
	//! \return Warto�� kom�rki o zadanych wsp�rz�dnych
    T & cell(int row, int column)
    {
        UTILS_ASSERT(row >= 0 && row < rows_, "Row out of range");
        UTILS_ASSERT(column >= 0 && column < columns_, "Column out of range");
        return content_[row][column];
    }
	//! \param row Wiersz
	//! \param column Kolumna
	//! \param cellContent Zawarto�� kom�rki do ustawienia
    void setCell(int row, int column, const T & cellContent)
    {
        UTILS_ASSERT(row >= 0 && row < rows_, "Row out of range");
        UTILS_ASSERT(column >= 0 && column < columns_, "Column out of range");
        content_[row][column] = cellContent;
    }

	//! \param row Wiersz
	//! \param column Kolumna
	//! \param val Warto�� kt�r� czy�cimy kom�rke
    void clearCell(int row, int column, const T & val = T())
    {
        UTILS_ASSERT(row >= 0 && row < rows_, "Row out of range");
        UTILS_ASSERT(column >= 0 && column < columns_, "Column out of range");
        content_[row][column] = val;
    }

	//! \param row Wiersz
	//! \return Zawarto�� wiersza
    const Row & row(int row) const
    {
        UTILS_ASSERT(row >= 0 && row < rows_, "Row out of range");
        return content_[row];
    }
	//! \param column Kolumna
	//! \return Zawarto�� kolumny
    const Column column(int column) const
    {
        UTILS_ASSERT(column >= 0 && column < columns_, "Column out of range");
        Column col;

        for(int i = 0; i < rows_; ++i){
            col.push_back(content_[i][column]);
        }

        return col;
    }
	//! \return Zawarto�� tabeli danych
    const TableContent & content() const
    {
        return content_;
    }

private:
	//! Fizycznie zmiania rozmiar tablicy
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

//! Typ opisuj�cy rodzaj zadawania wymiar�w
enum HtmlWidthType {
    WRelative,
    WAbsolute
};

//! Typ opisuj�cy wymiary
struct HtmlWidth
{
	//! Wymiar
    float width;
	//! Spos�b zadawania wymiaru
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

//! Klasa do zarz�dzania atrybutami kom�rek tabeli HTML
class HtmlCellAttributes
{
public:
	//! Konstruktor domy�lny
    HtmlCellAttributes();
	//! Destruktor
    ~HtmlCellAttributes();

public:
	//! \param html [out]Dokument do kt�rego generujemy atrybut w postaci tag�w HTML
    void generateHtmlAttributes(QString & html) const;
	//! \param width Rozmiar elementu
	//! \param typ Spos�b zadawania rozmiaru
    void setWidth(float width, HtmlWidthType type);
	//! Czy�ci atrybut wymiaru
    void clearWidth();
	//! \param bgColor Kolor t�a kom�rki
    void setBgColor(const QString & bgColor);
	//! Czy�ci atrybut koloru t�a
    void clearBgColor();
	//! \param colSpan Ilo�� kolumn na kt�re kom�rka b�dzie rozszerzana
    void setColSpan(CellSpan colSpan);
	//! Czy�ci atrybut ilo�ci kolumn na kt�re kom�rka b�dzie rozszerzana
    void clearColSpan();
	//! \param rowSpan Ilo�� wierszy na kt�re kom�rka b�dzie rozszerzana
    void setRowSpan(CellSpan rowSpan);
	//! Czy�ci atrybut ilo�ci wierszy na kt�re kom�rka b�dzie rozszerzana
    void clearRowSpan();
	//! \param hAlign Wyr�wnanie zawarto�ci kom�rki w poziomie
    void setHAlign(HAlign hAlign);
	//! Czy�ci atrybut wyr�wnania zawarto�ci kom�rki w poziomie
    void clearHAlign();
	//! \param vAlign Wyr�wnanie zawarto�ci kom�rki w pionie
    void setVAlign(VAlign vAlign);
	//! Czy�ci atrybut wyr�wnania zawarto�ci kom�rki w pionie
    void clearVAlign();

private:
	//! Mapa atrubut�w
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

//! Klasa obs�uguj�ca atrybuty tabeli
class HtmlTableAttributes
{
public:

    HtmlTableAttributes();
    ~HtmlTableAttributes();

public:
	//! \param html [out]Dokument do kt�rego generujemy atrybut w postaci tag�w HTML
    void generateHtmlAttributes(QString & html) const;
	//! \param width Szeroko�� tabeli
	//! \param typ Spos�b zadawania rozmiaru
    void setWidth(float width, HtmlWidthType type);
	//! Czy�ci atrybut szeroko�ci
    void clearWidth();
	//! \param height Wysoko�c tabeli
	//! \param typ Spos�b zadawania rozmiaru
	void setHeight(float height, HtmlWidthType type);
	//! Czy�ci atrybut wysoko�ci
    void clearHeight();
	//! \param bgColor Kolor t�a tabeli
    void setBgColor(const QString & bgColor);
	//! Czy�ci atrybut koloru t�a
    void clearBgColor();
	//! \param border Szeroko�� ramki tabeli i kom�ek
	void setBorder(int border);
	//! Czy�ci atrybut szeroko�ci ramki tabeli i kom�ek
    void clearBorder();
	//! \param spacing Dodatkowa ilo�� pixeli wok� w�a�ciwej zawarto�ci kom�rki
    void setCellSpacing(int spacing);
	//! Czy�ci atrybut dodatkowej ilo�ci pixeli wok� w�a�ciwej zawarto�ci kom�rki
    void clearCellSpacing();
	//! \param spacing Odst�p pomi�dzy kom�rkami w tabeli
    void setCellPadding(int padding);
	//! Czy�ci atrybut odst�pu pomi�dzy kom�rkami w tabeli
    void clearCellPadding();

private:
	//! Mapa atrybut�w tabeli
    std::map<HtmlTableAttribute, boost::any> attributes;
};

//! Typ tabeli opisuj�cej atrybuty kom�rek tabeli HTML
typedef DataTable<HtmlCellAttributes> HtmlDataTableCellAttributes;

//! Typ elementu tabeli - wiersza, kolumny, kom�rki
enum StyleStatus {
    Single  = 0, //! Pojedynczy element
    First   = 1, //! Pierwszy element
    Middle  = 2, //! �rodkowy element
    Last    = 3  //! Ostatni element
};

//! \param value warto�� dla kt�rej wyznaczmy typ
//! \param min Minimalna warto�� zakresu
//! \param max Maksymalna warto�c zakresu
//! \return Typ elementu: Single -> min = max = value, First -> value = min, min != max, Middle -> value != min, value != max, min != max, Last -> value = max, min != max
StyleStatus styleStatus(int value, int min, int max);

//! Typ opisuj�cy style wierszy dla ich status�w
typedef QString HtmlRowStyles[4];
//! Typ opisuj�cy style kolumn dla ich status�w
typedef QString HtmlColumnStyles[4];

//! Typ opisuj�cy style kom�rek zale�nie od ich status�w
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
	//! Metoda generuje/wype�nia/styluje nag��wki tabeli
	//! \param content Zawarto�c tabeli kt�r� uzupe�niamy
	//! \param cellAttributes Atrybuty kom�rek
	//! \param rowsData Dane wiersza
	//! \param structure Uk�ad tabeli - ilo�c nag��wk�w...
	//! \param styles Style tabeli
	//! \param rows Ilo�� wierszy kt�re b�d� brane pod uwag�
	//! \param columns Ilo�� kolumn kt�re b�d� brane pod uwag�
    static void buildHeaders(HtmlDataTableContent & content, const HtmlDataTableCellAttributes & cellAttributes,
        HtmlDataTableContent::Column & rowsData, const TableHeadersStructureDescriptor & structure,
        const HtmlDataTableStyles & styles, int rows, int columns);

	//! Metoda generuje zawarto�� kom�rki tabeli
	//! \param content [out] Zawarto�� kom�rki
	//! \param style Styl kom�rki
	//! \param cellAttributes Atrybuty kom�rki
    static void buildCell(QString & content, const QString & style = QString(), const QString & cellAtributes = QString());

	//! Metoda generuje zawarto�� tabeli (dane, nie nag��wki)
    static void buildContent(HtmlDataTableContent & content, const HtmlDataTableCellAttributes & cellAttributes,
        const TableHeadersStructureDescriptor & structure, const HtmlDataTableStyles & styles, int rows, int columns);

	//! Metoda otwiera znaczniki HTML tabeli
	//! \param table [out] Dokument HTML do kt�rego generujemy tabel�
	//! \param style Styl tabeli
	//! \param attributes Atrybuty tabeli
    static void openTable(QString & table, const QString & style, const HtmlTableAttributes & attributes);
	//! Metodad zamyka znaczniki HTML tabeli
	//! \param table [out] Dokument HTML do kt�rego generujemy tabel�
    static void closeTable(QString & table);
	//! Metoda otwiera znaczniki HTML wiersza tabeli
	//! \param table [out] Dokument HTML do kt�rego generujemy tabel�
	//! \param style Styl wiersza
    static void openRow(QString & table, const QString & style);
	//! Metodad zamyka znaczniki HTML wiersza tabeli
	//! \param table [out] Dokument HTML do kt�rego generujemy tabel�
    static void closeRow(QString & table);
};

typedef boost::array<boost::array<std::pair<QTextTableCellFormat, QTextBlockFormat>,4>,4> TableCellsFormat;

//! Kalsa pomocnicza przy wype�nianiu tabeli wg standard�w Qt
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

}
#endif	//	HEADER_GUARD___TABLEGENERATOR_H__
