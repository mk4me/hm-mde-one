#include "CommunicationPCH.h"
#include "DataSourceFilterManager.h"
#include <utils/Debug.h>

using namespace communication;
using namespace webservices;

DataSourceFilterManager::DataSourceFilterManager() : currentFilter_(-1)
{

}

DataSourceFilterManager::~DataSourceFilterManager()
{
    
}

int DataSourceFilterManager::registerFilter(DataSourceFilter * filter)
{
    auto it = filters.begin();

    while(it != filters.end()){
        if((*it).get() == filter){
            break;
        }
        ++it;
    }
    
    if(it != filters.end()){
        throw std::runtime_error("Filter already registered");
    }

    int ret = filters.size();
    filters.push_back(DataFilterPtr(filter));

    return ret;
}

void DataSourceFilterManager::setCurrentFilter(int idx)
{
    UTILS_ASSERT(idx == -1 || (idx >= 0 && idx < filters.size()));

    currentFilter_ = idx;
}

int DataSourceFilterManager::size() const
{
    return filters.size();
}

bool DataSourceFilterManager::empty() const
{
    return filters.empty();
}

int DataSourceFilterManager::currentFilterIndex() const
{
    return currentFilter_;
}

DataFilterPtr DataSourceFilterManager::currentFilter() const
{
	if(currentFilter_ == -1){
		return DataFilterPtr();
	}

	return dataFilter(currentFilter_);
}

DataFilterPtr DataSourceFilterManager::dataFilter(int idx) const
{
    return filters[idx];
}

