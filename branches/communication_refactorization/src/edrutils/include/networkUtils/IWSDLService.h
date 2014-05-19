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
	//! WA¯NA UWAGA DO WYJ¥TKÓW !! POWINNY BY RZUCANE TYLKO I WY£¥CZNIE PODCZAS WYWO£YWANIA INVOKE W ZALE¯NOŒCI CO SIÊ NIE POWIEDZIE !!
	//! MA TO NA CELU WPROWADZENIE LENIWEJ INICJALIZACJI US£UG - PO£¥CZENIE JEST NAWI¥ZYWANE FAKTYCZNIE W MOMENCIE U¯YCIA

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

	//! Wyj¹tek obs³ugi serwisu wsdl
	class NETWORKUTILS_EXPORT WSDLServiceException
	{
	public:
		WSDLServiceException(const ExceptionType type = Unknown) throw() : type_(type) {}
		WSDLServiceException(const WSDLServiceException & exception) throw() : error(exception.error), type_(exception.type_) {}
		WSDLServiceException(const char * exception, const ExceptionType type = Unknown) throw() : error(exception), type_(type) {}

		//! \return Typ wyj¹tku
		inline const ExceptionType type() const { return type_; }
		//! \return Opis wyj¹tku
		inline const char * what() const { error.c_str(); }

	private:
		//! Opis wyj¹tku
		std::string error;
		//! Typ wyj¹tku
		const ExceptionType type_;
	};

	class NETWORKUTILS_EXPORT IWSDLService
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
			setValue(param, boost::lexical_cast<std::string>(value));
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