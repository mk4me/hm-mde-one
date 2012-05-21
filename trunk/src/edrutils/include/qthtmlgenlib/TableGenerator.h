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

//! Wzorzec klasy z danymi do wyœwietlania - najczêœciej s¹ to ci¹gi znaków: stD::String lub QString
template<class T>
class DataTable
{

public:
    //! Typ wpisu wiersza
    typedef std::vector<T> Row;
    //! Typ wpisu kolumny
    typedef Row Column;
    //! Zawartoœæ tabeli
    typedef std::vector<Row> TableContent;

public:
	//! Konstruktor domyœlny
    DataTable() : rows_(0), columns_(0)
    {

    }

	//! Konstruktor
	//! \param rows Iloœc wierszy tabeli danych
	//! \param column Iloœæ kolumn tabeli danych
    DataTable(int rows, int columns) : rows_(rows), columns_(columns)
    {
        UTILS_ASSERT(rows > 0, "Wrong rows count");
        UTILS_ASSERT(columns > 0, "Wrong columns count");
        rebuildContent();
    }

	//! Konstruktor kopiuj¹cy
	//! \param htmlTableContent Tabela do skopiowania
    DataTable(const DataTable & htmlTableContent) : rows_(htmlTableContent.rows_), columns_(htmlTableContent.columns_), content_(htmlTableContent.content_)
    {

    }

	//! Destruktor
    ~DataTable() {}

	//! \param rows Iloœæ wierszy tabeli
    void setRows(int rows)
    {
        UTILS_ASSERT(rows > 0, "Wrong rows count");
        rows_ = rows;
        rebuildContent();
    }

	//! \param columns Iloœæ kolumn tabeli
    void setColumns(int columns)
    {
        UTILS_ASSERT(columns > 0, "Wrong columns count");
        columns_ = columns;
        for(int i = 0; i < rows_; ++i){
            content_[i].resize(columns_);
        }
    }

	//! \param rows Iloœæ wierszy tabeli
	//! \param columns Iloœæ kolumn tabeli
    void setDimensions(int rows, int columns)
    {
        UTILS_ASSERT(rows > 0, "Wrong rows count");
        UTILS_ASSERT(columns > 0, "Wrong columns count");

        rows_ = rows;
        columns_ = columns;

        rebuildContent();
    }

	//! \return Iloœæ wierszy tabeli
    int rows() const
    {
        return rows_;
    }
	//! \return Iloœæ kolumn tabeli
    int columns() const
    {
        return columns_;
    }

	//! \param row Wiersz który ustawiamy
	//! \param rowContent Zawartoœæ wiersza któr¹ ustawiamy
    void setRow(int row, const Row & rowContent)
    {
        UTILS_DEBUG(row >= 0 && row < rows_, "Row out of range");
        int columnsToCopy = std::max(columns_, rowContent.size());
        std::copy(rowContent.begin(), rowContent.begin() + columnsToCopy, content_[row].begin());
    }
	//! \param column Kolumna który ustawiamy
	//! \param columnContent Zawartoœæ kolumny któr¹ ustawiamy
    void setColumn(int column, const Column & columnContent)
    {
        UTILS_DEBUG(column >= 0 && column < columns_, "Column out of range");
        int rowsToCopy = std::max(rows_, columnContent.size());
        for(int i = 0; i < rowsToCopy; ++i){
            content_[i][column] = columnContent[i];
        }
    }

	//! \param row Wiersz który czyœcimy
	//! \param val Wartoœæ któr¹ ustawiamy w ca³ym wierszu
    void clearRow(int row, const T & val = T())
    {
        UTILS_DEBUG(row >= 0 && row < rows_, "Row out of range");
        std::fill(content_[row].begin(), content_[row].end(), val);
    }
	//! \param column Kolumna któr¹ czyœcimy
	//! \param val Wartoœæ któr¹ ustawiamy w ca³ej kolumnie
    void clearColumn(int column, const T & val = T())
    {
        UTILS_DEBUG(column >= 0 && column < columns_, "Column out of range");
        for(int i = 0; i < rows_; ++i){
            content_[i][column] = val;
        }
    }

