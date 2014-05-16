#include <hmdbserviceslib/MetadataParser.h>
#include <hmdbserviceslib/Entity.h>
#include <utils/Debug.h>
#include <tinyxml.h>

namespace hmdbServices
{
	void MotionMetadataParser::parseFile(const std::string & path, MotionMetaData::MetaData & metadata)
	{
		TiXmlDocument document(path);
		if (!document.LoadFile()) {
			UTILS_ASSERT(false, "Blad wczytania pliku MotionMetadata");
		}
		TiXmlHandle hDocument(&document);
		TiXmlElement* _element;
		TiXmlHandle hParent(0);

		_element = hDocument.FirstChildElement().Element();
		if (!_element) {
			UTILS_ASSERT(false, "Blad wczytania z pliku MotionMetadata");
		}
		hParent = TiXmlHandle(_element);

		//SessionGroups
		TiXmlElement* session_goups_element = hParent.FirstChild("SessionGroups").ToElement();
		if (session_goups_element) {
			TiXmlElement* session_goup_element = session_goups_element->FirstChildElement("SessionGroup");
			while (session_goup_element) {
				MotionMetaData::SessionGroup sessionGroup;
				session_goup_element->QueryIntAttribute("SessionGroupID", &sessionGroup.sessionGroupID);
				session_goup_element->QueryStringAttribute("SessionGroupName", &sessionGroup.sessionGroupName);

				metadata.sessionGroups[sessionGroup.sessionGroupID] = sessionGroup;
				session_goup_element = session_goup_element->NextSiblingElement();
			}
		}
		//MotionKinds
		TiXmlElement* motion_kinds_element = hParent.FirstChild("MotionKinds").ToElement();
		if (motion_kinds_element) {
			TiXmlElement* motion_kind_element = motion_kinds_element->FirstChildElement("MotionKind");
			while (motion_kind_element) {
				MotionMetaData::MotionKind motionKind;
				motion_kind_element->QueryStringAttribute("MotionKindName", &motionKind.motionKindName);

				metadata.motionKinds.push_back(motionKind);
				motion_kind_element = motion_kind_element->NextSiblingElement();
			}
		}
		//Labs
		TiXmlElement* labs_element = hParent.FirstChild("Labs").ToElement();
		if (labs_element) {
			TiXmlElement* lab_element = labs_element->FirstChildElement("Lab");
			while (lab_element) {
				MotionMetaData::Lab lab;
				lab_element->QueryIntAttribute("LabID", &lab.labID);
				lab_element->QueryStringAttribute("LabName", &lab.labName);

				metadata.labs[lab.labID] = lab;
				lab_element = lab_element->NextSiblingElement();
			}
		}
		//AttributeGroups
		TiXmlElement* attribute_groups_element = hParent.FirstChild("AttributeGroups").ToElement();
		if (attribute_groups_element) {
			TiXmlElement* attribute_group_element = attribute_groups_element->FirstChildElement("AttributeGroup");
			while (attribute_group_element) {
				MotionMetaData::AttributeGroup attributeGroup;
				attribute_group_element->QueryIntAttribute("AttributeGroupID", &attributeGroup.attributeGroupID);
				attribute_group_element->QueryStringAttribute("AttributeGroupName", &attributeGroup.attributeGroupName);
				{
					std::string describedEntity;
					attribute_group_element->QueryStringAttribute("DescribedEntity", &describedEntity);
					attributeGroup.describedEntity = xmlWsdl::Entity::convert(describedEntity);
				}

				//Attributes
				TiXmlElement* attrs_element = attribute_group_element->FirstChildElement("Attributes");
				if (attrs_element) {
					TiXmlElement* attr_element = attrs_element->FirstChildElement("Attribute");
					while (attr_element) {
						MotionMetaData::Attribute attribute;
						attr_element->QueryStringAttribute("AttributeName", &attribute.attributeName);

						{
							std::string attributeType;
							attr_element->QueryStringAttribute("AttributeType", &attributeType);
							attribute.attributeType = xmlWsdl::AttributeType::convert(attributeType);
						}

						attr_element->QueryStringAttribute("Unit", &attribute.unit);
						//EnumValues
						/*TiXmlElement* enum_values_element = attr_element->FirstChildElement("EnumValues");
						if(enum_values_element) {
						TiXmlElement* enum_value_element = enum_values_element->FirstChildElement("Enumeration");
						while(enum_value_element) {
						communication::MotionMetaData::Enumeration enumeration;
						enum_value_element->QueryStringAttribute("EnumValue", &enumeration.enumValue);

						attribute.enumValues.push_back(enumeration);
						enum_value_element = enum_value_element->NextSiblingElement();
						}
						}*/
						//attributeGroup.attributes.push_back(attribute);
						attributeGroup.attributes[attribute.attributeName] = attribute;
						attr_element = attr_element->NextSiblingElement();
					}
				}

				metadata.attributeGroups[attributeGroup.attributeGroupID] = attributeGroup;
				attribute_group_element = attribute_group_element->NextSiblingElement();
			}
		}
	}

	void MedicalMetadataParser::parseFile(const std::string & path, MedicalMetaData::MetaData & metadata)
	{
		TiXmlDocument document(path);
		if (!document.LoadFile()) {
			UTILS_ASSERT(false, "Blad wczytania pliku MedicalMetadata");
		}
		TiXmlHandle hDocument(&document);
		TiXmlElement* _element;
		TiXmlHandle hParent(0);

		_element = hDocument.FirstChildElement().Element();
		if (!_element) {
			UTILS_ASSERT(false, "Blad wczytania z pliku MedicalMetadata");
		}
		hParent = TiXmlHandle(_element);

		//ExamTypes
		TiXmlElement* exam_types_element = hParent.FirstChild("ExamTypes").ToElement();
		if (exam_types_element) {
			TiXmlElement* exam_type_element = exam_types_element->FirstChildElement("ExamType");
			while (exam_type_element) {
				MedicalMetaData::ExamType examType;
				exam_type_element->QueryIntAttribute("ExamTypeID", &examType.examTypeID);
				exam_type_element->QueryStringAttribute("ExamTypeName", &examType.name);

				metadata.examTypes[examType.examTypeID] = examType;
				exam_type_element = exam_type_element->NextSiblingElement();
			}
		}
		//Disorders
		TiXmlElement* disorders_element = hParent.FirstChild("Disorders").ToElement();
		if (disorders_element) {
			TiXmlElement* disorder_element = disorders_element->FirstChildElement("Disorder");
			while (disorder_element) {
				MedicalMetaData::DisorderType disorderType;
				disorder_element->QueryIntAttribute("DisorderID", &disorderType.disorderTpeID);
				disorder_element->QueryStringAttribute("DisorderName", &disorderType.name);

				metadata.disorderTypes[disorderType.disorderTpeID] = disorderType;
				disorder_element = disorder_element->NextSiblingElement();
			}
		}
	}
}