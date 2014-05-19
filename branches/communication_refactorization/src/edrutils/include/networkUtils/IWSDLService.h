/********************************************************************
	created:  2014/05/14	20:06:38
	filename: IWSDLService.h
	author:	  Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef __HEADER_GUARD_NETWORKUTILS__IWSDLSERVICE_H__
#define __HEADER_GUARD_NETWORKUTILS__IWSDLSERVICE_H__

#include <string>
#include <networkUtils/Export.h>
#include <utils/SmartPtr.h>
#include <boost/lexical_cast.hpp>

namespace networkUtils
{
	//! WA�NA UWAGA DO WYJ�TK�W !! POWINNY BY RZUCANE TYLKO I WY��CZNIE PODCZAS WYWO�YWANIA INVOKE W ZALE�NO�CI CO SI� NIE POWIEDZIE !!
	//! MA TO NA CELU WPROWADZENIE LENIWEJ INICJALIZACJI US�UG - PO��CZENIE JEST NAWI�ZYWANE FAKTYCZNIE W MOMENCIE U�YCIA

	enum ExceptionType
	{
		Unknown,
		Security,
		Operation,
		OperationValue,
		Initialization,
		Response,
		Invoke
	};

	//! Wyj�tek obs�ugi serwisu wsdl
	class NETWORKUTILS_EXPORT WSDLServiceException
	{
	public:
		WSDLServiceException(const ExceptionType type = Unknown) throw() : type_(type) {}
		WSDLServiceException(const WSDLServiceException & exception) throw() : error(exception.error), type_(exception.type_) {}
		WSDLServiceException(const char * exception, const ExceptionType type = Unknown) throw() : error(exception), type_(type) {}

		//! \return Typ wyj�tku
		inline const ExceptionType type() const { return type_; }
		//! \return Opis wyj�tku
		inline const char * what() const { error.c_str(); }

	private:
		//! Opis wyj�tku
		std::string error;
		//! Typ wyj�tku
		const ExceptionType type_;
	};

	class NETWORKUTILS_EXPORT IWSDLService
	{
	public:
		//! Desturktor wirtualny
		virtual ~IWSDLService() {}
		//! \param operation Metoda serwisu do wywo�ania
		virtual void setOperation(const std::string & operation) = 0;
		//! \param name Nazwa warto�ci do ustawienia
		//! \param value Warto�� zmiennej
		virtual void setValue(const std::string & name, const std::string & value) = 0;
		//! Wykonuje operacj� na serwisie webowym
		//! \param process Czy odpowied� ma by� przetworzona i mo�na o rezultaty pyta� metodami getValue
		//! W przeciwnym wypadku nale�y samemu parsowa� odpowied�
		virtual void invoke(const bool process = false) = 0;
		//! \return Pe�na odpowied� serwisu webowego w formacie html/xml
		virtual const std::string xmlResponse() const = 0;
		//! \param name Nazwa warto�ci kt�r� chcemy pobra�
		//! \return Wska�nik do warto�ci, nullptr je�li nie ma takiej warto�ci, wskaxnik pozostaje pod kontrol� implementacji IWSDLService
		virtual const void * getValue(const std::string & name) const = 0;
		//! \tparam Typ jaki chemy ustawi� jako parametr
		//! \param param Nazwa parametru dla kt�rego ustawiamy warto�� wywo�ania
		//! \param value Warto�c dla zadanego parametru
		template<class T>
		void setValue(const std::string& param, const T& value)
		{
			setValue(param, boost::lexical_cast<std::string>(value));
		}
		//! \tparam Typ jaki chemy pobra�
		//! \param param Nazwa parametru dla kt�rego pobieramy warto�� je�li wykonywali�my zapytanie z parsowanie
		//! \param value [out] Warto�c dla zadanego parametru
		template<class T>
		const bool getValue(const std::string & name, T & value) const
		{
			const void * ret = getValue(name);
			if (ret != nullptr){
				value = *((T*)ret);
				return true;
			}

			return false;
		}
	};

	DEFINE_SMART_POINTERS(IWSDLService);
}

#endif	// __HEADER_GUARD_NETWORKUTILS__IWSDLSERVICE_H__