	//! \param row Wiersz
	//! \param column Kolumna
	//! \return Wartoœæ komórki o zadanych wspó³rzêdnych
    const T & cell(int row, int column) const
    {
        UTILS_ASSERT(row >= 0 && row < rows_, "Row out of range");
        UTILS_ASSERT(column >= 0 && column < columns_, "Column out of range");
        return content_[row][column];
    }
	//! \param row Wiersz
	//! \param column Kolumna
	//! \return Wartoœæ komórki o zadanych wspó³rzêdnych
    T & cell(int row, int column)
    {
        UTILS_ASSERT(row >= 0 && row < rows_, "Row out of range");
        UTILS_ASSERT(column >= 0 && column < columns_, "Column out of range");
        return content_[row][column];
    }
	//! \param row Wiersz
	//! \param column Kolumna
	//! \param cellContent Zawartoœæ komórki do ustawienia
    void setCell(int row, int column, const T & cellContent)
    {
        UTILS_ASSERT(row >= 0 && row < rows_, "Row out of range");
        UTILS_ASSERT(column >= 0 && column < columns_, "Column out of range");
        content_[row][column] = cellContent;
    }

	//! \param row Wiersz
	//! \param column Kolumna
	//! \param val Wartoœæ któr¹ czyœcimy komórke
    void clearCell(int row, int column, const T & val = T())
    {
        UTILS_ASSERT(row >= 0 && row < rows_, "Row out of range");
        UTILS_ASSERT(column >= 0 && column < columns_, "Column out of range");
        content_[row][column] = val;
    }

	//! \param row Wiersz
	//! \return Zawartoœæ wiersza
    const Row & row(int row) const
    {
        UTILS_ASSERT(row >= 0 && row < rows_, "Row out of range");
        return content_[row];
    }
	//! \param column Kolumna
	//! \return Zawartoœæ kolumny
    const Column column(int column) const
    {
        UTILS_ASSERT(column >= 0 && column < columns_, "Column out of range");
        Column col;

        for(int i = 0; i < rows_; ++i){
            col.push_back(content_[i][column]);
        }

        return col;
    }
	//! \return Zawartoœæ tabeli danych
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
    //! Iloœæ wierszy
    int rows_;
    //! Iloœæ kolumn
    int columns_;
    //! Zawartoœæ tabeli
    TableContent content_;
};

typedef DataTable<QString> HtmlDataTableContent;

//! Typ do rozszerzania komórek
typedef int CellSpan;

//! Typ wyrównania zawartoœci komórek w poziomie
enum HAlign {
    HALeft,
    HARight,
    HACenter,
    HAJustify
};

//! Typ wyrównania zawartoœci komórek w pionie
enum VAlign {
    VATop,
    VAMiddle,
    VABottom
};

//! Typ opisuj¹cy rodzaj zadawania wymiarów
enum HtmlWidthType {
    WRelative,
    WAbsolute
};

//! Typ opisuj¹cy wymiary
struct HtmlWidth
{
	//! Wymiar
    float width;
	//! Sposób zadawania wymiaru
    HtmlWidthType type;
};

//! Lista atrybutów komórki
enum HtmlCellAttribute {
    CellWidth,      //! Szerokoœæ
    CellBgColor,    //! Kolor t³a
    CellColSpan,    //! Na ile komórek w pionie rozszerzamy
    CellRowSpan,    //! Na ile komórek w poziomie rozszerzamy
    CellHAlign,     //! Wyrównanie tekstu komórki w poziomie
    CellVAlign,     //! Wyrównanie tekstu w pionie
};

