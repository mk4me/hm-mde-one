/********************************************************************
created:  2014/05/14	20:06:38
filename: IWSDLService.h
author:	  Mateusz Janiak

purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_NETWORKUTILS__IWSDLSERVICE_H__
#define __HEADER_GUARD_NETWORKUTILS__IWSDLSERVICE_H__

#include <string>
#include <utils/SmartPtr.h>
#include <boost/lexical_cast.hpp>
#include <stdexcept>

namespace networkUtils
{
	//! Wyj�tek obs�ugi serwisu wsdl
	class WSDLServiceException : public std::runtime_error
	{
	public:
		//! Typ wyj�tku
		enum ExceptionType
		{
			Unknown,		//! Nieznany typ
			Security,		//! Wyj�tek bezpiecze�stwa
			Operation,		//! Wyj�tek operacji
			OperationValue,	//! Wyj�tek parametru operacji
			Initialization,	//! Wyj�tek inicjalizacji us�ugi
			Response,		//! Wyj�tek odpowiedzi
			Invoke			//! Wyj�tek wykonania us�ugi
		};

	public:
		//! \param type Typ wyjatku us�ugi
		WSDLServiceException(const ExceptionType type = Unknown) throw() : std::runtime_error(std::string()), type_(type) {}
		//! \param exception Wujatek kt�ry kopiujemy
		WSDLServiceException(const WSDLServiceException & exception) throw() : std::runtime_error(exception), type_(exception.type_) {}
		//! \param exception Opis wyjatku
		//! \param type Typ wyjatku us�ugi
		WSDLServiceException(const char * exception, const ExceptionType type = Unknown) throw() : std::runtime_error(exception), type_(type) {}

		//! \return Typ wyj�tku
		inline const ExceptionType type() const { return type_; }

	private:
		//! Typ wyj�tku
		const ExceptionType type_;
	};

	class IWSDLService
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
			try{
				setValue(param, boost::lexical_cast<std::string>(value));
			}
			catch (std::exception & e){
				throw WSDLServiceException(e.what(), WSDLServiceException::OperationValue);
			}
			catch (...){
				throw WSDLServiceException("Unknown set operation value error", WSDLServiceException::OperationValue);
			}
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
