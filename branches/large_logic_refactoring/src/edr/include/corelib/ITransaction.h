/********************************************************************
    created:  2013/01/16
    created:  16:1:2013   12:00
    filename: ITransaction.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ITRANSACTION_H__
#define HEADER_GUARD___ITRANSACTION_H__

namespace core {

	//! Interfejs transakcji dla manager�w. Powinien dzia�a� w oparciu o RAII - jak ScopedLock,
	//! czyli przy tworzeniu pr�bujemy blokowa� zasoby potrzebny do zapewnienia izolacji transakcji,
	//! przy niszczeniu commitujemy, lub w dowolnym momencie rollbackujemy. Rollbackowanie powinno zwalnia� zasoby
	//! u�yte do zapewnienia izolacji transakcji.
	class ITransaction
	{
	public:
		//! Cofa zmiany w ramach transakcji
		virtual void rollback() = 0;
		//! \return Czy transakcja zosta�a wycofana np. w wyniku b��du jednej z operacji transakcji lub
		//! r�cznie przez u�ytkownika w wyniku zewn�trznych czynnik�w
		virtual const bool isRolledback() const = 0;
	};

}

#endif	//	HEADER_GUARD___ITRANSACTION_H__