//! Klasa do zarz¹dzania atrybutami komórek tabeli HTML
class HtmlCellAttributes
{
public:
	//! Konstruktor domyœlny
    HtmlCellAttributes();
	//! Destruktor
    ~HtmlCellAttributes();

public:
	//! \param html [out]Dokument do którego generujemy atrybut w postaci tagów HTML
    void generateHtmlAttributes(QString & html) const;
	//! \param width Rozmiar elementu
	//! \param typ Sposób zadawania rozmiaru
    void setWidth(float width, HtmlWidthType type);
	//! Czyœci atrybut wymiaru
    void clearWidth();
	//! \param bgColor Kolor t³a komórki
    void setBgColor(const QString & bgColor);
	//! Czyœci atrybut koloru t³a
    void clearBgColor();
	//! \param colSpan Iloœæ kolumn na które komórka bêdzie rozszerzana
    void setColSpan(CellSpan colSpan);
	//! Czyœci atrybut iloœci kolumn na które komórka bêdzie rozszerzana
    void clearColSpan();
	//! \param rowSpan Iloœæ wierszy na które komórka bêdzie rozszerzana
    void setRowSpan(CellSpan rowSpan);
	//! Czyœci atrybut iloœci wierszy na które komórka bêdzie rozszerzana
    void clearRowSpan();
	//! \param hAlign Wyrównanie zawartoœci komórki w poziomie
    void setHAlign(HAlign hAlign);
	//! Czyœci atrybut wyrównania zawartoœci komórki w poziomie
    void clearHAlign();
	//! \param vAlign Wyrównanie zawartoœci komórki w pionie
    void setVAlign(VAlign vAlign);
	//! Czyœci atrybut wyrównania zawartoœci komórki w pionie
    void clearVAlign();

private:
	//! Mapa atrubutów
    std::map<HtmlCellAttribute, boost::any> attributes;
};

//! Lista atrybutów tabeli
enum HtmlTableAttribute {
    TableWidth,			//! Szerokoœæ
	TableHeight,		//! Wysokoœæ
	TableBorder,		//! Gruboœæ ramki
    TableBgColor,		//! Kolor t³a
    TableCellSpacing,	//! Odstêp pomiêdzy komórkami
    TableCellPadding    //! Padding komórek
};

//! Klasa obs³uguj¹ca atrybuty tabeli
class HtmlTableAttributes
{
public:

    HtmlTableAttributes();
    ~HtmlTableAttributes();

public:
	//! \param html [out]Dokument do którego generujemy atrybut w postaci tagów HTML
    void generateHtmlAttributes(QString & html) const;
	//! \param width Szerokoœæ tabeli
	//! \param typ Sposób zadawania rozmiaru
    void setWidth(float width, HtmlWidthType type);
	//! Czyœci atrybut szerokoœci
    void clearWidth();
	//! \param height Wysokoœc tabeli
	//! \param typ Sposób zadawania rozmiaru
	void setHeight(float height, HtmlWidthType type);
	//! Czyœci atrybut wysokoœci
    void clearHeight();
	//! \param bgColor Kolor t³a tabeli
    void setBgColor(const QString & bgColor);
	//! Czyœci atrybut koloru t³a
    void clearBgColor();
	//! \param border Szerokoœæ ramki tabeli i komóek
	void setBorder(int border);
	//! Czyœci atrybut szerokoœci ramki tabeli i komóek
    void clearBorder();
	//! \param spacing Dodatkowa iloœæ pixeli wokó³ w³aœciwej zawartoœci komórki
    void setCellSpacing(int spacing);
	//! Czyœci atrybut dodatkowej iloœci pixeli wokó³ w³aœciwej zawartoœci komórki
    void clearCellSpacing();
	//! \param spacing Odstêp pomiêdzy komórkami w tabeli
    void setCellPadding(int padding);
	//! Czyœci atrybut odstêpu pomiêdzy komórkami w tabeli
    void clearCellPadding();

private:
	//! Mapa atrybutów tabeli
    std::map<HtmlTableAttribute, boost::any> attributes;
};

//! Typ tabeli opisuj¹cej atrybuty komórek tabeli HTML
typedef DataTable<HtmlCellAttributes> HtmlDataTableCellAttributes;

//! Typ elementu tabeli - wiersza, kolumny, komórki
enum StyleStatus {
    Single  = 0, //! Pojedynczy element
    First   = 1, //! Pierwszy element
    Middle  = 2, //! Œrodkowy element
    Last    = 3  //! Ostatni element
};

//! \param value wartoœæ dla której wyznaczmy typ
//! \param min Minimalna wartoœæ zakresu
//! \param max Maksymalna wartoœc zakresu
//! \return Typ elementu: Single -> min = max = value, First -> value = min, min != max, Middle -> value != min, value != max, min != max, Last -> value = max, min != max
StyleStatus styleStatus(int value, int min, int max);

//! Typ opisuj¹cy style wierszy dla ich statusów
typedef QString HtmlRowStyles[4];
//! Typ opisuj¹cy style kolumn dla ich statusów
typedef QString HtmlColumnStyles[4];

