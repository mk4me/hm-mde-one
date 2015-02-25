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
#include <list>
#include <utils/SmartPtr.h>
#include <kinematiclib/Joint.h>

namespace kinematic
{
	//! Schemat mapowania kości
	struct SkeletonMappingScheme
	{
		//! Typ pojedynczego schematu mapowania jointow
		typedef std::map<std::string, std::string> MappingDict;
		//! statyczna metoda wczytuje schematy z pliku XML
		//! \param filename nazwa pliku do wczytania
		//! \return kolekcja schematów mapowania
		static void loadFromXML(const std::string& filename, std::list<MappingDict>& outSchemes);
		//! \return Domyślne mapowanie
		static const MappingDict& defaultAcclaimToHAnimMappingScheme();

		struct MappingSchemeVisitor
		{
			MappingSchemeVisitor(const MappingDict & mappingDict)
				: mappingDict(mappingDict)
			{

			}

			MappingSchemeVisitor(MappingDict && mappingDict)
				: mappingDict(std::move(mappingDict))
			{

			}

			~MappingSchemeVisitor()
			{

			}

			void operator()(JointPtr joint)
			{
				if (joint != nullptr){
					auto it = mappingDict.find(joint->value.name);
					if (it != mappingDict.end()){
						joint->value.name = it->second;
					}
				}
			}

			MappingDict mappingDict;
		};
	};
}

#endif
