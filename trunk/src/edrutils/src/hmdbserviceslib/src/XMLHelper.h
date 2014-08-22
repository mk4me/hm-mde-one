/********************************************************************
	created:  2014/07/31	08:37:11
	filename: XMLHelper.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBSERVICES__XMLHELPER_H__
#define __HEADER_GUARD_HMDBSERVICES__XMLHELPER_H__

#include <string>
#include <tinyxml2.h>
#include <boost/lexical_cast.hpp>
#include <hmdbserviceslib/Entity.h>
#include <hmdbserviceslib/ShallowCopy.h>

namespace hmdbServices
{
	struct XMLHelper
	{
	public:
		template<class T>
		static const bool extractTagValue(const tinyxml2::XMLElement * root, const std::string & tag, T & value)
		{
			auto element = root->FirstChildElement(tag.c_str());

			if (element != nullptr){
				try{
					value = boost::lexical_cast<T>(element->GetText());
					return true;
				}
				catch (...){
					return false;
				}
			}

			return false;
		}

		template<class T>
		static const bool extractAttributeValue(const tinyxml2::XMLElement * root, const std::string & attribute, T & value)
		{
			auto attrib = root->FindAttribute(attribute.c_str());
			if (attrib != nullptr){
				try{
					value = boost::lexical_cast<T>(std::string(attrib->Value()));
					return true;
				}
				catch (...){
					return false;
				}
			}

			return false;
		}

		template<>
		static const bool extractTagValue<std::string>(const tinyxml2::XMLElement * root, const std::string & tag, std::string & value)
		{
			auto element = root->FirstChildElement(tag.c_str());

			if (element != nullptr){
				auto ret = element->GetText();
				if (ret != nullptr){
					value = std::string(ret);
				}

				return true;
			}

			return false;
		}

		template<>
		static const bool extractAttributeValue<std::string>(const tinyxml2::XMLElement * root, const std::string & attribute, std::string & value)
		{
			auto attrib = root->FindAttribute(attribute.c_str());

			if (attrib != nullptr){
				value = attrib->Value();
				return true;
			}

			return false;
		}

		template<typename T>
		static const bool extractAndConvertAttributeValue(const tinyxml2::XMLElement * root, const std::string & attribute, typename T::Type & value)
		{
			bool ret = false;

			std::string sValue;
			if (extractAttributeValue(root, attribute.c_str(), sValue) == true){
				value = T::convert(sValue);
				ret = true;
			}

			return ret;
		}

		template<typename T>
		static const bool extractAndConvertTagValue(const tinyxml2::XMLElement * root, const std::string & tag, typename T::Type & value)
		{
			bool ret = false;

			std::string sValue;
			if (extractTagValue(root, tag.c_str(), sValue) == true){
				value = T::convert(sValue);
				ret = true;
			}

			return ret;
		}

		static const bool extractAttributes(const tinyxml2::XMLElement * root, xmlWsdl::Attributes & attributes);

		static const bool extractEnumValues(const tinyxml2::XMLElement * root, xmlWsdl::EnumValueList & enumValues);

		static void extractAnnotation(const tinyxml2::XMLElement * element, xmlWsdl::Annotation & annotation);

		static void extractUserGroup(const tinyxml2::XMLElement * element, xmlWsdl::UserGroup & userGroup);

		static void extractUserDetails(const tinyxml2::XMLElement * element, xmlWsdl::UserDetails & userDetails);

		//! TODO
		//! ta metoda albo poprzednia musi znikn¹æ po ujednoliceniu opisu usera w odpowiedzi us³ug
		static void _extractUserDetails(const tinyxml2::XMLElement * element, xmlWsdl::UserDetails & userDetails);

		static void extractUserData(const tinyxml2::XMLElement * element, xmlWsdl::User & user);

		static void extractPerformer(const tinyxml2::XMLElement * element, xmlWsdl::PerformerDetails & performer);

		static void extractSession(const tinyxml2::XMLElement * element, xmlWsdl::SessionDetails & session);

		static void extractTrial(const tinyxml2::XMLElement * element, xmlWsdl::TrialDetails & trial);

		static void extractFile(const tinyxml2::XMLElement * element, xmlWsdl::FileDetails & file);

		static void extractMeasurementConf(const tinyxml2::XMLElement * element, xmlWsdl::MeasurementConfDetails & mcDetails);

		static void extractPerformerConf(const tinyxml2::XMLElement * element, xmlWsdl::PerformerConfDetails & pcDetails);

		static void extractAttribute(const tinyxml2::XMLElement * element, xmlWsdl::AttributeDefinition & attributeDef);

		static void extractAttributeGroup(const tinyxml2::XMLElement * element, xmlWsdl::AttributeGroupDefinition & attributeGroupDef);

		static void extractSessionGroup(const tinyxml2::XMLElement * element, xmlWsdl::SessionGroupDefinition & sessionGroupDef);

		static void extractMotionKind(const tinyxml2::XMLElement * element, xmlWsdl::MotionKindDefinition & motionKind);

		static void extractShallowCopyAttributes(const tinyxml2::XMLElement * element, MotionShallowCopy::Attrs & attributes);

		static void extractShallowCopyFile(const tinyxml2::XMLElement * element, MotionShallowCopy::File & file);
	};
}

#endif	// __HEADER_GUARD_HMDBSERVICES__XMLHELPER_H__