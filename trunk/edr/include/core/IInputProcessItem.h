/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   9:12
	filename: 	IInputProcessItem.h
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__IINPUTPROCESSITEM_H__
#define HEADER_GUARD_CORE__IINPUTPROCESSITEM_H__

#include <core/IWorkflowItemBase.h>
#include <core/IInputDescription.h>

namespace core {

class IInputProcessItem : public IWorkflowItemBase, public IInputDescription
{
public:
    virtual ~IInputProcessItem() 
    {
    }
    //! Metoda wywo≥ywana po przypisaniu danych wejúciowych wizualizatorowi. Powinna uwzglÍdniÊ czyszczenie widgetu
    //! po poprzednim setUp'ie.
    //! \param source èrÛd≥o danych.
    virtual void setUp(IObjectSource* source) = 0;
};

}

#endif