/********************************************************************
	created:	2011/07/17
	created:	17:7:2011   11:26
	filename: 	SkeletalMappingScheme.h
	author:		Wojciech Kniec

	purpose:
	*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__SKELETALMAPPINGSCHEME_H__
#define HEADER_GUARD_KINEMATIC__SKELETALMAPPINGSCHEME_H__

#include <string>
#include <map>
#include <vector>
#include <set>
#include <utils/SmartPtr.h>

class TiXmlNode;

namespace kinematic {
	class SkeletonMappingScheme;
	typedef utils::shared_ptr<SkeletonMappingScheme> SkeletonMappingSchemePtr;
	typedef utils::shared_ptr<const SkeletonMappingScheme> SkeletonMappingSchemeConstPtr;

	//! Schemat mapowania kości
	class SkeletonMappingScheme {
	public:
		//! Typ pojedynczego schematu mapowania jointow
		typedef std::map<std::string, std::string> jointsMappingDict;
		//! typ opisujacy zakres segmentow - first to najwyzszy w hierarchii, second to najnizszy
		typedef std::pair<std::string, std::string> segmentsRange;
		//! Typ pojedynczego schematu mapowania kosci
		typedef std::map<std::string, segmentsRange> segmentsMappingDict;
		//! statyczna metoda wczytuje schematy z pliku XML
		//! \param filename nazwa pliku do wczytania
		//! \return kolekcja schematów mapowania
		static void loadFromXML(const std::string& filename, std::vector<SkeletonMappingSchemePtr>& outSchemes);
		//! \return Domyślne mapowanie
		static const jointsMappingDict& defaultJointsMappingDict();
		//! \return podstawowy schemat nazewniczy
		static SkeletonMappingSchemePtr createDefault();
		//! \return schemat nazewniczy
		static SkeletonMappingSchemePtr create(const jointsMappingDict & schemeDict);
		//! \return schemat nazewniczy
		static SkeletonMappingSchemePtr create(const segmentsMappingDict & schemeDict);
		//! \return zwraca słownik mapowania
		const jointsMappingDict& getJointsMappingDictionary() const { return jointsMappingDictionary_; }
		//! \return zwraca słownik mapowania
		const segmentsMappingDict& getSegmentsMappingDictionary() const { return segmentsMappingDictionary_; }
		//! \return głęboka kopia obiektu
		virtual SkeletonMappingScheme* clone() const
		{
			std::auto_ptr<SkeletonMappingScheme> ret(new SkeletonMappingScheme(*this));
			return ret.release();
		}

	private:
		//! konstruktor
		//! \param mapping wypełniony słownik mapowania
		SkeletonMappingScheme(const jointsMappingDict& jointsMapping,
			const segmentsMappingDict & segmentsMapping)
			: jointsMappingDictionary_(jointsMapping),
			segmentsMappingDictionary_(segmentsMapping)
		{
		}

		//! \return Domyslne mapowanie
		static const jointsMappingDict createDefaultMappingDict();

	private:
		//! wypełniony słownik mapowania jointow
		jointsMappingDict jointsMappingDictionary_;
		//! wypelniony slownik mapowania segmentow
		segmentsMappingDict segmentsMappingDictionary_;

	private:
		//! wczytuje schematy mapowań z wczytanej struktury xml
		//! \param node węzeł zawierający mapowania
		//! \param result kolekcja ze schematami mapowań
		static void readNode(TiXmlNode* node, std::vector<SkeletonMappingSchemePtr>& result);
		//! wczytuje pojedynczy wpis mapowania
		//! \param node analizowany węzeł
		static jointsMappingDict readMappingNode(TiXmlNode* node);
	};
}

#endif
