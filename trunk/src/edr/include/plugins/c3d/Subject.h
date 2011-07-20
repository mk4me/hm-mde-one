/********************************************************************
	created:	2011/07/05
	created:	5:7:2011   7:45
	filename: 	Subject.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_PLUGINC3D__SUBJECT_H__
#define HEADER_GUARD_PLUGINC3D__SUBJECT_H__

#include <string>
#include <boost/smart_ptr.hpp>
#include <utils/ClonePolicies.h>
#include <utils/PtrPolicyBoost.h>
#include <core/ObjectWrapper.h>

struct Subject
{
	std::string date;
	std::string description1;
	std::string notes;
	std::string sex;
	std::string birthdate;
	std::string isIll;
	std::string isDiabetes;
	std::string ilnesses;
	std::string description2;
	std::string name;
	std::string surname;
	std::string status;
};
typedef boost::shared_ptr<Subject> SubjectPtr;
typedef boost::shared_ptr<const Subject> SubjectConstPtr;
CORE_DEFINE_WRAPPER(Subject, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);

#endif