//! Typ opisuj¹cy style komórek zale¿nie od ich statusów
typedef QString HtmlStyles[4][4];

//! Agregat styli komórek
struct HtmlDataTableStyles
{
    //! Styl tabeli
    QString tableStyle_;

	//! Atrybuty tabeli
	HtmlTableAttributes tableAttributes;

    //! Styl wierszy gdy poziome na³ówki
    HtmlRowStyles headerRowStyle_;

    //! Style wierszy danych
    HtmlRowStyles contentRowStyle_;

    //! Macierz styli komórek nag³ówków
    HtmlStyles headersStyles;

    //! Macierz styli komórek danych
    HtmlStyles contentStyles;
};

//Opisuje strukturê nag³ówków tabeli
struct TableHeadersStructureDescriptor {
    //! Iloœæ wierszy nag³ówkowych
    int headerRows;
    //! Ilosæ kolumn nag³ówkowych
    int headerColumns;
};

//! Klasa generuj¹ca tabele HTML z danymi
class HtmlDataTableGenerator
{
public:
    //! \param content Zawartoœæ tabeli wiersz-kolumna
    //! \param structure Struktura tabeli
    //! \param styles Style poszczególnycvh komórek
    //! \param rows Ca³kowita iloœæ wierszy tabeli (utniemy lub dodamy jeœli bêdzie trzeba
    //! \param columns Ca³kowita iloœæ kolumn tabeli (utniemy lub dodamy jeœli bêdzie trzeba
    static void generateHtmlTable(QString & table, const HtmlDataTableContent & content, TableHeadersStructureDescriptor structure,
        const HtmlDataTableCellAttributes & cellAttributes = HtmlDataTableCellAttributes(), const HtmlDataTableStyles & styles = HtmlDataTableStyles(), int rows = 0, int columns = 0);

private:
	//! Metoda generuje/wype³nia/styluje nag³ówki tabeli
	//! \param content Zawartoœc tabeli któr¹ uzupe³niamy
	//! \param cellAttributes Atrybuty komórek
	//! \param rowsData Dane wiersza
	//! \param structure Uk³ad tabeli - iloœc nag³ówków...
	//! \param styles Style tabeli
	//! \param rows Iloœæ wierszy które bêd¹ brane pod uwagê
	//! \param columns Iloœæ kolumn które bêd¹ brane pod uwagê
    static void buildHeaders(HtmlDataTableContent & content, const HtmlDataTableCellAttributes & cellAttributes,
        HtmlDataTableContent::Column & rowsData, const TableHeadersStructureDescriptor & structure,
        const HtmlDataTableStyles & styles, int rows, int columns);

	//! Metoda generuje zawartoœæ komórki tabeli
	//! \param content [out] Zawartoœæ komórki
	//! \param style Styl komórki
	//! \param cellAttributes Atrybuty komórki
    static void buildCell(QString & content, const QString & style = QString(), const QString & cellAtributes = QString());

	//! Metoda generuje zawartoœæ tabeli (dane, nie nag³ówki)
    static void buildContent(HtmlDataTableContent & content, const HtmlDataTableCellAttributes & cellAttributes,
        const TableHeadersStructureDescriptor & structure, const HtmlDataTableStyles & styles, int rows, int columns);

	//! Metoda otwiera znaczniki HTML tabeli
	//! \param table [out] Dokument HTML do którego generujemy tabelê
	//! \param style Styl tabeli
	//! \param attributes Atrybuty tabeli
    static void openTable(QString & table, const QString & style, const HtmlTableAttributes & attributes);
	//! Metodad zamyka znaczniki HTML tabeli
	//! \param table [out] Dokument HTML do którego generujemy tabelê
    static void closeTable(QString & table);
	//! Metoda otwiera znaczniki HTML wiersza tabeli
	//! \param table [out] Dokument HTML do którego generujemy tabelê
	//! \param style Styl wiersza
    static void openRow(QString & table, const QString & style);
	//! Metodad zamyka znaczniki HTML wiersza tabeli
	//! \param table [out] Dokument HTML do którego generujemy tabelê
    static void closeRow(QString & table);
};

typedef boost::array<boost::array<std::pair<QTextTableCellFormat, QTextBlockFormat>,4>,4> TableCellsFormat;

//! Kalsa pomocnicza przy wype³nianiu tabeli wg standardów Qt
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
