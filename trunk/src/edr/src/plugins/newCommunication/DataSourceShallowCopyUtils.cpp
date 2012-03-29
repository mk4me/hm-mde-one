#include "CommunicationPCH.h"
#include "DataSourceShallowCopyUtils.h"

#include <webserviceslib/ShallowCopyParser.h>
#include <webserviceslib/MetadataParser.h>

using namespace communication;

void DataSourceShallowCopyUtils::createShallowCopy(ShallowCopy & shallowCopy, const core::Filesystem::Path & motionShallowCopy,
    const core::Filesystem::Path & motionMetadata, const core::Filesystem::Path & medicalShallowCopy,
    const core::Filesystem::Path & medicalMetadata)
{
	using namespace webservices;

	ShallowCopy tmpShallowCopy;

	//inicjalizujemy
	tmpShallowCopy.motionShallowCopy.reset(new MotionShallowCopy::ShallowCopy());
	tmpShallowCopy.medicalShallowCopy.reset(new MedicalShallowCopy::ShallowCopy());

	//parsujemy
    MotionShallowCopyParser::parseFile(motionShallowCopy.string(), *(tmpShallowCopy.motionShallowCopy));
    MedicalShallowCopyParser::parseFile(medicalShallowCopy.string(), *(tmpShallowCopy.medicalShallowCopy));
    MotionMetadataParser::parseFile(motionMetadata.string(), tmpShallowCopy.motionMetaData);
    MedicalMetadataParser::parseFile(medicalMetadata.string(), tmpShallowCopy.medicalMetaData);

	//mamy teoretycznie wszystko - teraz próbuje po³¹czyæ obie bazy
	auto subjectsITEnd = tmpShallowCopy.motionShallowCopy->performers.end();
	auto patientsITEnd = tmpShallowCopy.medicalShallowCopy->patients.end();
	for(auto patientIT = tmpShallowCopy.medicalShallowCopy->patients.begin(); patientIT != patientsITEnd; ++patientIT){
		auto subjectIT = tmpShallowCopy.motionShallowCopy->performers.find(patientIT->second->motionPerformerID);
		if(subjectIT != subjectsITEnd){
			patientIT->second->performer = subjectIT->second;
			subjectIT->second->patient = patientIT->second;
		}
	}

	// jeœli wszystko siê powiod³o podmieniamy wartosci
	std::swap(shallowCopy, tmpShallowCopy);
}

bool DataSourceShallowCopyUtils::checkShallowCopyIntegrity(const ShallowCopy & shallowCopy)
{
    //TODO
    //sprawdzaæ integralnoœæ danych w p³ytkiej kopii bazy danych
    return true;
}

bool DataSourceShallowCopyUtils::shallowCopyRequiresRefresh(const ShallowCopy & shallowCopy, webservices::DateTime & currentVersion)
{
	return shallowCopy.motionShallowCopy->timestamp < currentVersion ? true : false;
}