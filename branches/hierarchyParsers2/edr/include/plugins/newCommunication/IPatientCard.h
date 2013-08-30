/********************************************************************
    created:  2012/03/09
    created:  9:3:2012   11:31
    filename: IPatientCard.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IPATIENTCARD_H__
#define HEADER_GUARD___IPATIENTCARD_H__

#include <QtGui/QWidget>
#include <QtGui/QPixmap>
#include <webserviceslib/Entity.h>
#include <plugins/newCommunication/IDataSourceUserData.h>

namespace communication
{
	//! Interfejs karty pacjenta
	class IPatientCard
	{
	public:

		//! Wirtualny destruktor
		virtual ~IPatientCard() {}

		//! \return Nazwa schematu karty pacjenta
		virtual const std::string name() = 0;
		//! \return Widget z kartą pacjenta
		virtual QWidget * widget() = 0;
		//! \param patient Pacjent którego należy sformatować w karcie pacjenta - dla nullptr należy zresetować kartę pacjenta
		//! \param subject Subject którego sesje będziemy czytać
		//! \param photo Zdjęcie pacjenta
		//! \param userData Dane użytkownika (na potrzeby notatek!!)
		virtual void setPatient(const webservices::MedicalShallowCopy::Patient * patient = nullptr,
			const webservices::MotionShallowCopy::Performer * subject = nullptr,
			const QPixmap & photo = QPixmap(), const IUserData & userData = IUserData()) = 0;
	};
}

#endif	//	HEADER_GUARD___IPATIENTCARD_H__
