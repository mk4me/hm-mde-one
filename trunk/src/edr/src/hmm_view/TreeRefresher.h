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

//! klasa s³u¿y do odœwie¿ania drzewa danych
class TreeRefresher
{
public:
    TreeRefresher();
    virtual ~TreeRefresher() {}

public:
    //! \return czy mo¿na odœwie¿ac drzewo
    bool getPreventRefresh() const { return preventRefresh; }
    //! ustawia mo¿liwoœæ odœwie¿ania drzewa
    //! \param val 
    void setPreventRefresh(bool val);
    //! Odswiezy drzewo, jeœli jest to mo¿liwe, jeœli nie drzewo zostanie odswiezone kiedy blokada zostanie zdjeta.
    //! \param tree drzewo do odœwie¿enia
    void refresh(QTreeWidget* tree);

private:
    //! wlasciwe odœwie¿enie drzewa
    //! \param tree drzewo do odœwie¿enia
    //! \param sessions sesje, na podstawie których budowane bêdzie drzewo
    void actualRefresh(QTreeWidget* tree, const std::vector<PluginSubject::SessionConstPtr>& sessions);

private:
    //! true - odœwie¿anie zablokowane
    bool preventRefresh;
    //! true - odœwie¿anie bylo zablokowane kiedy przyszlo polecenia odœwie¿enia
    bool needRefresh;
    //! drzewo, które trzeba odswiezyc
    QTreeWidget* tree;
};


#endif
