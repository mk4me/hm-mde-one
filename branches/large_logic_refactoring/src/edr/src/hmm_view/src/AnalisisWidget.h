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

//! Klasa jest odpowiedzialna za widok zakładki analiz
class AnalisisWidget : public QWidget, public Ui::AnalisisWidget
{
    Q_OBJECT;
public:
    //! Konstruktor
    //! \param parent rodzic widgeta
    //! \param hmm obiekt widoku HMM 
    //! \param margin margines dla filtrów
    //! \param flags flagi Qt
    AnalisisWidget(QWidget* parent, HmmMainWindow* hmm, int margin = 2, Qt::WindowFlags flags = 0);
	virtual ~AnalisisWidget() {}

public:
    //! \return drzewo danych
    QTreeWidget* getTreeWidget();
    //! \return widget przechowujący miniaturki do raportów
    QWidget* getRaportsThumbnailList();
    //! dodaje widget z filtrami
    //! \param filter dodawany widget
    void addDataFilterWidget(DataFilterWidget* filter);
    //! \return obszar, gdzie laduja wizualizatory i timeline
    QWidget* getArea() { return analisisArea; }

private slots:
    //! zmienia widoczność filtrów
    //! \param expand chowa/pokazuje filtry
    void setFiltersExpanded(bool expand);
    //! kliknięto w filtr
    //! \param filter kliknięty
    void filterClicked(FilterEntryWidget* filter);
    //! Kliknięto zatwierdź w graficznym konfiguratorze
    void applyClicked();
    //! proste przejście do pierwszej zakładki (z drzewem danych)
    void switchToFirstTab();
    //! odtwarza drzewo danych 
    //! \param currentFilter filtr, na podstawie którego odtworzone zostanie drzewo
    void recreateTree(FilterEntryWidget* currentFilter);

private:
    //! szerokość filtru w zakładce
    int filterWidth;
    //! wysokość filtru w zakładce
    int filterHeight;
    //! margines dla filtrów w zakładce
    int margin;
    //! drzewo danych
    AnalisisTreeWidget* treeWidget;
    //! widok HMM
    HmmMainWindow* hmm;
    //TODO potrzebne tylko, aby przekazac info między elementami.
    FilterEntryWidget* currentFilter;
};



#endif
