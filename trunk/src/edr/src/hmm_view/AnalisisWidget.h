/********************************************************************
	created:	2011/10/21
	created:	21:10:2011   11:09
	filename: 	AnalisisWidget.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__ANALISISWIDGET_H__
#define HEADER_GUARD_HMM__ANALISISWIDGET_H__


#include "DataFilterWidget.h"
#include "ui_AnalisisWidget.h"

class AnalisisTreeWidget;

//! Klasa jest odpowiedzialna za widok zakladki analiz
class AnalisisWidget : public QWidget, public Ui::AnalisisWidget
{
    Q_OBJECT;
public:
    //! Konstruktor
    //! \param parent rodzic widgeta
    //! \param hmm obiekt widoku HMM 
    //! \param margin margines dla filtr�w
    //! \param flags flagi Qt
    AnalisisWidget(QWidget* parent, HmmMainWindow* hmm, int margin = 2, Qt::WindowFlags flags = 0);
	virtual ~AnalisisWidget() {}

public:
    //! \return drzewo danych
    QTreeWidget* getTreeWidget();
    //! \return widget przechowuj�cy miniaturki do raport�w
    QWidget* getRaportsThumbnailList();
    //! dodaje widget z filtrami
    //! \param filter dodawany widget
    void addDataFilterWidget(DataFilterWidget* filter);
    //! \return obszar, gdzie laduja wizualizatory i timeline
    QWidget* getArea() { return analisisArea; }

private slots:
    //! chowa/pokazuje filtry
    //! \param expand 
    void setFiltersExpanded(bool expand);
    //! kliknieto w filtr
    //! \param filter klikniety
    void filterClicked(FilterEntryWidget* filter);
    //! Kliknieto zatwierdz w graficznym konfiguratorze
    void applyClicked();
    //! proste przejscie do pierwszej zakladki (z drzewem danych)
    void switchToFirstTab();
    //! odtwarza drzewo danych 
    //! \param currentFilter filtr, na podstawie kt�rego odtworzone zostanie drzewo
    void recreateTree(FilterEntryWidget* currentFilter);

private:
    //! szeroko�� filtru w zakladce
    int filterWidth;
    //! wysoko�� filtru w zakladce
    int filterHeight;
    //! margines dla filtr�w w zakladce
    int margin;
    //! drzewo danych
    AnalisisTreeWidget* treeWidget;
    //! widok HMM
    HmmMainWindow* hmm;
    //TODO potrzebne tylko, aby przekazac info mi�dzy elementami.
    FilterEntryWidget* currentFilter;
};



#endif
