#include "BasicQueriesWS.h"
#include <hmdbserviceslib/BasicQueriesWS.h>
#include <hmdbserviceslib/DateTimeUtils.h>

using namespace hmdbCommunication;

GeneralBasicQueriesWS::GeneralBasicQueriesWS(const WSDLServiceCreator creator)
	: creator(creator)
{
}

GeneralBasicQueriesWS::~GeneralBasicQueriesWS()
{
}

const hmdbServices::DateTime GeneralBasicQueriesWS::dataModificationTime() const
{
	return hmdbServices::GeneralBasicQueriesWS(creator()).dataModificationTime();
}

const hmdbServices::DateTime GeneralBasicQueriesWS::metadataModificationTime() const
{
	return hmdbServices::GeneralBasicQueriesWS(creator()).metadataModificationTime();
}

MotionBasicQueriesWS::MotionBasicQueriesWS(const WSDLServiceCreator creator)
	: creator(creator)
{
}

MotionBasicQueriesWS::~MotionBasicQueriesWS()
{
}

const std::string MotionBasicQueriesWS::getPerformerByIdXML(const int id) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).getPerformerByIdXML(id);
}

const std::string MotionBasicQueriesWS::getSessionByIdXML(const int id) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).getSessionByIdXML(id);
}

const std::string MotionBasicQueriesWS::getSessionLabel(const int id) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).getSessionLabel(id);
}

const std::string MotionBasicQueriesWS::getTrialByIdXML(const int id) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).getTrialByIdXML(id);
}

const std::string MotionBasicQueriesWS::getMeasurementConfigurationByIdXML(const int id) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).getMeasurementConfigurationByIdXML(id);
}

const std::string MotionBasicQueriesWS::getPerformerConfigurationByIdXML(const int id) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).getPerformerConfigurationByIdXML(id);
}

const std::string MotionBasicQueriesWS::listPerformersXML() const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listPerformersXML();
}

const std::string MotionBasicQueriesWS::listPerformersWithAttributesXML() const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listPerformersWithAttributesXML();
}

const std::string MotionBasicQueriesWS::listLabPerformersWithAttributesXML(const int labID) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listLabPerformersWithAttributesXML(labID);
}

const std::string MotionBasicQueriesWS::listSessionPerformersWithAttributesXML(const int sessionID) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listSessionPerformersWithAttributesXML(sessionID);
}

const std::string MotionBasicQueriesWS::listPerformerSessionsXML(const int performerID) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listPerformerSessionsXML(performerID);
}

const std::string MotionBasicQueriesWS::listPerformerSessionsWithAttributesXML(const int performerID) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listPerformerSessionsWithAttributesXML(performerID);
}

const std::string MotionBasicQueriesWS::listLabSessionsWithAttributesXML(const int labID) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listLabSessionsWithAttributesXML(labID);
}

const std::string MotionBasicQueriesWS::listGroupSessionsWithAttributesXML(const int sessionGroupID) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listGroupSessionsWithAttributesXML(sessionGroupID);
}

const std::string MotionBasicQueriesWS::listMeasurementConfigSessionsWithAttributesXML(const int measurementConfID) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listMeasurementConfigSessionsWithAttributesXML(measurementConfID);
}

const std::string MotionBasicQueriesWS::listSessionSessionGroups(const int sessionID) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listSessionSessionGroups(sessionID);
}

const std::string MotionBasicQueriesWS::listSessionTrialsXML(const int sessionID) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listSessionTrialsXML(sessionID);
}

const std::string MotionBasicQueriesWS::listSessionTrialsWithAttributesXML(const int sessionID) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listSessionTrialsWithAttributesXML(sessionID);
}

const std::string MotionBasicQueriesWS::listMeasurementConfigurationsWithAttributesXML() const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listMeasurementConfigurationsWithAttributesXML();
}

const std::string MotionBasicQueriesWS::listSessionPerformerConfsWithAttributesXML(const int sessionID) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listSessionPerformerConfsWithAttributesXML(sessionID);
}

const std::string MotionBasicQueriesWS::listFilesXML(const int subjectID, const std::string & subjectType) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listFilesXML(subjectID, subjectType);
}

const std::string MotionBasicQueriesWS::listFilesWithAttributesXML(const int subjectID, const std::string & subjectType) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listFilesWithAttributesXML(subjectID, subjectType);
}

const std::string MotionBasicQueriesWS::listFileAttributeDataXML(const int subjectID, const std::string & subjectType) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listFileAttributeDataXML(subjectID, subjectType);
}

const std::string MotionBasicQueriesWS::listFileAttributeDataWithAttributesXML(const int subjectID, const std::string & subjectType) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listFileAttributeDataWithAttributesXML(subjectID, subjectType);
}

const std::string MotionBasicQueriesWS::listAttributesDefined(const std::string & attributeGroupName,
	const std::string & entityKind) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listAttributesDefined(attributeGroupName, entityKind);
}

const std::string MotionBasicQueriesWS::listAttributeGroupsDefined(const std::string & entityKind) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listAttributeGroupsDefined(entityKind);
}

const std::string MotionBasicQueriesWS::listSessionGroupsDefined() const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listSessionGroupsDefined();
}

const std::string MotionBasicQueriesWS::listMotionKindsDefined() const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listMotionKindsDefined();
}

const std::string MotionBasicQueriesWS::listEnumValues(const std::string & attributeName,
	const std::string & entityKind) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listEnumValues(attributeName, entityKind);
}

const std::string MotionBasicQueriesWS::listMyAnnotationsXML() const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listMyAnnotationsXML();
}

const std::string MotionBasicQueriesWS::listAnnotationsXML() const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listAnnotationsXML();
}

const std::string MotionBasicQueriesWS::listMyAwaitingAnnotationsXML() const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listMyAwaitingAnnotationsXML();
}

const std::string MotionBasicQueriesWS::listMyReviewedAnnotationsXML() const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listMyReviewedAnnotationsXML();
}

const hmdbServices::DateTime MotionBasicQueriesWS::dataModificationTime() const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).dataModificationTime();
}

const hmdbServices::DateTime MotionBasicQueriesWS::metadataModificationTime() const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).metadataModificationTime();
}

MedicalBasicQueriesWS::MedicalBasicQueriesWS(const WSDLServiceCreator creator)
	: creator(creator)
{
}

MedicalBasicQueriesWS::~MedicalBasicQueriesWS()
{
}

const hmdbServices::DateTime MedicalBasicQueriesWS::dataModificationTime() const
{
	return hmdbServices::MedicalBasicQueriesWS(creator()).dataModificationTime();
}

const hmdbServices::DateTime MedicalBasicQueriesWS::metadataModificationTime() const
{
	return hmdbServices::MedicalBasicQueriesWS(creator()).metadataModificationTime();
}