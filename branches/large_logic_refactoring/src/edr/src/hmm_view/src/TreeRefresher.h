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

//! klasa służy do odświeżania drzewa danych, 
//! dodaje możliwość zablokowania, drzewo zostanie odświeżone dopiero, gdy będzie to możliwe
class TreeRefresher
{
public:
    //! 
    TreeRefresher();
    //! 
    virtual ~TreeRefresher() {}

public:
    //! \return czy można odświeżać drzewo
    bool getPreventRefresh() const { return preventRefresh; }
    //! ustawia możliwość odświeżania drzewa
    //! \param val 
    void setPreventRefresh(bool val);
    //! odświeży drzewo, jeśli jest to możliwe, jeśli nie drzewo zostanie odświeżone kiedy blokada zostanie zdjeta.
    //! \param tree drzewo do odświeżenia
    void refresh(QTreeWidget* tree);

private:
    //! właściwe odświeżenie drzewa
    //! \param tree drzewo do odświeżenia
    //! \param sessions sesje, na podstawie których budowane będzie drzewo
    void actualRefresh(QTreeWidget* tree, const core::ObjectWrapperCollection& sessions);

private:
    //! true - odświeżanie zablokowane
    bool preventRefresh;
    //! true - odświeżanie bylo zablokowane kiedy przyszlo polecenia odświeżenia
    bool needRefresh;
    //! drzewo, które trzeba odświeżyć
    QTreeWidget* tree;
};


#endif
