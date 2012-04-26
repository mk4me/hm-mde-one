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
#include <boost/bimap.hpp>

class MeasurementConfig
{
    friend class MeasurementsParser;

public:
	MeasurementConfig() : 
		number(-1)
	{}
    virtual ~MeasurementConfig() {}

    QString tr(const QString& text) const;
    bool hasEntry(const QString& text) const;
    bool hasId(const QString& id) const;
    QString getEntryById(const QString& id) const;
    QString getNameById(const QString& id) const;
    QString getIdByName(const QString& name) const;

    const QString& getName() const { return name; }
    int getNumber() const { return number; }

private:
    struct EntryData { QString name; QString id; };
    typedef std::pair<QString, EntryData> MapEntry;
	
private:
    QString name;
    int number;
    std::map<QString, EntryData> entry2Data;
};
typedef core::shared_ptr<MeasurementConfig> MeasurementConfigPtr;
typedef core::shared_ptr<const MeasurementConfig> MeasurementConfigConstPtr;

class Measurements
{
public:
    static core::shared_ptr<const Measurements> get();
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
private:
    static core::shared_ptr<const Measurements> measurements;
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
    void readAttribute(TiXmlElement* A, MeasurementConfig::MapEntry& entry); 

private:
    bool loaded;
    MeasurementsPtr measurments;
};
typedef core::shared_ptr<MeasurementsParser> MeasurementsParserPtr;
typedef core::shared_ptr<const MeasurementsParser> MeasurementsParserConstPtr;


#endif
