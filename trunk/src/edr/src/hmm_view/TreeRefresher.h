/********************************************************************
	created:	2012/05/31
	created:	31:5:2012   12:23
	filename: 	TreeRefresher.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__TREEREFRESHER_H__
#define HEADER_GUARD_HMM__TREEREFRESHER_H__

#include <plugins/subject/Types.h>

//! klasa sluzy do odswiezania drzewa danych
class TreeRefresher
{
public:
    TreeRefresher();
    virtual ~TreeRefresher() {}

public:
    //! \return czy mozna odswiezac drzewo
    bool getPreventRefresh() const { return preventRefresh; }
    //! ustawia mozliwosc odswiezania drzewa
    //! \param val 
    void setPreventRefresh(bool val);
    //! Odswiezy drzewo, jesli jest to mozliwe, jesli nie drzewo zostanie odswiezone kiedy blokada zostanie zdjeta.
    //! \param tree drzewo do odswiezenia
    void refresh(QTreeWidget* tree);

private:
    //! wlasciwe odswiezenie drzewa
    //! \param tree drzewo do odswiezenia
    //! \param sessions sesje, na podstawie ktorych budowane bedzie drzewo
    void actualRefresh(QTreeWidget* tree, const std::vector<PluginSubject::SessionConstPtr>& sessions);

private:
    //! true - odswiezanie zablokowane
    bool preventRefresh;
    //! true - odswiezanie bylo zablokowane kiedy przyszlo polecenia odswiezenia
    bool needRefresh;
    //! drzewo, ktore trzeba odswiezyc
    QTreeWidget* tree;
};


#endif
