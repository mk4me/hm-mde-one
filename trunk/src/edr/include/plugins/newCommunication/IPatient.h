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

namespace communication
{

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
    typedef std::pair<float, std::string> value_type;


	value_type bodyMass;
    value_type height;
    value_type interAsisDistance;
    value_type leftLegLength;
    value_type rightLegLenght;
    value_type leftKneeWidth;
    value_type rightKneeWidth;
    value_type leftAnkleWidth;
    value_type rightAnkleWidth;
    value_type leftCircuitThigh;
    value_type rightCircuitThight;
    value_type leftCircuitShank;
    value_type rightCircuitShank;
    value_type leftShoulderOffset;
    value_type rightShoulderOffset;
    value_type leftElbowWidth;
    value_type rightElbowWidth;
    value_type leftWristWidth;
    value_type rightWristWidth;
    value_type leftWristThickness;
    value_type rightWristThickness;
    value_type leftHandWidth;
    value_type rightHandWidth;
    value_type leftHandThickness;
    value_type rightHandThickness;
};

class IPatient
{

public:

	enum Gender {
		Male,
		Female,
		NotSpecifiedGender
	};

public:
    virtual ~IPatient() {};

    virtual PluginSubject::SubjectID getID() const = 0;

    virtual const std::string & getName() const = 0;
    virtual const std::string & getSurname() const = 0;
    virtual const std::string & getBirthday() const = 0;
    virtual Gender getGender() const = 0;
    virtual const core::shared_ptr<const QPixmap> & getPhoto() const = 0;

    virtual unsigned int disordersCount() const = 0;
    virtual const Disorder & disorder(unsigned int idx) const = 0;
};

typedef core::shared_ptr<IPatient> PatientPtr;
typedef core::shared_ptr<const IPatient> PatientConstPtr;

}

CORE_DEFINE_WRAPPER(communication::IPatient, utils::PtrPolicyBoost, utils::ClonePolicyNotImplemented);
CORE_DEFINE_WRAPPER(communication::AntropometricData, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);

#endif HEADER_GUARD___IPATIENT_H__