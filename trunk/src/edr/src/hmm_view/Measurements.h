/********************************************************************
	created:	2012/03/30
	created:	30:3:2012   12:11
	filename: 	Measurements.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__MEASUREMENTS_H__
#define HEADER_GUARD_HMM__MEASUREMENTS_H__

#include <QtCore/QString>
#include <vector>
#include <core/SmartPtr.h>

class MeasurementConfig
{
    friend class MeasurementsParser;

public:
    virtual ~MeasurementConfig() {}

    QString tr(const QString& text) const;
    bool hasEntry(const QString& text) const;

    const QString& getName() const { return name; }
    int getNumber() const { return number; }

private:
    QString name;
    int number;
    std::map<QString, QString> namesMap;
};
typedef core::shared_ptr<MeasurementConfig> MeasurementConfigPtr;
typedef core::shared_ptr<const MeasurementConfig> MeasurementConfigConstPtr;

class Measurements
{
public:
    typedef std::pair<QString, QString> Entry;

public:
    Measurements() {}
    virtual ~Measurements() {}
   
public:
    void addConfig(MeasurementConfigPtr config);

    MeasurementConfigConstPtr getConfig(const QString& name) const;
    MeasurementConfigConstPtr getConfig(int number) const;

    MeasurementConfigConstPtr tryGetConfig(const QString& name) const;
    MeasurementConfigConstPtr tryGetConfig(int number) const;

    bool hasConfig(const QString& name) const;
    bool hasConfig(int number) const;

private:
    std::map<int, MeasurementConfigPtr> configsByNumber;
    std::map<QString, MeasurementConfigPtr> configsByName;
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
