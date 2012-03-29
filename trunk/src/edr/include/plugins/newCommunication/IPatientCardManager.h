/********************************************************************
    created:  2012/03/09
    created:  9:3:2012   11:35
    filename: IPatientCardManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IPATIENTCARDMANAGER_H__
#define HEADER_GUARD___IPATIENTCARDMANAGER_H__

#include <plugins/newCommunication/IPatientCard.h>

namespace communication
{
	//! Interfejs menagera schematów kart pacjenta
	class IPatientCardManager
	{
	public:
		//! Wirtualny destruktor
		virtual ~IPatientCardManager() {}

		//! \param patientCard Schemat karty pacjenta do zarejestrowania, staje siê w³asnoœci¹ managera
		virtual int registerPatientCard(IPatientCard * patientCard) = 0;
		//! \return Aktualna karty pacjenta
		virtual IPatientCard * currentPatientCard() const = 0;
		//! \return Index aktualnej karty pacjenta
		virtual int currentPatientCardIndex() const = 0;
		//! \param idx Index karty pacjenta do ustawienia
		virtual void setPatientCard(int idx) = 0;
		//! \param idx Index karty pacjenta dla której chcemy uzyskaæ widget
		//! \return Widget danej karty pacjenta
		virtual QWidget * patientCardWidget(int idx) const = 0;
		//! \param idx Index karty pacjenta ktorej nazwê chcemy pobraæ
		//! \return Nazwa karty pacjenta
		virtual const std::string & patientCardName(int idx) const = 0;
		//! \return Iloœc zarejestrowanych kart pacjenta
		virtual int patientCardsCount() const = 0;
	};
}

#endif	//	HEADER_GUARD___IPATIENTCARDMANAGER_H__
