/********************************************************************
	created:  2016/07/18
	filename: SkiMetadataParser.h
	author:	  Mateusz Janiak
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_SKIDATA__SKIMETADATAPARSER_H__
#define __HEADER_GUARD_SKIDATA__SKIMETADATAPARSER_H__

#include <corelib/IParser.h>

namespace skidata
{
	class SkiMetadataParser : public plugin::ISourceParser
	{
		UNIQUE_ID("33178925-FC36-BACA-BACA-25F5268BE1F2")
		CLASS_DESCRIPTION("Ski metadata parser", "Ski metadata parser")
	public:
		//! Konstruktor domy�lny
		SkiMetadataParser();
		//! Destruktor wirtualny
		virtual ~SkiMetadataParser();
		//! \retrun Lista rozszerze�, kt�re parser obs�uguje. Musz� by� oddzielone �rednikiem.
		//! Obs�ugiwany format rozszerzenia: [ { *. | . } ]ext
		virtual void acceptedExpressions(Expressions & expressions) const;
		//! \return Instancja tego samego typu.
		virtual IParser* create() const;
		//! \param object [out] Obiekty danych parsera w kolejno�ci jak w opisie ExpressionDescription::types dla danego �r�d�a!!
		//! \param idx Indeks obiektu wg opisu dla danego �r�d�a, kt�ry chcemy pobra�
		virtual void getObject(core::Variant& object, const core::VariantsVector::size_type idx) const;
		//! Metoda zwalniaj�ca sparsowane zasoby parsera
		virtual void reset();
		//! Przyporz�dkowanie parsera do konkretnego pliku.
		//! Na ka�dej instancji parsera ta metoda wywo�ywana jest maksymalnie jeden raz.
		//! \param source �cie�ka do �r�d�a danych - plik, url, id urz�dzenia, ...
		virtual void parse(const std::string & source);

	private:
		//! Dane
		utils::ObjectWrapperPtr data;
	};
}

#endif  // __HEADER_GUARD_SKIDATA__SKIMETADATAPARSER_H__