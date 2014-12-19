/********************************************************************
	created:	2013/04/18
	created:	18:4:2013   9:22
	filename: 	IAnalysisDataItem.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MDE_VIEW__IANALYSISDATAITEM_H__
#define HEADER_GUARD_MDE_VIEW__IANALYSISDATAITEM_H__

#include <corelib/Variant.h>
#include <corelib/HierarchyHelper.h>

namespace core {

//! Interfejs dla podstawowego elementu hierarchii danych.
//! Jego podstawowym zadaniem jest dostarczenie danych i sposobu ich wizualizacji
class IHierarchyDataItem
{
public:
	virtual ~IHierarchyDataItem() {}

public:
    //! \return kolekcja z helperami (obiekty, kt�re dostarczaj� informacji jak wizualizowa� dane)
    virtual std::list<HierarchyHelperPtr> getHelpers() const = 0;
	//! \return domy�lny helper dla danych, najlepiej do nich pasuj�cy, domy�lnie null
	virtual HierarchyHelperPtr getDefaultHelper() const { return HierarchyHelperPtr(); }
    //! \return wrapper zwi�zany z elementem w hierarchii, mo�e by� pusty wska�nik, je�li dane s� na wy�szym poziomie abstrakcji   
	virtual VariantConstPtr getData() const = 0;
};
DEFINE_SMART_POINTERS(IHierarchyDataItem);

}

#endif
