/********************************************************************
    created:  2011/10/28
    created:  28:10:2011   14:10
    filename: IPatient.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IPATIENT_H__
#define HEADER_GUARD___IPATIENT_H__

#include <QtGui/QPixmap>
#include <string>
#include <plugins/subject/ISubject.h>
#include <core/SmartPtr.h>
#include <boost/units/quantity.hpp>
#include <boost/units/systems/si.hpp>

struct Disorder
{
    std::string name;
    std::string diagnosisDate;
    std::string focus;
    std::string comments;
    std::map<std::string, std::string> attributes;
};

struct AntropometricData
{
    typedef boost::units::quantity<boost::units::si::length> length;
    typedef boost::units::quantity<boost::units::si::mass> mass;

    length height;
    length interAsisDistance;
    length leftLegLength;
    length rightLegLenght;
   /*     <A Name="LeftKneeWidth" Value="124" />
        <A Name="RightKneeWidth" Value="124" />
        <A Name="LeftAnkleWidth" Value="71" />
        <A Name="RightAnkleWidth" Value="71" />
        <A Name="LeftCircuitThigh" Value="0" />
        <A Name="RightCircuitThight" Value="0" />
        <A Name="LeftCircuitShank" Value="0" />
        <A Name="RightCircuitShank" Value="0" />
        <A Name="LeftShoulderOffset" Value="40" />
        <A Name="RightShoulderOffset" Value="40" />
        <A Name="LeftElbowWidth" Value="65" />
        <A Name="RightElbowWidth" Value="65" />
        <A Name="LeftWristWidth" Value="41" />
        <A Name="RightWristWidth" Value="41" />
        <A Name="LeftWristThickness" Value="0" />
        <A Name="RightWristThickness" Value="0" />
        <A Name="LeftHandWidth" Value="0" />
        <A Name="RightHandWidth" Value="0" />
        <A Name="LeftHandThickness" Value="26" />
        <A Name="RightHandThickness" Value="25" />*/
};

class IPatient
{
public:
    virtual ~IPatient() {};

    virtual PluginSubject::SubjectID getID() const = 0;
    virtual const std::string & getName() const = 0;
    virtual const std::string & getSurname() const = 0;
    virtual const std::string & getBirthday() const = 0;
    virtual char getGender() const = 0;
    virtual const core::shared_ptr<const QPixmap> & getPhoto() const = 0;

    virtual unsigned int disordersCount() const = 0;
    virtual const Disorder & disorder(unsigned int idx) const = 0;
};

typedef core::shared_ptr<IPatient> PatientPtr;
typedef core::shared_ptr<const IPatient> PatientConstPtr;

CORE_DEFINE_WRAPPER(IPatient, utils::PtrPolicyBoost, utils::ClonePolicyNotImplemented);

#endif HEADER_GUARD___IPATIENT_H__
