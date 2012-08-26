#ifndef HEADER_GUARD_COMMUNICATION_METADATA_H__
#define HEADER_GUARD_COMMUNICATION_METADATA_H__

#include <string>
#include <vector>
#include <exception>
#include <boost/shared_ptr.hpp>

namespace MetaData
{

    struct SessionGroup
    {
        int sessionGroupID;
        std::string sessionGroupName;
    };

    typedef std::vector<SessionGroup> SessionGroups;

    struct MotionKind
    {
        std::string motionKindName;
    };

    typedef std::vector<MotionKind> MotionKinds;

    struct Lab
    {
        int labID;
        std::string labName;
    };

    typedef std::vector<Lab> Labs;

    struct Enumeration
    {
        std::string enumValue;
    };

    typedef std::vector<Enumeration> Enumerations;

    struct Attribute
    {
        Enumerations enumValues;
        std::string attributeName;
        std::string attributeType;
    };

    typedef std::vector<Attribute> Attributes;

    struct AttributeGroup
    {
        Attributes attributes;
        int attributeGroupID;
        std::string attributeGroupName;
        std::string describedEntity;
        std::string unit;
    };

    typedef std::vector<AttributeGroup> AttributeGroups;

    struct MetaData
    {
        SessionGroups sessionGroups;
        MotionKinds motionKinds;
        Labs labs;
        AttributeGroups attributeGroups;
    };

    typedef boost::shared_ptr<MetaData> MetaDataPtr;
    typedef boost::shared_ptr<const MetaData> MetaDataConstPtr;
    typedef boost::weak_ptr<MetaData> MetaDataWPtr;
    typedef boost::weak_ptr<const MetaData> MetaDataWConstPtr;

    
//MetaData loadMetadata(const std::string& filename)
//{
//    MetaData metaData;
//    TiXmlDocument document(filename);
//    if(!document.LoadFile())
//    {
//        LOG_ERROR(": !Blad wczytania pliku Metadata\n");
//        throw std::runtime_error("Blad wczytania pliku Metadata");
//    }
//    TiXmlHandle hDocument(&document);
//    TiXmlElement* _element;
//    TiXmlHandle hParent(0);
//
//    _element = hDocument.FirstChildElement().Element();
//    if(!_element)
//    {
//        LOG_ERROR(": !Blad czytania z pliku Metadata\n");
//        throw std::runtime_error("Blad czytania z pliku Metadata");
//    }
//    hParent = TiXmlHandle(_element);
//
//    //SessionGroups
//    TiXmlElement* session_goups_element = hParent.FirstChild("SessionGroups").ToElement();
//    if(session_goups_element)
//    {
//        TiXmlElement* session_goup_element = session_goups_element->FirstChildElement("SessionGroup");
//        while(session_goup_element)
//        {
//            SessionGroup sessionGroup;
//            session_goup_element->QueryIntAttribute("SessionGroupID", &sessionGroup.sessionGroupID);
//            session_goup_element->QueryStringAttribute("SessionGroupName", &sessionGroup.sessionGroupName);
//
//            metaData.sessionGroups.push_back(sessionGroup);
//            session_goup_element = session_goup_element->NextSiblingElement();
//        }
//    }
//    //MotionKinds
//    TiXmlElement* motion_kinds_element = hParent.FirstChild("MotionKinds").ToElement();
//    if(motion_kinds_element)
//    {
//        TiXmlElement* motion_kind_element = motion_kinds_element->FirstChildElement("MotionKind");
//        while(motion_kind_element)
//        {
//            MotionKind motionKind;
//            motion_kind_element->QueryStringAttribute("MotionKindName", &motionKind.motionKindName);
//
//            metaData.motionKinds.push_back(motionKind);
//            motion_kind_element = motion_kind_element->NextSiblingElement();
//        }
//    }
//    //Labs
//    TiXmlElement* labs_element = hParent.FirstChild("Labs").ToElement();
//    if(labs_element)
//    {
//        TiXmlElement* lab_element = labs_element->FirstChildElement("Lab");
//        while(lab_element)
//        {
//            Lab lab;
//            lab_element->QueryIntAttribute("LabID", &lab.labID);
//            lab_element->QueryStringAttribute("LabName", &lab.labName);
//
//            metaData.labs.push_back(lab);
//            lab_element = lab_element->NextSiblingElement();
//        }
//    }
//    //AttributeGroups
//    TiXmlElement* attribute_groups_element = hParent.FirstChild("AttributeGroups").ToElement();
//    if(attribute_groups_element)
//    {
//        TiXmlElement* attribute_group_element = attribute_groups_element->FirstChildElement("AttributeGroup");
//        while(attribute_group_element)
//        {
//            AttributeGroup attributeGroup;
//            attribute_group_element->QueryIntAttribute("AttributeGroupID", &attributeGroup.attributeGroupID);
//            attribute_group_element->QueryStringAttribute("AttributeGroupName", &attributeGroup.attributeGroupName);
//            attribute_group_element->QueryStringAttribute("DescribedEntity", &attributeGroup.describedEntity);
//            attribute_group_element->QueryStringAttribute("Unit", &attributeGroup.unit);
//
//            //Attributes
//            TiXmlElement* attrs_element = attribute_group_element->FirstChildElement("Attributes");
//            if(attrs_element)
//            {
//                TiXmlElement* attr_element = attrs_element->FirstChildElement("Attribute");
//                while(attr_element)
//                {
//                    Attribute attribute;
//                    attr_element->QueryStringAttribute("AttributeName", &attribute.attributeName);
//                    attr_element->QueryStringAttribute("AttributeType", &attribute.attributeType);
//
//                    //EnumValues
//                    TiXmlElement* enum_values_element = attr_element->FirstChildElement("EnumValues");
//                    if(enum_values_element)
//                    {
//                        TiXmlElement* enum_value_element = enum_values_element->FirstChildElement("Enumeration");
//                        while(enum_value_element)
//                        {
//                            communicationTest::Enumeration enumeration;
//                            enum_value_element->QueryStringAttribute("EnumValue", &enumeration.enumValue);
//
//                            attribute.enumValues.push_back(enumeration);
//                            enum_value_element = enum_value_element->NextSiblingElement();
//                        }
//                    }
//                    attributeGroup.attributes.push_back(attribute);
//                    attr_element = attr_element->NextSiblingElement();
//                }
//            }
//
//            metaData.attributeGroups.push_back(attributeGroup);
//            attribute_group_element = attribute_group_element->NextSiblingElement();
//        }
//    }
//    LOG_INFO("metadata parsed.");
//    return metaData;
//}

}
#endif //HEADER_GUARD_COMMUNICATION_METADATA_H__
