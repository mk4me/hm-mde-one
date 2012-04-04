/********************************************************************
	created:	2012/03/30
	created:	30:3:2012   12:11
	filename: 	Measurements.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_C3DPLUGIN__MEASUREMENTS_H__
#define HEADER_GUARD_C3DPLUGIN__MEASUREMENTS_H__

#include <string>
#include <vector>
#include <core/SmartPtr.h>
#include <plugins/c3d/IMeasurementConfig.h>

class MeasurementConfig : public IMeasurementConfig
{
    friend class MeasurementsParser;

public:
    virtual ~MeasurementConfig() {}

    std::string tr(const std::string& text) const;
    bool hasEntry(const std::string& text) const;

    const std::string& getName() const { return name; }
    int getNumber() const { return number; }

private:
    std::string name;
    int number;
    std::map<std::string, std::string> namesMap;
};
typedef core::shared_ptr<MeasurementConfig> MeasurementConfigPtr;
typedef core::shared_ptr<const MeasurementConfig> MeasurementConfigConstPtr;

class Measurements
{
public:
    typedef std::pair<std::string, std::string> Entry;

public:
    Measurements() {}
    virtual ~Measurements() {}
   
public:
    void addConfig(MeasurementConfigPtr config);

    MeasurementConfigConstPtr getConfig(const std::string& name) const;
    MeasurementConfigConstPtr getConfig(int number) const;

    MeasurementConfigConstPtr tryGetConfig(const std::string& name) const;
    MeasurementConfigConstPtr tryGetConfig(int number) const;

    bool hasConfig(const std::string& name) const;
    bool hasConfig(int number) const;

private:
    std::map<int, MeasurementConfigPtr> configsByNumber;
    std::map<std::string, MeasurementConfigPtr> configsByName;
};
typedef core::shared_ptr<Measurements> MeasurementsPtr;
typedef core::shared_ptr<const Measurements> MeasurementsConstPtr;


class MeasurementsParser
{
public:
    MeasurementsParser();
	virtual ~MeasurementsParser() {}
public:
    void parse(const std::string& filename);
    bool isLoaded() const { return loaded; }
    MeasurementsConstPtr getMeasurments();

private:
    void readMeasurment( TiXmlElement* element, MeasurementConfigPtr config );
    void readAttribute(TiXmlElement* A, Measurements::Entry& pair); 

private:
    bool loaded;
    MeasurementsPtr measurments;
};
typedef core::shared_ptr<MeasurementsParser> MeasurementsParserPtr;
typedef core::shared_ptr<const MeasurementsParser> MeasurementsParserConstPtr;


#endif
