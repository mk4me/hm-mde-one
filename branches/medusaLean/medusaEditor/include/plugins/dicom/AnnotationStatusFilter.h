/********************************************************************
	created:  2014/03/26
	created:  26:3:2014   12:39
	filename: AnnotationStatusFilter.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD_DICOM__ANNOTATIONSTATUSFILTER_H__
#define HEADER_GUARD_DICOM__ANNOTATIONSTATUSFILTER_H__

#include <plugins/dicom/Export.h>
#include <plugins/medusaDatabase/ShallowCopyFilter.h>
#include <set>

namespace dicom
{
	class PLUGIN_DICOM_EXPORT AnnotationStatusFilter : public medusaDatabase::ShallowCopyFilter
	{
	public:
		//! Zbi�r identyfikator�w
		typedef std::set<int> Identifiers;

		enum GeneralStatus
		{
			InEdition,
			InVerification,
			Verified
		};

	public:
		//! \param name Nazwa filtra
		//! \param accept Czy dane maj� by� akceptowane czy odrzucane jesli znajdziemy identyfikator
		//! \param missingDefault Czy dane maj� by� akceptowane czy odrzucane jesli nie znajdziemy identyfikator
		//! \param identifiers Identyfikatory na kt�rych b�dzie operowa� filtr
		//! \param session Sesja zdalna
		AnnotationStatusFilter(const QString & name, const bool accept,
			const bool missingDefault, const GeneralStatus status, medusaDatabase::IHMDBSession * session = nullptr);
		//! Destruktor wirtualny
		virtual ~AnnotationStatusFilter();
		//! \param shallowCopyContext Kontekst p�ytkiej kopii bazy danych
		//! \return Filtr danych
		virtual AnnotationStatusFilter * create(medusaDatabase::IHMDBSession * session) const;
		//! Metoda wywo�ywana przed filtrowaniem
		virtual void beforeFiltering();
		//! \return Nazwa fitlra
		virtual QString name() const;
		//! Filtr dla motion�w opisuj�cych badania USG
		virtual bool filterMotion(const hmdbServices::MotionShallowCopy::Trial* motion) const;

	private:
		//! Metoda od�wie�a identyfikatory
		void refreshIdentifiers();

		static void groupAnnotations(medusaDatabase::IHMDBSession * session,
			AnnotationStatusFilter::Identifiers & inEdition,
			AnnotationStatusFilter::Identifiers & inVerification,
			AnnotationStatusFilter::Identifiers & verified);

	private:
		//! Nazwa filtra
		const QString name_;
		//! Identyfikatory
		mutable Identifiers identifiers;
		//! Czy akceptowa� je�li znajdziemy identyfikator
		const bool accept;
		//! Czy akceptowa� je�li nie znajdziemy identyfikator
		const bool missingDefault;
		//! Sesja
		medusaDatabase::IHMDBSession * session;
		//! Czas ostatniej aktualizacji
		hmdbServices::DateTime lastUpdateTime;
		//! Status
		GeneralStatus status;
	};
}

#endif	//	HEADER_GUARD_DICOM__ANNOTATIONSTATUSFILTER_H__