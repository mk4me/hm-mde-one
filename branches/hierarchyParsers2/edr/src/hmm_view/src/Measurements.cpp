#include "hmmPCH.h"
#include <iostream>
#include <tinyxml.h>
#include "Measurements.h"
#include <corelib/ILog.h>

MeasurementConfig::MeasurementConfig() : 
    number(-1)
{
}

void MeasurementsParser::parse( const std::string& filename )
{
    measurments = MeasurementsPtr(new Measurements());
    TiXmlDocument doc(filename);
    bool loadOkay = doc.LoadFile();
    TiXmlElement* rootElement = nullptr;
    // jeśli plik jest plikiem *xml ...
    if (loadOkay) {
        TiXmlElement* docElement = doc.FirstChildElement();
        while (docElement) {
            // szukanie głównego elementu - "MeasurementConfs"
            if (strcmp(docElement->Value(), "MeasurementConfs") == 0) {
                rootElement = docElement;
                break;
            }
            docElement = docElement->NextSiblingElement();
        }
    } else {
        // oczyt nie udal się z jakiegoś powodu
        throw std::runtime_error(std::string("Unable to load : ") + filename);
    }

    // jeśli plik zawiera odpowiedni korzen (root)
    if (rootElement) {
        for (TiXmlElement* element = rootElement->FirstChildElement(); element != nullptr; element = element->NextSiblingElement()) {
            if (element && strcmp(element->Value(), "MeasurementConf") == 0) {
                MeasurementConfigPtr config(new MeasurementConfig());
                readMeasurment(element, config);
                measurments->addConfig(config);
            }
        }
    } else {
        std::runtime_error(filename + " does not have root element");
    }
    loaded = true;
}

void MeasurementsParser::readMeasurment( TiXmlElement* element, MeasurementConfigPtr config )
{
    TiXmlElement* attrs = element->FirstChildElement(); 
    TiXmlAttribute* attrib = element->FirstAttribute();
    while(attrib) {
        if (strcmp(attrib->Name(), "MeasurementConfID") == 0) {
            config->number = attrib->IntValue();
        } else if (strcmp(attrib->Name(), "Name") == 0) {
            config->name = QString::fromUtf8(attrib->Value());
        }
        attrib = attrib->Next();
    }
    if (attrs && strcmp(attrs->Value(), "Attrs") == 0) {
        if (attrs->NextSiblingElement()) {
            throw std::runtime_error("Wrong measurments xml file");
        }
        for (TiXmlElement* a = attrs->FirstChildElement(); a != nullptr; a = a->NextSiblingElement()) {
            if (a && strcmp(a->Value(), "A") == 0) {
                MeasurementConfig::MapEntry entry;
                readAttribute(a, entry);
                config->entry2Data.insert(entry);
            }
        }
    }
}

void MeasurementsParser::readAttribute( TiXmlElement* A, MeasurementConfig::MapEntry& entry )
{
    TiXmlAttribute* attrib = A->FirstAttribute();
    while(attrib) {
        if (strcmp(attrib->Name(), "Name") == 0) {
            entry.first = QString::fromUtf8(attrib->Value());
        } else if (strcmp(attrib->Name(), "Value") == 0) {
            entry.second.name = QString::fromUtf8(attrib->Value());
        } else if (strcmp(attrib->Name(), "Id") == 0) {
            entry.second.id = QString::fromUtf8(attrib->Value());
        }
        attrib = attrib->Next();
    }
}

MeasurementsConstPtr MeasurementsParser::getMeasurments()
{
    UTILS_ASSERT(isLoaded());
    return measurments;
}

MeasurementsParser::MeasurementsParser() : 
    loaded(false)
{

}


void Measurements::addConfig( MeasurementConfigPtr config )
{
    if (config) {
        const QString& name = config->getName();
        int number = config->getNumber();
        if (!hasConfig(name) && !hasConfig(number)) {
            configsByName[name] = config;
            configsByNumber[number] = config;
        } else {
            throw std::runtime_error("Config already added");
        }
    } else {
        throw std::runtime_error("Null config was passed");
    }
}

MeasurementConfigConstPtr Measurements::getConfig( const QString& name ) const
{
    auto it = configsByName.find(name);
    if (it != configsByName.end()) {
        return it->second;
    }
    
    throw std::runtime_error("Config not found");
}

MeasurementConfigConstPtr Measurements::getConfig( int number ) const
{
    auto it = configsByNumber.find(number);
    if (it != configsByNumber.end()) {
        return it->second;
    } 

    throw std::runtime_error("Config not found");
}

MeasurementConfigConstPtr Measurements::tryGetConfig( const QString& name ) const
{
    if (hasConfig(name)) {
        return getConfig(name);
    }

    return MeasurementConfigConstPtr();
}

MeasurementConfigConstPtr Measurements::tryGetConfig( int number ) const
{
    if (hasConfig(number)) {
        return getConfig(number);
    }

    return MeasurementConfigConstPtr();
}

bool Measurements::hasConfig( const QString& name ) const
{
    return configsByName.find(name) != configsByName.end();
}

bool Measurements::hasConfig( int number ) const
{
    return configsByNumber.find(number) != configsByNumber.end();
}

MeasurementsConstPtr Measurements::measurements;

MeasurementsConstPtr Measurements::get()
{
    if (!measurements) {
        std::string p = plugin::getResourcePath("schemas/measurementconfs.xml").string();
        try {
            MeasurementsParser parser;
            parser.parse(p);
            measurements = parser.getMeasurments();
        } catch (std::runtime_error& e) {
            PLUGIN_LOG_ERROR(e.what());
        }
    }

    return measurements;
}

bool MeasurementConfig::hasEntry( const QString& text ) const
{
    auto it = entry2Data.find(text);
    return it != entry2Data.end();
}

QString MeasurementConfig::tr( const QString& text ) const
{
    auto it = entry2Data.find(text);
    if (it != entry2Data.end()) {
        return it->second.name;
    }
    return text;
}

QString MeasurementConfig::getIdByName(const QString& name) const
{
    for (auto it = entry2Data.cbegin(); it != entry2Data.cend(); ++it) {
        if (it->second.name == name) {
            return it->second.id;
        }
    }

    return QString();
}

