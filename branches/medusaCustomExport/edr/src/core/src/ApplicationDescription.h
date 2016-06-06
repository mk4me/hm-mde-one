/********************************************************************
    created:  2014/04/02
    created:  2:4:2014   10:38
    filename: ApplicationDescription.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___APPLICATIONDESCRIPTION_H__
#define HEADER_GUARD___APPLICATIONDESCRIPTION_H__

#include <corelib/IApplicationDescription.h>

namespace core
{
	class ApplicationDescription : public IApplicationDescription
	{
	public:
		//! Domy�lny konstruktor
		ApplicationDescription(const ExtendedVersion * version,
			const plugin::IVendorDescription * vendor);
		//! Destruktor wirtualny
		virtual ~ApplicationDescription();

		//! \return Wersja aplikacji
		virtual const ExtendedVersion * version() const;
		//! \return Nazwa aplikacji
		virtual const std::string name() const;
		//! \return Kr�ka nazwa aplikacji
		virtual const std::string shortName() const;
		//! \return Opis aplikacji
		virtual const std::string description() const;
		//! \return Dostawca aplikacji
		virtual const plugin::IVendorDescription * vendor() const;

		//! \param Nazwa aplikacji
		void setName(const std::string & name);
		//! \param Kr�tka nazwa aplikacji
		void setShortName(const std::string & shortName);
		//! \param Kr�tka nazwa aplikacji
		void setDescription(const std::string & description);

	private:
		//! Nazwa skr�cona
		std::string shortName_;
		//! Opis aplikacji
		std::string description_;
		//! Wersja
		const ExtendedVersion * version_;
		//! Dostawca
		const plugin::IVendorDescription * vendor_;
	};
}

#endif	//	HEADER_GUARD___APPLICATIONDESCRIPTION_H__
