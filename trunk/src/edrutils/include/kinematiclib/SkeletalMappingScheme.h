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

	//! Schemat mapowania kosci
	class SkeletonMappingScheme {
	public:
		//! typ pojedynczego schematu mapowania
		typedef std::map<std::string, std::string> mappingDict;
		//! statyczna metoda wczytuje schematy z pliku XML
		//! \param filename nazwa pliku do wczytania
		//! \return kolekcja schematow mapowania
		static void loadFromXML(const std::string& filename, std::vector<SkeletonMappingSchemePtr>& outSchemes);
		//! \return podstawowy schemat nazewniczy
		static SkeletonMappingSchemePtr createDefault();
		//! \return zwraca slownik mapowania
		const mappingDict& getMappingDictionary() { return mappingDictionary; }

		virtual SkeletonMappingScheme* clone() const 
		{
			return new SkeletonMappingScheme(*this);
		}

	private:
		SkeletonMappingScheme(const mappingDict& mapping) {
			this->mappingDictionary = mapping;
		}

	private:
		mappingDict mappingDictionary;

	private:
		static void readNode(TiXmlNode* node, std::vector<SkeletonMappingSchemePtr>& result);
		static mappingDict readMappingNode(TiXmlNode* node);
	};
}

#endif
