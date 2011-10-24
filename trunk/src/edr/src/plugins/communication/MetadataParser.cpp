#include "CommunicationPCH.h"
#include <plugins/communication/MetadataParser.h>

MetadataParser::MetadataParser() : metadata(new communication::MetaData::MetaData()),
    object(core::ObjectWrapper::create<communication::MetaData::MetaData>())
{
    object->set(metadata);
}

MetadataParser::~MetadataParser()
{
}

void MetadataParser::parseFile(const core::Filesystem::Path& path)
{
    this->path = path;
    
    TiXmlDocument document(path.string());
    if(!document.LoadFile()) {
        UTILS_ASSERT(false, "Blad wczytania pliku Metadata");
    }
    TiXmlHandle hDocument(&document);
    TiXmlElement* _element;
    TiXmlHandle hParent(0);

    _element = hDocument.FirstChildElement().Element();
    if(!_element) {
        UTILS_ASSERT(false, "Blad wczytania z pliku Metadata");
    }
    hParent = TiXmlHandle(_element);

    //SessionGroups
    TiXmlElement* session_goups_element = hParent.FirstChild("SessionGroups").ToElement();
    if(session_goups_element) {
        TiXmlElement* session_goup_element = session_goups_element->FirstChildElement("SessionGroup");
        while(session_goup_element) {
            communication::MetaData::SessionGroup sessionGroup;
            session_goup_element->QueryIntAttribute("SessionGroupID", &sessionGroup.sessionGroupID);
            session_goup_element->QueryStringAttribute("SessionGroupName", &sessionGroup.sessionGroupName);

            metadata->sessionGroups.push_back(sessionGroup);
            session_goup_element = session_goup_element->NextSiblingElement();
        }
    }
    //MotionKinds
    TiXmlElement* motion_kinds_element = hParent.FirstChild("MotionKinds").ToElement();
    if(motion_kinds_element) {
        TiXmlElement* motion_kind_element = motion_kinds_element->FirstChildElement("MotionKind");
        while(motion_kind_element) {
            communication::MetaData::MotionKind motionKind;
            motion_kind_element->QueryStringAttribute("MotionKindName", &motionKind.motionKindName);

            metadata->motionKinds.push_back(motionKind);
            motion_kind_element = motion_kind_element->NextSiblingElement();
        }
    }
    //Labs
    TiXmlElement* labs_element = hParent.FirstChild("Labs").ToElement();
    if(labs_element) {
        TiXmlElement* lab_element = labs_element->FirstChildElement("Lab");
        while(lab_element) {
            communication::MetaData::Lab lab;
            lab_element->QueryIntAttribute("LabID", &lab.labID);
            lab_element->QueryStringAttribute("LabName", &lab.labName);

            metadata->labs.push_back(lab);
            lab_element = lab_element->NextSiblingElement();
        }
    }
    //AttributeGroups
    TiXmlElement* attribute_groups_element = hParent.FirstChild("AttributeGroups").ToElement();
    if(attribute_groups_element) {
        TiXmlElement* attribute_group_element = attribute_groups_element->FirstChildElement("AttributeGroup");
        while(attribute_group_element) {
            communication::MetaData::AttributeGroup attributeGroup;
            attribute_group_element->QueryIntAttribute("AttributeGroupID", &attributeGroup.attributeGroupID);
            attribute_group_element->QueryStringAttribute("AttributeGroupName", &attributeGroup.attributeGroupName);
            attribute_group_element->QueryStringAttribute("DescribedEntity", &attributeGroup.describedEntity);
            attribute_group_element->QueryStringAttribute("Unit", &attributeGroup.unit);

            //Attributes
            TiXmlElement* attrs_element = attribute_group_element->FirstChildElement("Attributes");
            if(attrs_element) {
                TiXmlElement* attr_element = attrs_element->FirstChildElement("Attribute");
                while(attr_element) {
                    communication::MetaData::Attribute attribute;
                    attr_element->QueryStringAttribute("AttributeName", &attribute.attributeName);
                    attr_element->QueryStringAttribute("AttributeType", &attribute.attributeType);

                    //EnumValues
                    TiXmlElement* enum_values_element = attr_element->FirstChildElement("EnumValues");
                    if(enum_values_element) {
                        TiXmlElement* enum_value_element = enum_values_element->FirstChildElement("Enumeration");
                        while(enum_value_element) {
                            communication::MetaData::Enumeration enumeration;
                            enum_value_element->QueryStringAttribute("EnumValue", &enumeration.enumValue);

                            attribute.enumValues.push_back(enumeration);
                            enum_value_element = enum_value_element->NextSiblingElement();
                        }
                    }
                    attributeGroup.attributes.push_back(attribute);
                    attr_element = attr_element->NextSiblingElement();
                }
            }

            metadata->attributeGroups.push_back(attributeGroup);
            attribute_group_element = attribute_group_element->NextSiblingElement();
        }
    }
    LOG_INFO("metadata parsed.");
}

core::IParser* MetadataParser::create()
{
    return new MetadataParser();
}

void MetadataParser::getSupportedExtensions(core::IParser::Extensions & extensions) const
{
    core::IParser::ExtensionDescription desc;

    desc.types.insert(typeid(communication::MetaData::MetaData));
    desc.description = "Metadata from motion DB";
    extensions["xml"] = desc;
}

const communication::MetaData::MetaData& MetadataParser::getMetadata() const
{
    return *metadata;
}

void MetadataParser::getObjects( core::Objects& objects )
{
    objects.insert(object);
}
