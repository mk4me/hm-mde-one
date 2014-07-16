#include <webserviceslib/MetadataParser.h>
#include <webserviceslib/Entity.h>
#include <utils/Debug.h>
#include <tinyxml2.h>

namespace webservices
{

void MotionMetadataParser::parseFile(const std::string & path, MotionMetaData::MetaData & metadata)
{    
    tinyxml2::XMLDocument document;
    if (document.LoadFile(path.c_str()) != tinyxml2::XML_NO_ERROR) {
        UTILS_ASSERT(false, "Blad wczytania pliku MotionMetadata");
    }
    tinyxml2::XMLHandle hDocument(&document);
    tinyxml2::XMLElement* _element;
    tinyxml2::XMLHandle hParent(0);

    _element = hDocument.FirstChildElement().ToElement();
    if(!_element) {
        UTILS_ASSERT(false, "Blad wczytania z pliku MotionMetadata");
    }
    hParent = tinyxml2::XMLHandle(_element);

    //SessionGroups
    tinyxml2::XMLElement* session_goups_element = hParent.FirstChildElement("SessionGroups").ToElement();
    if(session_goups_element) {
        tinyxml2::XMLElement* session_goup_element = session_goups_element->FirstChildElement("SessionGroup")->ToElement();
        while(session_goup_element) {
            MotionMetaData::SessionGroup sessionGroup;
            session_goup_element->QueryIntAttribute("SessionGroupID", &sessionGroup.sessionGroupID);
            sessionGroup.sessionGroupName = utils::safeString( session_goup_element->Attribute("SessionGroupName"));

            metadata.sessionGroups[sessionGroup.sessionGroupID] = sessionGroup;
            session_goup_element = session_goup_element->NextSiblingElement();
        }
    }
    //MotionKinds
    tinyxml2::XMLElement* motion_kinds_element = hParent.FirstChildElement("MotionKinds").ToElement();
    if(motion_kinds_element) {
        tinyxml2::XMLElement* motion_kind_element = motion_kinds_element->FirstChildElement("MotionKind");
        while(motion_kind_element) {
            MotionMetaData::MotionKind motionKind;
            motionKind.motionKindName = utils::safeString( motion_kind_element->Attribute("MotionKindName"));

            metadata.motionKinds.push_back(motionKind);
            motion_kind_element = motion_kind_element->NextSiblingElement();
        }
    }
    //Labs
    tinyxml2::XMLElement* labs_element = hParent.FirstChildElement("Labs").ToElement();
    if(labs_element) {
        tinyxml2::XMLElement* lab_element = labs_element->FirstChildElement("Lab")->ToElement();
        while(lab_element) {
            MotionMetaData::Lab lab;
            lab_element->QueryIntAttribute("LabID", &lab.labID);
            lab.labName = utils::safeString( lab_element->Attribute("LabName"));

            metadata.labs[lab.labID] = lab;
            lab_element = lab_element->NextSiblingElement();
        }
    }
    //AttributeGroups
    tinyxml2::XMLElement* attribute_groups_element = hParent.FirstChildElement("AttributeGroups").ToElement();
    if(attribute_groups_element) {
        tinyxml2::XMLElement* attribute_group_element = attribute_groups_element->FirstChildElement("AttributeGroup");
        while(attribute_group_element) {
            MotionMetaData::AttributeGroup attributeGroup;
            attribute_group_element->QueryIntAttribute("AttributeGroupID", &attributeGroup.attributeGroupID);
            attributeGroup.attributeGroupName = utils::safeString( attribute_group_element->Attribute("AttributeGroupName"));
			{
				std::string describedEntity;
				describedEntity = utils::safeString( attribute_group_element->Attribute("DescribedEntity"));
				attributeGroup.describedEntity = xmlWsdl::Entity::convert(describedEntity);
			}

            //Attributes
            tinyxml2::XMLElement* attrs_element = attribute_group_element->FirstChildElement("Attributes");
            if(attrs_element) {
                tinyxml2::XMLElement* attr_element = attrs_element->FirstChildElement("Attribute");
                while(attr_element) {
                    MotionMetaData::Attribute attribute;
                    attribute.attributeName = utils::safeString( attr_element->Attribute("AttributeName"));

					{
						std::string attributeType;
						attributeType = utils::safeString( attr_element->Attribute("AttributeType"));
						attribute.attributeType = xmlWsdl::AttributeType::convert(attributeType);
					}

                    attribute.unit = utils::safeString( attr_element->Attribute("Unit"));
                    //EnumValues
                    /*tinyxml2::XMLElement* enum_values_element = attr_element->FirstChildElement("EnumValues");
                    if(enum_values_element) {
                        tinyxml2::XMLElement* enum_value_element = enum_values_element->FirstChildElement("Enumeration");
                        while(enum_value_element) {
                            communication::MotionMetaData::Enumeration enumeration;
                            enumeration.enumValue = utils::safeString( enum_value_element->Attribute("EnumValue"));

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
    tinyxml2::XMLDocument document;
    if(document.LoadFile(path.c_str()) != tinyxml2::XML_NO_ERROR) {
        UTILS_ASSERT(false, "Blad wczytania pliku MedicalMetadata");
    }
    tinyxml2::XMLHandle hDocument(&document);
    tinyxml2::XMLElement* _element;
    tinyxml2::XMLHandle hParent(0);

    _element = hDocument.FirstChildElement().ToElement();
    if(!_element) {
        UTILS_ASSERT(false, "Blad wczytania z pliku MedicalMetadata");
    }
    hParent = tinyxml2::XMLHandle(_element);

    //ExamTypes
    tinyxml2::XMLElement* exam_types_element = hParent.FirstChildElement("ExamTypes").ToElement();
    if(exam_types_element) {
        tinyxml2::XMLElement* exam_type_element = exam_types_element->FirstChildElement("ExamType");
        while(exam_type_element) {
            MedicalMetaData::ExamType examType;
            exam_type_element->QueryIntAttribute("ExamTypeID", &examType.examTypeID);
            examType.name = utils::safeString( exam_type_element->Attribute("ExamTypeName"));

            metadata.examTypes[examType.examTypeID] = examType;
            exam_type_element = exam_type_element->NextSiblingElement();
        }
    }
    //Disorders
    tinyxml2::XMLElement* disorders_element = hParent.FirstChildElement("Disorders").ToElement();
    if(disorders_element) {
        tinyxml2::XMLElement* disorder_element = disorders_element->FirstChildElement("Disorder");
        while(disorder_element) {
            MedicalMetaData::DisorderType disorderType;
            disorder_element->QueryIntAttribute("DisorderID", &disorderType.disorderTpeID);
            disorderType.name = utils::safeString( disorder_element->Attribute("DisorderName"));

            metadata.disorderTypes[disorderType.disorderTpeID] = disorderType;
            disorder_element = disorder_element->NextSiblingElement();
        }
    }
}

}
