/********************************************************************
	created:	2011/07/17
	created:	17:7:2011   11:26
	filename: 	SkeletalMappingScheme.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__SKELETALMAPPINGSCHEME_H__
#define HEADER_GUARD_KINEMATIC__SKELETALMAPPINGSCHEME_H__

namespace kinematic {

	class SkeletonMappingScheme;
	typedef boost::shared_ptr<SkeletonMappingScheme> SkeletonMappingSchemePtr;
	typedef boost::shared_ptr<const SkeletonMappingScheme> SkeletonMappingSchemeConstPtr;

	//! Schemat mapowania kości
	class SkeletonMappingScheme {
	public:
		//! typ pojedynczego schematu mapowania
		typedef std::map<std::string, std::string> mappingDict;
		//! statyczna metoda wczytuje schematy z pliku XML
		//! \param filename nazwa pliku do wczytania
		//! \return kolekcja schematów mapowania
		static void loadFromXML(const std::string& filename, std::vector<SkeletonMappingSchemePtr>& outSchemes);
		//! \return podstawowy schemat nazewniczy
		static SkeletonMappingSchemePtr createDefault();
		//! \return zwraca słownik mapowania
		const mappingDict& getMappingDictionary() { return mappingDictionary; }
        //! \return głęboka kopia obiektu
		virtual SkeletonMappingScheme* clone() const 
		{
			return new SkeletonMappingScheme(*this);
		}

	private:
		//! konstruktor
		//! \param mapping wypełniony słownik mapowania
		SkeletonMappingScheme(const mappingDict& mapping) {
			this->mappingDictionary = mapping;
		}

	private:
        //! wypełniony słownik mapowania
		mappingDict mappingDictionary;

	private:
		//! wczytuje schematy mapowań z wczytanej struktury xml
		//! \param node węzeł zawierający mapowania
		//! \param result kolekcja ze schematami mapowań
		static void readNode(TiXmlNode* node, std::vector<SkeletonMappingSchemePtr>& result);
		//! wczytuje pojedynczy wpis mapowania
		//! \param node analizowany węzeł
		static mappingDict readMappingNode(TiXmlNode* node);
	};
}

#endif
