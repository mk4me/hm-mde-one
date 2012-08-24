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

//! okno s�u�y do wy�wietlenia podsumowania i dodatkowych informacji dla wizualizatora lub elementu drzewa danych
class SummaryWindow
{
public:
    //! konstruktor
    //! \param hmm dostarcza drzewa analiz, przechowuje okienko z podsumowaniami
    SummaryWindow(HmmMainWindow* hmm) : hmm(hmm), tree(nullptr) {}
	//! destruktor
	virtual ~SummaryWindow() {}

public:
    //! kolekcja z obiektami pomocniczymi dla element�w drzewa analiz
    typedef std::vector<TreeItemHelperPtr> HelpersCollection;

public:
    //! inicjalizuje widget z podsumowaniami
    void initialize();
    //! wy�wietla konkretne informacje w okienku podsumowa�
    //! \param helpers kolekcja z obiektami pomocniczymi dla element�w drzewa analiz, dostarczaj� one informacje do wy�wietlenia
    void display(const HelpersCollection& helpers);
    //! dodaje element z informacjami
    //! \param text tekst do wy�wietlenia
    //! \param root korze�, do kt�rego zostanie dodany element
    void addItem( QString text, QTreeWidgetItem* root );
    //! Metoda pomocnicza, tworzy opis pr�by pomiarowej
    //! \param motion pr�ba pomiarowa, kt�rej b�dzie dotyczy� opis
    QString createDescription( PluginSubject::MotionConstPtr motion ) const;

private:
    //! g��wne okno aplikacji
    HmmMainWindow* hmm;
    //! drzewo, kt�re b�dzie wy�wietlane w oknie z podsumowaniami
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
    //! \param sw obiekt z oknem podsumowa�
    //! \param hmm g��wne okno aplikacji
    SummaryWindowController(SummaryWindowPtr sw, HmmMainWindow* hmm);

public slots:
    //! pobiera dane z elementu i przesy�a je do okna podsumowa�
    //! \param item elmenet drzewa analiz
    //! \param column zaznaczona kolumna, ignorowane
    void onTreeItemSelected(QTreeWidgetItem* item, int column);
    //! pobiera dane na podstawie element�w zawartych w wizualizatorze i przesy�a je do okna podsumowa�
    //! \param visualizatorWidget wizualizator, kt�ry uzyska� focus
    void onVisualizator(VisualizerWidget* visualizatorWidget);

private:
    //! g��wne okno aplikacji
    HmmMainWindow* hmm;
    //! obiekt z oknem podsumowa�
    SummaryWindowPtr summary;
};



#endif
