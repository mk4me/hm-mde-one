/********************************************************************
	created:  2014/03/26
	created:  26:3:2014   12:39
	filename: AnnotationStatusFilter.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD_DICOM__ANNOTATIONSTATUSFILTER_H__
#define HEADER_GUARD_DICOM__ANNOTATIONSTATUSFILTER_H__

//#include <plugins/hmdbCommunication/DataSourceFilter.h>
#include <hmdbserviceslib/ShallowCopy.h>
#include <set>

namespace dicom
{
	class AnnotationStatusFilter// : public ShallowCopyFilter
	{
	public:
		//! Zbiór identyfikatorów
		typedef std::set<int> Identifiers;

	public:
		//! \param name Nazwa filtra
		//! \param accept Czy dane maj¹ byæ akceptowane czy odrzucane jesli znajdziemy identyfikator
		//! \param missingDefault Czy dane maj¹ byæ akceptowane czy odrzucane jesli nie znajdziemy identyfikator
		//! \param identifiers Identyfikatory na których bêdzie operowa³ filtr
		AnnotationStatusFilter(const std::string & name, const bool accept,
			const bool missingDefault,
			const Identifiers & identifiers = Identifiers());
		//! Destruktor wirtualny
		virtual ~AnnotationStatusFilter();

		//! \return Nazwa fitlra
		virtual std::string name() const;

		//! Filtr dla motionów opisuj¹cych badania USG
		virtual bool filterMotion(const hmdbServices::MotionShallowCopy::Trial* motion) const;

		//! \param identifiers Identyfikatory na których bêdzie operowa³ filtr
		void setIdentifiers(const Identifiers & identifiers);

	private:
		//! Nazwa filtra
		const std::string name_;
		//! Identyfikatory
		mutable Identifiers identifiers;
		//! Czy akceptowaæ jeœli znajdziemy identyfikator
		const bool accept;
		//! Czy akceptowaæ jeœli nie znajdziemy identyfikator
		const bool missingDefault;
	};
}

#endif	//	HEADER_GUARD_DICOM__ANNOTATIONSTATUSFILTER_H__
