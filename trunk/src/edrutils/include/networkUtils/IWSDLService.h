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
#include <exception>

namespace networkUtils
{
	//! Wyj¹tek obs³ugi serwisu wsdl
	class WSDLServiceException : public std::exception
	{
	public:
		//! Typ wyj¹tku
		enum ExceptionType
		{
			Unknown,		//! Nieznany typ
			Security,		//! Wyj¹tek bezpieczeñstwa
			Operation,		//! Wyj¹tek operacji
			OperationValue,	//! Wyj¹tek parametru operacji
			Initialization,	//! Wyj¹tek inicjalizacji us³ugi
			Response,		//! Wyj¹tek odpowiedzi
			Invoke			//! Wyj¹tek wykonania us³ugi
		};

	public:
		//! \param type Typ wyjatku us³ugi
		WSDLServiceException(const ExceptionType type = Unknown) throw() : std::exception(), type_(type) {}
		//! \param exception Wujatek który kopiujemy
		WSDLServiceException(const WSDLServiceException & exception) throw() : std::exception(exception), type_(exception.type_) {}
		//! \param exception Opis wyjatku
		//! \param type Typ wyjatku us³ugi
		WSDLServiceException(const char * exception, const ExceptionType type = Unknown) throw() : std::exception(exception), type_(type) {}

		//! \return Typ wyj¹tku
		inline const ExceptionType type() const { return type_; }

	private:
		//! Typ wyj¹tku
		const ExceptionType type_;
	};

	class IWSDLService
	{
	public:
		//! Desturktor wirtualny
		virtual ~IWSDLService() {}
		//! \param operation Metoda serwisu do wywo³ania
		virtual void setOperation(const std::string & operation) = 0;
		//! \param name Nazwa wartoœci do ustawienia
		//! \param value Wartoœæ zmiennej
		virtual void setValue(const std::string & name, const std::string & value) = 0;
		//! Wykonuje operacjê na serwisie webowym
		//! \param process Czy odpowiedŸ ma byæ przetworzona i mo¿na o rezultaty pytaæ metodami getValue
		//! W przeciwnym wypadku nale¿y samemu parsowaæ odpowiedŸ
		virtual void invoke(const bool process = false) = 0;
		//! \return Pe³na odpowiedŸ serwisu webowego w formacie html/xml
		virtual const std::string xmlResponse() const = 0;
		//! \param name Nazwa wartoœci któr¹ chcemy pobraæ
		//! \return WskaŸnik do wartoœci, nullptr jeœli nie ma takiej wartoœci, wskaxnik pozostaje pod kontrol¹ implementacji IWSDLService
		virtual const void * getValue(const std::string & name) const = 0;
		//! \tparam Typ jaki chemy ustawiæ jako parametr
		//! \param param Nazwa parametru dla którego ustawiamy wartoœæ wywo³ania
		//! \param value Wartoœc dla zadanego parametru
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
		//! \tparam Typ jaki chemy pobraæ
		//! \param param Nazwa parametru dla którego pobieramy wartoœæ jeœli wykonywaliœmy zapytanie z parsowanie
		//! \param value [out] Wartoœc dla zadanego parametru
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