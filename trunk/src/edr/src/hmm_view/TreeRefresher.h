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

//! klasa s�u�y do od�wie�ania drzewa danych
class TreeRefresher
{
public:
    TreeRefresher();
    virtual ~TreeRefresher() {}

public:
    //! \return czy mo�na od�wie�ac drzewo
    bool getPreventRefresh() const { return preventRefresh; }
    //! ustawia mo�liwo�� od�wie�ania drzewa
    //! \param val 
    void setPreventRefresh(bool val);
    //! Odswiezy drzewo, je�li jest to mo�liwe, je�li nie drzewo zostanie odswiezone kiedy blokada zostanie zdjeta.
    //! \param tree drzewo do od�wie�enia
    void refresh(QTreeWidget* tree);

private:
    //! wlasciwe od�wie�enie drzewa
    //! \param tree drzewo do od�wie�enia
    //! \param sessions sesje, na podstawie kt�rych budowane b�dzie drzewo
    void actualRefresh(QTreeWidget* tree, const std::vector<PluginSubject::SessionConstPtr>& sessions);

private:
    //! true - od�wie�anie zablokowane
    bool preventRefresh;
    //! true - od�wie�anie bylo zablokowane kiedy przyszlo polecenia od�wie�enia
    bool needRefresh;
    //! drzewo, kt�re trzeba odswiezyc
    QTreeWidget* tree;
};


#endif
