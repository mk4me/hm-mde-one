#include <qthtmlgenlib/TableGenerator.h>

HtmlCellAttributes::HtmlCellAttributes()
{

}

HtmlCellAttributes::~HtmlCellAttributes()
{

}

void HtmlCellAttributes::generateHtmlAttributes(QString & html) const
{
    for(auto it = attributes.begin(); it != attributes.end(); ++it){

        switch(it->first){

        case CellWidth:
            {                
                HtmlWidth htmlWidth = boost::any_cast<HtmlWidth>(it->second);
                std::string attrib(" width=\"");
                attrib += boost::lexical_cast<std::string>(htmlWidth.width) + (htmlWidth.type == WAbsolute ? "px" : "%") + "\"";
                html += attrib.c_str();
            }

            break;

        case CellBgColor:

            {
                QString color = boost::any_cast<QString>(it->second);
                if(color.isEmpty() == false){
                    html += " bgcolor=\"" + color + "\"";
                }
            }
            break;
        case CellColSpan:
            {
                int span = boost::any_cast<int>(it->second);
                std::string attrib(" colspan=\"");
                attrib += boost::lexical_cast<std::string>(span) + "\"";
                html += attrib.c_str();
            }

            break;
        case CellRowSpan:
            {
                int span = boost::any_cast<int>(it->second);
                std::string attrib(" rowspan=\"");
                attrib += boost::lexical_cast<std::string>(span) + "\"";
                html += attrib.c_str();
            }

            break;
        case CellHAlign:
            {
                HAlign hAlign = boost::any_cast<HAlign>(it->second);
                std::string attrib;

                switch(hAlign){
                case HALeft:

                    attrib = "left";
                    break;
                case HARight:

                    attrib = "right";
                    break;
                case HACenter:

                    attrib = "center";
                    break;
                case HAJustify:

                    attrib = "justify";
                    break;
                }
                attrib = " align=\"" + attrib + "\"";
                html += attrib.c_str();
            }

            break;
        case CellVAlign:
            {
                VAlign vAlign = boost::any_cast<VAlign>(it->second);
                std::string attrib;

                switch(vAlign){
                case VATop:

                    attrib = "top";
                    break;
                case VAMiddle:

                    attrib = "middle";
                    break;
                case VABottom:

                    attrib = "bottom";
                    break;
                }
                attrib = " valign=\"" + attrib + "\"";
                html += attrib.c_str();
            }
            break;

        }
    }
}

void HtmlCellAttributes::setWidth(float width, HtmlWidthType type)
{
    HtmlWidth htmlWidth = { width, type};
    attributes[CellWidth] = htmlWidth;
}

void HtmlCellAttributes::clearWidth()
{
    attributes.erase(CellWidth);
}

void HtmlCellAttributes::setBgColor(const QString & bgColor)
{
    attributes[CellBgColor] = bgColor;
}

void HtmlCellAttributes::clearBgColor()
{
    attributes.erase(CellWidth);
}

void HtmlCellAttributes::setColSpan(CellSpan colSpan)
{
    attributes[CellColSpan] = colSpan;
}

void HtmlCellAttributes::clearColSpan()
{
    attributes.erase(CellColSpan);
}

void HtmlCellAttributes::setRowSpan(CellSpan rowSpan)
{
    attributes[CellRowSpan] = rowSpan;
}

void HtmlCellAttributes::clearRowSpan()
{
    attributes.erase(CellRowSpan);
}

void HtmlCellAttributes::setHAlign(HAlign hAlign)
{
    attributes[CellHAlign] = hAlign;
}

void HtmlCellAttributes::clearHAlign()
{
    attributes.erase(CellHAlign);
}

void HtmlCellAttributes::setVAlign(HAlign vAlign)
{
    attributes[CellVAlign] = vAlign;
}

void HtmlCellAttributes::clearVAlign()
{
    attributes.erase(CellVAlign);
}

StyleStatus styleStatus(int value, int min, int max)
{
    UTILS_ASSERT(value >= min && value <= max, "Z³a wartoœæ po³o¿enia wzglêdem zakresu mo¿liwych wartoœci");
    StyleStatus ret = Middle;
    if(min == max){
        ret = Single;
    }else if(min == value){
        ret = First;
    }else if(max == value){
        ret = Last;
    }

    return ret;
}

