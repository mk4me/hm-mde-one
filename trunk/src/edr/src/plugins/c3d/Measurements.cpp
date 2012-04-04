#include "C3DPCH.h"
#include <iostream>
#include <tinyxml.h>
#include "Measurements.h"

void MeasurementsParser::parse( const std::string& filename )
{
    measurments = MeasurementsPtr(new Measurements());
    TiXmlDocument doc(filename.c_str());
    bool loadOkay = doc.LoadFile();
    TiXmlElement* rootElement = nullptr;
    // jesli plik jest plikiem *xml ...
    if (loadOkay) {
        TiXmlElement* docElement = doc.FirstChildElement();
        while (docElement) {
            // szukanie glownego elementu - "MeasurementConfs"
            if (strcmp(docElement->Value(), "MeasurementConfs") == 0) {
                rootElement = docElement;
                break;
            }
            docElement = docElement->NextSiblingElement();
        }
    } else {
        // oczyt nie udal sie z jakiegos powodu
        throw std::runtime_error(std::string("Unable to load : ") + filename);
    }

    // jesli plik zawiera odpowiedni korzen (root)
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
            config->name = attrib->Value();
        }
        attrib = attrib->Next();
    }
    if (attrs && strcmp(attrs->Value(), "Attrs") == 0) {
        if (attrs->NextSiblingElement()) {
            throw std::runtime_error("Wrong measurments xml file");
        }
        for (TiXmlElement* a = attrs->FirstChildElement(); a != nullptr; a = a->NextSiblingElement()) {
            if (a && strcmp(a->Value(), "A") == 0) {
                Measurements::Entry entry;
                readAttribute(a, entry);
                config->namesMap.insert(entry);
            }
        }
    }
}

void MeasurementsParser::readAttribute( TiXmlElement* A, Measurements::Entry& pair )
{
    TiXmlAttribute* attrib = A->FirstAttribute();
    while(attrib) {
        if (strcmp(attrib->Name(), "Name") == 0) {
            pair.first = attrib->Value();
        } else if (strcmp(attrib->Name(), "Value") == 0) {
            pair.second = attrib->Value();
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
        const std::string& name = config->getName();
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

MeasurementConfigConstPtr Measurements::getConfig( const std::string& name ) const
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

MeasurementConfigConstPtr Measurements::tryGetConfig( const std::string& name ) const
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

bool Measurements::hasConfig( const std::string& name ) const
{
    return configsByName.find(name) != configsByName.end();
}

bool Measurements::hasConfig( int number ) const
{
    return configsByNumber.find(number) != configsByNumber.end();
}

bool MeasurementConfig::hasEntry( const std::string& text ) const
{
    auto it = namesMap.find(text);
    return it != namesMap.end();
}

std::string MeasurementConfig::tr( const std::string& text ) const
{
    auto it = namesMap.find(text);
    if (it != namesMap.end()) {
        return it->second;
    }
    return text;
}
