/********************************************************************
	created:	2012/05/04
	created:	4:5:2012   13:43
	filename: 	SummaryWindow.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__SUMMARYWINDOW_H__
#define HEADER_GUARD_HMM__SUMMARYWINDOW_H__

class HmmMainWindow;
class VisualizerWidget;

#include <plugins/subject/ISubject.h>
#include "TreeItemHelper.h"

//! okno s³u¿y do wyœwietlenia podsumowania i dodatkowych informacji dla wizualizatora lub elementu drzewa danych
class SummaryWindow
{
public:
    //! konstruktor
    //! \param hmm dostarcza drzewa analiz, przechowuje okienko z podsumowaniami
    SummaryWindow(HmmMainWindow* hmm) : hmm(hmm), tree(nullptr) {}
	//! destruktor
	virtual ~SummaryWindow() {}

public:
    //! kolekcja z obiektami pomocniczymi dla elementów drzewa analiz
    typedef std::vector<TreeItemHelperPtr> HelpersCollection;

public:
    //! inicjalizuje widget z podsumowaniami
    void initialize();
    //! wyœwietla konkretne informacje w okienku podsumowañ
    //! \param helpers kolekcja z obiektami pomocniczymi dla elementów drzewa analiz, dostarczaj¹ one informacje do wyœwietlenia
    void display(const HelpersCollection& helpers);
    //! dodaje element z informacjami
    //! \param text tekst do wyœwietlenia
    //! \param root korzeñ, do którego zostanie dodany element
    void addItem( QString text, QTreeWidgetItem* root );
    //! Metoda pomocnicza, tworzy opis próby pomiarowej
    //! \param motion próba pomiarowa, której bêdzie dotyczy³ opis
    QString createDescription( PluginSubject::MotionConstPtr motion ) const;

private:
    //! g³ówne okno aplikacji
    HmmMainWindow* hmm;
    //! drzewo, które bêdzie wyœwietlane w oknie z podsumowaniami
    QTreeWidget* tree;
};
typedef core::shared_ptr<SummaryWindow> SummaryWindowPtr;
typedef core::shared_ptr<const SummaryWindow> SummaryWindowConstPtr;

//! Kontroler dla okienka z podsumowaniem
class SummaryWindowController : public QObject
{
    Q_OBJECT;
public:
    //! konstruktor
    //! \param sw obiekt z oknem podsumowañ
    //! \param hmm g³ówne okno aplikacji
    SummaryWindowController(SummaryWindowPtr sw, HmmMainWindow* hmm);

public slots:
    //! pobiera dane z elementu i przesy³a je do okna podsumowañ
    //! \param item elmenet drzewa analiz
    //! \param column zaznaczona kolumna, ignorowane
    void onTreeItemSelected(QTreeWidgetItem* item, int column);
    //! pobiera dane na podstawie elementów zawartych w wizualizatorze i przesy³a je do okna podsumowañ
    //! \param visualizatorWidget wizualizator, który uzyska³ focus
    void onVisualizator(VisualizerWidget* visualizatorWidget);

private:
    //! g³ówne okno aplikacji
    HmmMainWindow* hmm;
    //! obiekt z oknem podsumowañ
    SummaryWindowPtr summary;
};



#endif