void HtmlDataTableGenerator::generateHtmlTable(QString & table, const HtmlDataTableContent & content, TableHeadersStructureDescriptor structure,
    const HtmlDataTableCellAttributes & cellAttributes, const HtmlDataTableStyles & styles, int rows, int columns)
{
    HtmlDataTableContent c(content);
    HtmlDataTableCellAttributes a(cellAttributes);

    QString tmpTable;

    if(rows > 0) {
        c.setRows(rows);

    }else{
        rows = c.rows();
    }

    if(columns > 0) {
        c.setColumns(columns);
    }else{
        columns = c.columns();
    }

    a.setRows(rows);
    a.setColumns(columns);

    if(structure.headerRows > rows){
        structure.headerRows = rows;
    }

    if(structure.headerColumns > columns){
        structure.headerColumns = columns;
    }

    // wyznaczam maksymalne wartoœci indeksów dla których jeszcze mogê dzia³aæ
    int maxRows = rows - 1;
    int maxColumns = columns - 1;

    HtmlDataTableContent::Column rowsData(rows);

    // budujemy nag³owki tabeli
    buildHeaders(c, a, rowsData, structure, styles, rows, columns);
    // budujemy wnetrze tabeli - dane
    buildContent(c, a, structure, styles, rows, columns);

    // generujemy tabele
    openTable(tmpTable, styles.tableStyle_);
    for(int row = 0; row < rows; ++row){

        openRow(tmpTable, rowsData[row]);

        for(int column = 0; column < columns; ++column){
            tmpTable += c.cell(row, column);
        }

        closeRow(tmpTable);
    }

    closeTable(tmpTable);

    table.swap(tmpTable);
}

void HtmlDataTableGenerator::buildHeaders(HtmlDataTableContent & content, const HtmlDataTableCellAttributes & cellAttributes,
        HtmlDataTableContent::Column & rowsData, const TableHeadersStructureDescriptor & structure,
        const HtmlDataTableStyles & styles, int rows, int columns)
{
    // wyznaczam maksymalne wartoœci indeksów dla których jeszcze mogê dzia³aæ
    int maxRows = structure.headerRows - 1;
    int maxColumns = structure.headerColumns - 1;

    int row = 0;

    /*for( ; row < structure.headerRows; ++row){
    StyleStatus rowCellStatus = styleStatus(row, 0, maxRows);
    rowsData[row] = styles.headerRowStyle_[rowCellStatus];
    for(int column = 0; column < structure.headerColumns; ++column){
    QString attributes;
    cellAttributes.cell(row, column).generateHtmlAttributes(attributes);
    buildCell(content.cell(row, column), styles.headersStyles[rowCellStatus][styleStatus(column, 0, maxColumns)], attributes);
    }
    }*/

    // Najpierw generujemy komórki wierszy nag³ówkowych
    for( ; row < structure.headerRows; ++row){
        StyleStatus rowCellStatus = styleStatus(row, 0, maxRows);
        rowsData[row] = styles.headerRowStyle_[rowCellStatus];
        for(int column = 0; column < columns; ++column){
            QString attributes;
            cellAttributes.cell(row, column).generateHtmlAttributes(attributes);
            buildCell(content.cell(row, column), styles.headersStyles[rowCellStatus][styleStatus(column, 0, columns)], attributes);
        }
    }

    int startRow = row;
    int maxContentRows = rows - 1;
    for( ; row < rows; ++row){

        rowsData[row] = styles.contentRowStyle_[styleStatus(row, startRow, maxContentRows)];

        StyleStatus rowCellStatus = styleStatus(row, 0, maxContentRows);
        for(int column = 0; column < structure.headerColumns; ++column){
            QString attributes;
            cellAttributes.cell(row, column).generateHtmlAttributes(attributes);
            buildCell(content.cell(row, column), styles.headersStyles[rowCellStatus][styleStatus(column, 0, maxColumns)], attributes);
        }
    }
}

void HtmlDataTableGenerator::buildCell(QString & content, const QString & style, const QString & cellAtributes)
{
    content = "<td" + (cellAtributes.isEmpty() == false ? " " + cellAtributes : "") + (style.isEmpty() == false ? " style=\"" + style + "\"" : "") + ">" + content + "</td>";
}

void HtmlDataTableGenerator::buildContent(HtmlDataTableContent & content, const HtmlDataTableCellAttributes & cellAttributes,
    const TableHeadersStructureDescriptor & structure, const HtmlDataTableStyles & styles, int rows, int columns)
{
    // wyznaczam maksymalne wartoœci indeksów dla których jeszcze mogê dzia³aæ
    int maxRows = rows - 1;
    int maxColumns = columns - 1;

    // uzupe³niam style komórek nag³ówkowych wiersza i kolumny
    for(int row = structure.headerRows; row < rows; ++row){
        auto rowStatus = styleStatus(row, structure.headerRows, maxRows);
        for(int column = structure.headerColumns; column < columns; ++column){
            QString attributes;
            cellAttributes.cell(row, column).generateHtmlAttributes(attributes);
            buildCell(content.cell(row, column), styles.contentStyles[rowStatus][styleStatus(column, structure.headerColumns, maxColumns)], attributes);
        }
    }
}

void HtmlDataTableGenerator::openTable(QString & table, const QString & style)
{
    table += "<table border=\"1\" cellspacing=\"0\" cellpadding=\"6\"" + (style.isEmpty() == false ? " style=\"" + style + "\"" : QString()) + ">";
}

void HtmlDataTableGenerator::closeTable(QString & table)
{
    table += "</table>";
}

void HtmlDataTableGenerator::openRow(QString & table, const QString & style)
{
    table += "<tr" + (style.isEmpty() == false ? " style=\"" + style + "\"" : QString()) + ">";
}

void HtmlDataTableGenerator::closeRow(QString & table)
{
    table += "</tr>";
}