void DataSourceFilterManager::filterShallowCopy(const communication::ShallowCopy & inShallow, communication::ShallowCopy & outShallow, const communication::DataFilterPtr & filter)
{
	if(filter == nullptr){
		outShallow = inShallow;
		return;
	}

	//schorzenia
	for(auto disorderIT = inShallow.medicalShallowCopy->disorders.begin(); disorderIT != inShallow.medicalShallowCopy->disorders.end(); ++disorderIT){
		
		if(filter->filterDisorder(disorderIT->second) == true){			
			
			auto disorder = new MedicalShallowCopy::Disorder();

			disorder->name = disorderIT->second->name;
			disorder->disorderID = disorderIT->second->disorderID;

			outShallow.medicalShallowCopy->disorders.insert(MedicalShallowCopy::Disorders::value_type(disorder->disorderID, disorder));

			auto & patients = outShallow.medicalShallowCopy->patientsByDisorder[disorderIT->first];

			//pacjenci
			for(auto patientIT = inShallow.medicalShallowCopy->patients.begin(); patientIT != inShallow.medicalShallowCopy->patients.end(); ++patientIT){

				if(filter->filterPatient(patientIT->second) == true){			

					auto patient = new MedicalShallowCopy::Patient();

					patient->patientID = patientIT->second->patientID;
					patient->name = patientIT->second->name;
					patient->surname = patientIT->second->surname;
					patient->gender = patientIT->second->gender;
					patient->birthDate = patientIT->second->birthDate;

					//póŸniej trzeba poprawiæ o dostêpny zakres i nowe instancje patient->disorders

					//zapisuje pacjenta
					outShallow.medicalShallowCopy->patients.insert(MedicalShallowCopy::Patients::value_type(patient->patientID, patient));
					//aktualizuje grupowanie pacjentów po schorzeniu
					patients.insert(patient);

					if(filter->filterSubject(patientIT->second->performer) == true){
						auto performer = new MotionShallowCopy::Performer();
						//aktualizuje pacjenta
						patient->performer = performer;
						patient->motionPerformerID = patientIT->second->motionPerformerID;

						performer->attrs = patientIT->second->performer->attrs;
						performer->patient = patient;
						performer->performerID = patientIT->second->performer->performerID;

						//zapisuje subjecta
						outShallow.motionShallowCopy->performers.insert(MotionShallowCopy::Performers::value_type(performer->performerID, performer));

						//konfiguracje i sesje
						for(auto perfConfIT = patientIT->second->performer->performerConfs.begin(); perfConfIT != patientIT->second->performer->performerConfs.end(); ++perfConfIT){
							if(filter->filterSession(perfConfIT->second->session) == true){
								
								auto perfConf = new MotionShallowCopy::PerformerConf();

								perfConf->attrs = perfConfIT->second->attrs;
								perfConf->performer = performer;
								perfConf->performerConfID = perfConfIT->second->performerConfID;

								auto session = new MotionShallowCopy::Session();
								perfConf->session = session;

								outShallow.motionShallowCopy->performerConfs.insert(MotionShallowCopy::PerformerConfs::value_type(perfConf->performerConfID, perfConf));

								performer->performerConfs.insert(MotionShallowCopy::PerformerConfs::value_type(perfConf->performerConfID, perfConf));

								session->attrs = perfConfIT->second->session->attrs;
								session->labID = perfConfIT->second->session->labID;
								session->motionKind = perfConfIT->second->session->motionKind;
								session->performerConf = perfConf;
								session->sessionDate = perfConfIT->second->session->sessionDate;
								session->sessionDescription = perfConfIT->second->session->sessionDescription;
								session->sessionID = perfConfIT->second->session->sessionID;
								session->sessionName = perfConfIT->second->session->sessionName;
								session->tags = perfConfIT->second->session->tags;
								session->userID = perfConfIT->second->session->userID;

								//filtrujemy pliki sesji
								for(auto fileIT = perfConfIT->second->session->files.begin(); fileIT != perfConfIT->second->session->files.end(); ++fileIT){
									if(filter->filterFile(fileIT->second) == true){
										
										auto file = new MotionShallowCopy::File();
										
										file->fileDescription = fileIT->second->fileDescription;
										file->fileID = fileIT->second->fileID;
										file->fileName = fileIT->second->fileName;
										file->session = session;
										file->subdirPath = fileIT->second->subdirPath;
										file->trial = nullptr;

										//zapisz plik
										outShallow.motionShallowCopy->files.insert(MotionShallowCopy::Files::value_type(file->fileID, file));
										//aktualizuj sesjê
										session->files.insert(MotionShallowCopy::Files::value_type(file->fileID, file));
									}
								}

								//filtrujemy triale
								for(auto motionIT = perfConfIT->second->session->trials.begin(); motionIT != perfConfIT->second->session->trials.end(); ++motionIT){
									if(filter->filterMotion(motionIT->second) == true){

										auto motion = new MotionShallowCopy::Trial();

										motion->attrs = motionIT->second->attrs;
										motion->session = session;
										motion->trialDescription = motionIT->second->trialDescription;
										motion->trialID = motionIT->second->trialID;
										motion->trialName = motionIT->second->trialName;

										//zapisz plik
										outShallow.motionShallowCopy->trials.insert(MotionShallowCopy::Trials::value_type(motion->trialID, motion));
										//aktualizuj sesjê
										session->trials.insert(MotionShallowCopy::Trials::value_type(motion->trialID, motion));

										//filtrujemy pliki motiona
										for(auto fileIT = perfConfIT->second->session->files.begin(); fileIT != perfConfIT->second->session->files.end(); ++fileIT){
											if(filter->filterFile(fileIT->second) == true){

												auto file = new MotionShallowCopy::File();

												file->fileDescription = fileIT->second->fileDescription;
												file->fileID = fileIT->second->fileID;
												file->fileName = fileIT->second->fileName;
												file->session = nullptr;
												file->subdirPath = fileIT->second->subdirPath;
												file->trial = motion;

												//zapisz plik
												outShallow.motionShallowCopy->files.insert(MotionShallowCopy::Files::value_type(file->fileID, file));
												//aktualizuj sesjê
												motion->files.insert(MotionShallowCopy::Files::value_type(file->fileID, file));
											}
										}
									}
								}
							}
						}

					}else{
						patient->performer = nullptr;
						patient->motionPerformerID = -1;
					}					
				}
			}
		}
	}

	//uzupe³niamy choroby pacjenta
	for(auto patientIT = outShallow.medicalShallowCopy->patients.begin(); patientIT != outShallow.medicalShallowCopy->patients.end(); ++patientIT){
		
		auto patient = inShallow.medicalShallowCopy->patients.find(patientIT->first);

		for(auto disorderIT = patient->second->disorders.begin(); disorderIT != patient->second->disorders.end(); ++disorderIT){

			auto disorder = outShallow.medicalShallowCopy->disorders.find(disorderIT->first);

			if(disorder != outShallow.medicalShallowCopy->disorders.end()){
				
				MedicalShallowCopy::DisorderOccurence disOcc;

				disOcc.disorder = disorder->second;
				disOcc.comments = disorderIT->second.comments;
				disOcc.diagnosisDate = disorderIT->second.diagnosisDate;
				disOcc.focus = disorderIT->second.focus;
				
				patientIT->second->disorders.insert(MedicalShallowCopy::PatientDisorders::value_type(disorder->first, disOcc));
			}
		}
	}
}