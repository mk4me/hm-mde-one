#include "CommunicationPCH.h"
#include <plugins/communication/MetadataParser.h>

MotionMetadataParser::MotionMetadataParser() : metadata(new communication::MotionMetaData::MetaData()),
    object(core::ObjectWrapper::create<communication::MotionMetaData::MetaData>())
{
    object->set(metadata);
}

MotionMetadataParser::~MotionMetadataParser()
{
}

void MotionMetadataParser::parseFile(const core::Filesystem::Path& path)
{
    this->path = path;
    
    TiXmlDocument document(path.string());
    if(!document.LoadFile()) {
        UTILS_ASSERT(false, "Blad wczytania pliku MotionMetadata");
    }
    TiXmlHandle hDocument(&document);
    TiXmlElement* _element;
    TiXmlHandle hParent(0);

    _element = hDocument.FirstChildElement().Element();
    if(!_element) {
        UTILS_ASSERT(false, "Blad wczytania z pliku MotionMetadata");
    }
    hParent = TiXmlHandle(_element);

    //SessionGroups
    TiXmlElement* session_goups_element = hParent.FirstChild("SessionGroups").ToElement();
    if(session_goups_element) {
        TiXmlElement* session_goup_element = session_goups_element->FirstChildElement("SessionGroup");
        while(session_goup_element) {
            communication::MotionMetaData::SessionGroup sessionGroup;
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
            communication::MotionMetaData::MotionKind motionKind;
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
            communication::MotionMetaData::Lab lab;
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
            communication::MotionMetaData::AttributeGroup attributeGroup;
            attribute_group_element->QueryIntAttribute("AttributeGroupID", &attributeGroup.attributeGroupID);
            attribute_group_element->QueryStringAttribute("AttributeGroupName", &attributeGroup.attributeGroupName);
            attribute_group_element->QueryStringAttribute("DescribedEntity", &attributeGroup.describedEntity);

            //Attributes
            TiXmlElement* attrs_element = attribute_group_element->FirstChildElement("Attributes");
            if(attrs_element) {
                TiXmlElement* attr_element = attrs_element->FirstChildElement("Attribute");
                while(attr_element) {
                    communication::MotionMetaData::Attribute attribute;
                    attr_element->QueryStringAttribute("AttributeName", &attribute.attributeName);
                    attr_element->QueryStringAttribute("AttributeType", &attribute.attributeType);
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

            metadata->attributeGroups.push_back(attributeGroup);
            attribute_group_element = attribute_group_element->NextSiblingElement();
        }
    }
    LOG_INFO("Motion metadata parsed.");
}

core::IParser* MotionMetadataParser::create()
{
    return new MotionMetadataParser();
}

void MotionMetadataParser::getSupportedExtensions(core::IParser::Extensions & extensions) const
{
    core::IParser::ExtensionDescription desc;

    desc.types.insert(typeid(communication::MotionMetaData::MetaData));
    desc.description = "Metadata from motion DB";
    extensions["xml"] = desc;
}

const communication::MotionMetaData::MetaData& MotionMetadataParser::getMetadata() const
{
    return *metadata;
}

void MotionMetadataParser::getObjects( core::Objects& objects )
{
    objects.insert(object);
}



MedicalMetadataParser::MedicalMetadataParser() : metadata(new communication::MedicalMetaData::MetaData()),
    object(core::ObjectWrapper::create<communication::MedicalMetaData::MetaData>())
{
    object->set(metadata);
}

MedicalMetadataParser::~MedicalMetadataParser()
{
}

void MedicalMetadataParser::parseFile(const core::Filesystem::Path& path)
{
    this->path = path;

    TiXmlDocument document(path.string());
    if(!document.LoadFile()) {
        UTILS_ASSERT(false, "Blad wczytania pliku MedicalMetadata");
    }
    TiXmlHandle hDocument(&document);
    TiXmlElement* _element;
    TiXmlHandle hParent(0);

    _element = hDocument.FirstChildElement().Element();
    if(!_element) {
        UTILS_ASSERT(false, "Blad wczytania z pliku MedicalMetadata");
    }
    hParent = TiXmlHandle(_element);

    //ExamTypes
    TiXmlElement* exam_types_element = hParent.FirstChild("ExamTypes").ToElement();
    if(exam_types_element) {
        TiXmlElement* exam_type_element = exam_types_element->FirstChildElement("ExamType");
        while(exam_type_element) {
            communication::MedicalMetaData::ExamType examType;
            exam_type_element->QueryIntAttribute("ExamTypeID", &examType.id);
            exam_type_element->QueryStringAttribute("ExamTypeName", &examType.name);

            metadata->examTypes[examType.id] = examType;
            exam_type_element = exam_type_element->NextSiblingElement();
        }
    }
    //Disorders
    TiXmlElement* disorders_element = hParent.FirstChild("Disorders").ToElement();
    if(disorders_element) {
        TiXmlElement* disorder_element = disorders_element->FirstChildElement("Disorder");
        while(disorder_element) {
            communication::MedicalMetaData::DisorderType disorderType;
            disorder_element->QueryIntAttribute("DisorderID", &disorderType.id);
            disorder_element->QueryStringAttribute("DisorderName", &disorderType.name);

            metadata->disorderTypes[disorderType.id] = disorderType;
            disorder_element = disorder_element->NextSiblingElement();
        }
    }
    
    LOG_INFO("Medical metadata parsed.");
}

core::IParser* MedicalMetadataParser::create()
{
    return new MedicalMetadataParser();
}

void MedicalMetadataParser::getSupportedExtensions(core::IParser::Extensions & extensions) const
{
    core::IParser::ExtensionDescription desc;

    desc.types.insert(typeid(communication::MotionMetaData::MetaData));
    desc.description = "Metadata from medical DB";
    extensions["xml"] = desc;
}

const communication::MedicalMetaData::MetaData& MedicalMetadataParser::getMetadata() const
{
    return *metadata;
}

void MedicalMetadataParser::getObjects( core::Objects& objects )
{
    objects.insert(object);
}
