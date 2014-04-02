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

void DataSourceFilterManager::clearFilters()
{
	std::vector<communication::DataFilterPtr>().swap(filters);
	currentFilter_ = -1;
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
    UTILS_ASSERT(idx == -1 || (idx >= 0 && idx < static_cast<int>(filters.size())));

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

	std::set<int> allVerifiedPatients;
	std::set<int> allVerifiedPerformers;

	//schorzenia
	auto disordersITEnd = inShallow.medicalShallowCopy->disorders.end();	
	for(auto disorderIT = inShallow.medicalShallowCopy->disorders.begin(); disorderIT != disordersITEnd; ++disorderIT){
		
		auto patientsToVerifyIT = inShallow.medicalShallowCopy->patientsByDisorder.find(disorderIT->first);		

		if(patientsToVerifyIT != inShallow.medicalShallowCopy->patientsByDisorder.end()){
			for(auto it = patientsToVerifyIT->second.begin(); it
				!= patientsToVerifyIT->second.end(); ++it){

				allVerifiedPatients.insert((*it)->patientID);
				if((*it)->performer != nullptr){
					allVerifiedPerformers.insert((*it)->performer->performerID);
				}
			}
		}

		if(filter->filterDisorder(disorderIT->second) == true){

			if(patientsToVerifyIT != inShallow.medicalShallowCopy->patientsByDisorder.end()
				&& patientsToVerifyIT->second.empty() == false){

				std::set<webservices::MedicalShallowCopy::Patient*> verifiedPatients;

				//pacjenci
				for(auto patientIT = patientsToVerifyIT->second.begin(); patientIT != patientsToVerifyIT->second.end(); ++patientIT){

					if(filter->filterPatient(*patientIT) == true){			

						if(filter->filterSubject((*patientIT)->performer) == true){

							std::set<webservices::MotionShallowCopy::Session*> verifiedSessions;

							//konfiguracje i sesje
							auto performerConfsITEnd = (*patientIT)->performer->performerConfs.end();
							for(auto perfConfIT = (*patientIT)->performer->performerConfs.begin(); perfConfIT != performerConfsITEnd; ++perfConfIT){
								
								if(filter->filterSession(perfConfIT->second->session) == true){

									std::set<webservices::MotionShallowCopy::File*> verifiedSessionFiles;
									std::set<webservices::MotionShallowCopy::Trial*> verifiedMotions;

									//filtrujemy pliki sesji
									auto filesITEnd = perfConfIT->second->session->files.end();
									for(auto fileIT = perfConfIT->second->session->files.begin(); fileIT != filesITEnd; ++fileIT){
										if(filter->filterFile(fileIT->second) == true){
										
											auto file = new MotionShallowCopy::File();

											file->fileDescription = fileIT->second->fileDescription;
											file->fileID          = fileIT->second->fileID;
											file->fileName        = fileIT->second->fileName;
											//to potem uzupelniamy
											file->session         = nullptr;
											file->subdirPath      = fileIT->second->subdirPath;
											file->trial           = nullptr;

											verifiedSessionFiles.insert(file);
										}
									}

									//filtrujemy triale
									auto motionsITEnd = perfConfIT->second->session->trials.end();
									for(auto motionIT = perfConfIT->second->session->trials.begin(); motionIT != motionsITEnd; ++motionIT){
										if(filter->filterMotion(motionIT->second) == true){

											std::set<webservices::MotionShallowCopy::File*> verifiedMotionFiles;

											//filtrujemy pliki motiona
											auto filesITEnd = perfConfIT->second->session->files.end();
											for(auto fileIT = perfConfIT->second->session->files.begin(); fileIT != filesITEnd; ++fileIT){
												if(filter->filterFile(fileIT->second) == true){

													auto file = new MotionShallowCopy::File();

													file->fileDescription = fileIT->second->fileDescription;
													file->fileID          = fileIT->second->fileID;
													file->fileName        = fileIT->second->fileName;
													file->session         = nullptr;
													file->subdirPath      = fileIT->second->subdirPath;
													file->trial           = nullptr;

													verifiedMotionFiles.insert(file);
												}
											}

											if(verifiedMotionFiles.empty() == false){
												//motion
												auto motion = new MotionShallowCopy::Trial();

												motion->attrs            = motionIT->second->attrs;
												//to bedziemy później aktualizować
												motion->session          = nullptr;
												motion->trialDescription = motionIT->second->trialDescription;
												motion->trialID          = motionIT->second->trialID;
												motion->trialName        = motionIT->second->trialName;

												//teraz aktualizujemy pliki
												for(auto it = verifiedMotionFiles.begin(); it != verifiedMotionFiles.end(); ++it){
													(*it)->trial = motion;
													auto val     = MotionShallowCopy::Files::value_type((*it)->fileID, *it);
													outShallow.motionShallowCopy->files.insert(val);
													motion->files.insert(val);
												}
												
												verifiedMotions.insert(motion);
											}
										}
									}

									if(verifiedMotions.empty() == false || verifiedSessionFiles.empty() == false){

										auto perfConf             = new MotionShallowCopy::PerformerConf();
										perfConf->attrs           = perfConfIT->second->attrs;
										//to ustawiamy pozniej
										perfConf->performer       = nullptr;
										perfConf->performerConfID = perfConfIT->second->performerConfID;
										auto session              = new MotionShallowCopy::Session();
										perfConf->session         = session;

										session->attrs              = perfConfIT->second->session->attrs;
										session->labID              = perfConfIT->second->session->labID;
										session->motionKind         = perfConfIT->second->session->motionKind;
										session->performerConf      = perfConf;
										session->sessionDate        = perfConfIT->second->session->sessionDate;
										session->sessionDescription = perfConfIT->second->session->sessionDescription;
										session->sessionID          = perfConfIT->second->session->sessionID;
										session->sessionName        = perfConfIT->second->session->sessionName;
										session->tags               = perfConfIT->second->session->tags;
										session->userID             = perfConfIT->second->session->userID;

										for(auto it = verifiedSessionFiles.begin(); it != verifiedSessionFiles.end(); ++it){
											(*it)->session = session;
											auto val = MotionShallowCopy::Files::value_type((*it)->fileID, *it);
											//zapisz plik
											outShallow.motionShallowCopy->files.insert(val);
											//aktualizuj sesję
											session->files.insert(val);											
										}
										
										for(auto it = verifiedMotions.begin(); it != verifiedMotions.end(); ++it){
											(*it)->session = session;
											auto val = MotionShallowCopy::Trials::value_type((*it)->trialID, *it);
											//zapisz motion
											outShallow.motionShallowCopy->trials.insert(val);
											//aktualizuj sesję - to później
											session->trials.insert(val);											
										}

										verifiedSessions.insert(session);
									}
								}
							}

							if(verifiedSessions.empty() == false){

								auto patient = new MedicalShallowCopy::Patient();

								patient->patientID = (*patientIT)->patientID;
								patient->name      = (*patientIT)->name;
								patient->surname   = (*patientIT)->surname;
								patient->gender    = (*patientIT)->gender;
								patient->birthDate = (*patientIT)->birthDate;

								//później trzeba poprawić o dostępny zakres i nowe instancje patient->disorders

								auto performer             = new MotionShallowCopy::Performer();
								performer->attrs           = (*patientIT)->performer->attrs;
								performer->patient         = patient;
								performer->performerID     = (*patientIT)->performer->performerID;


								//aktualizuje pacjenta
								patient->performer         = performer;
								patient->motionPerformerID = (*patientIT)->motionPerformerID;

								for(auto it = verifiedSessions.begin(); it != verifiedSessions.end(); ++it){
									(*it)->performerConf->performer = performer;
									auto val = MotionShallowCopy::PerformerConfs::value_type((*it)->performerConf->performerConfID, (*it)->performerConf);
									outShallow.motionShallowCopy->performerConfs.insert(val);
									performer->performerConfs.insert(val);
									outShallow.motionShallowCopy->sessions.insert(MotionShallowCopy::Sessions::value_type((*it)->sessionID, *it));
								}

								//zapisuje subjecta
								outShallow.motionShallowCopy->performers.insert(MotionShallowCopy::Performers::value_type(performer->performerID, performer));
								//zapisuje pacjenta
								outShallow.medicalShallowCopy->patients.insert(MedicalShallowCopy::Patients::value_type(patient->patientID, patient));
								//aktualizuje grupowanie pacjentów po schorzeniu
								verifiedPatients.insert(patient);
							}
						}
					}
				}

				if(verifiedPatients.empty() == false){

					auto disorder = new MedicalShallowCopy::Disorder();

					disorder->name = disorderIT->second->name;
					disorder->disorderID = disorderIT->second->disorderID;

					outShallow.medicalShallowCopy->disorders.insert(MedicalShallowCopy::Disorders::value_type(disorder->disorderID, disorder));
					auto & patients = outShallow.medicalShallowCopy->patientsByDisorder[disorderIT->first];

					for(auto it = verifiedPatients.begin(); it != verifiedPatients.end(); ++it){
						patients.insert(*it);
						outShallow.medicalShallowCopy->patientsByGender[(*it)->gender].insert(*it);
					}
				}
			}
		}
	}	

	//uzupełniamy choroby pacjenta
	auto outPatientsITEnd = outShallow.medicalShallowCopy->patients.end();
	for(auto patientIT = outShallow.medicalShallowCopy->patients.begin(); patientIT != outPatientsITEnd; ++patientIT){

		auto patient = inShallow.medicalShallowCopy->patients.find(patientIT->first);
		auto disordersITEnd = patient->second->disorders.end();
		for(auto disorderIT = patient->second->disorders.begin(); disorderIT != disordersITEnd; ++disorderIT){

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

	//pacjenci
	for(auto patientIT = inShallow.medicalShallowCopy->patients.begin();
		patientIT != inShallow.medicalShallowCopy->patients.end(); ++patientIT){

		if(allVerifiedPatients.find(patientIT->first) != allVerifiedPatients.end()){
			continue;
		}

		if(patientIT->second->performer != nullptr){
			allVerifiedPerformers.insert(patientIT->second->performer->performerID);
		}

		if(filter->filterPatient(patientIT->second) == true){			

			if(filter->filterSubject(patientIT->second->performer) == true){

				std::set<webservices::MotionShallowCopy::Session*> verifiedSessions;

				//konfiguracje i sesje
				auto performerConfsITEnd = patientIT->second->performer->performerConfs.end();
				for(auto perfConfIT = patientIT->second->performer->performerConfs.begin(); perfConfIT != performerConfsITEnd; ++perfConfIT){

					if(filter->filterSession(perfConfIT->second->session) == true){

						std::set<webservices::MotionShallowCopy::File*> verifiedSessionFiles;
						std::set<webservices::MotionShallowCopy::Trial*> verifiedMotions;

						//filtrujemy pliki sesji
						auto filesITEnd = perfConfIT->second->session->files.end();
						for(auto fileIT = perfConfIT->second->session->files.begin(); fileIT != filesITEnd; ++fileIT){
							if(filter->filterFile(fileIT->second) == true){

								auto file = new MotionShallowCopy::File();

								file->fileDescription = fileIT->second->fileDescription;
								file->fileID          = fileIT->second->fileID;
								file->fileName        = fileIT->second->fileName;
								//to potem uzupelniamy
								file->session         = nullptr;
								file->subdirPath      = fileIT->second->subdirPath;
								file->trial           = nullptr;

								verifiedSessionFiles.insert(file);
							}
						}

						//filtrujemy triale
						auto motionsITEnd = perfConfIT->second->session->trials.end();
						for(auto motionIT = perfConfIT->second->session->trials.begin(); motionIT != motionsITEnd; ++motionIT){
							if(filter->filterMotion(motionIT->second) == true){

								std::set<webservices::MotionShallowCopy::File*> verifiedMotionFiles;

								//filtrujemy pliki motiona
								auto filesITEnd = perfConfIT->second->session->files.end();
								for(auto fileIT = perfConfIT->second->session->files.begin(); fileIT != filesITEnd; ++fileIT){
									if(filter->filterFile(fileIT->second) == true){

										auto file = new MotionShallowCopy::File();

										file->fileDescription = fileIT->second->fileDescription;
										file->fileID          = fileIT->second->fileID;
										file->fileName        = fileIT->second->fileName;
										file->session         = nullptr;
										file->subdirPath      = fileIT->second->subdirPath;
										file->trial           = nullptr;

										verifiedMotionFiles.insert(file);
									}
								}

								if(verifiedMotionFiles.empty() == false){
									//motion
									auto motion = new MotionShallowCopy::Trial();

									motion->attrs            = motionIT->second->attrs;
									//to bedziemy później aktualizować
									motion->session          = nullptr;
									motion->trialDescription = motionIT->second->trialDescription;
									motion->trialID          = motionIT->second->trialID;
									motion->trialName        = motionIT->second->trialName;

									//teraz aktualizujemy pliki
									for(auto it = verifiedMotionFiles.begin(); it != verifiedMotionFiles.end(); ++it){
										(*it)->trial = motion;
										auto val     = MotionShallowCopy::Files::value_type((*it)->fileID, *it);
										outShallow.motionShallowCopy->files.insert(val);
										motion->files.insert(val);
									}

									verifiedMotions.insert(motion);
								}
							}
						}

						if(verifiedMotions.empty() == false || verifiedSessionFiles.empty() == false){

							auto perfConf             = new MotionShallowCopy::PerformerConf();
							perfConf->attrs           = perfConfIT->second->attrs;
							//to ustawiamy pozniej
							perfConf->performer       = nullptr;
							perfConf->performerConfID = perfConfIT->second->performerConfID;
							auto session              = new MotionShallowCopy::Session();
							perfConf->session         = session;

							session->attrs              = perfConfIT->second->session->attrs;
							session->labID              = perfConfIT->second->session->labID;
							session->motionKind         = perfConfIT->second->session->motionKind;
							session->performerConf      = perfConf;
							session->sessionDate        = perfConfIT->second->session->sessionDate;
							session->sessionDescription = perfConfIT->second->session->sessionDescription;
							session->sessionID          = perfConfIT->second->session->sessionID;
							session->sessionName        = perfConfIT->second->session->sessionName;
							session->tags               = perfConfIT->second->session->tags;
							session->userID             = perfConfIT->second->session->userID;

							for(auto it = verifiedSessionFiles.begin(); it != verifiedSessionFiles.end(); ++it){
								(*it)->session = session;
								auto val = MotionShallowCopy::Files::value_type((*it)->fileID, *it);
								//zapisz plik
								outShallow.motionShallowCopy->files.insert(val);
								//aktualizuj sesję
								session->files.insert(val);											
							}

							for(auto it = verifiedMotions.begin(); it != verifiedMotions.end(); ++it){
								(*it)->session = session;
								auto val = MotionShallowCopy::Trials::value_type((*it)->trialID, *it);
								//zapisz motion
								outShallow.motionShallowCopy->trials.insert(val);
								//aktualizuj sesję - to później
								session->trials.insert(val);											
							}

							verifiedSessions.insert(session);
						}
					}
				}

				if(verifiedSessions.empty() == false){

					auto patient = new MedicalShallowCopy::Patient();

					patient->patientID = patientIT->second->patientID;
					patient->name      = patientIT->second->name;
					patient->surname   = patientIT->second->surname;
					patient->gender    = patientIT->second->gender;
					patient->birthDate = patientIT->second->birthDate;

					//później trzeba poprawić o dostępny zakres i nowe instancje patient->disorders

					auto performer             = new MotionShallowCopy::Performer();
					performer->attrs           = patientIT->second->performer->attrs;
					performer->patient         = patient;
					performer->performerID     = patientIT->second->performer->performerID;


					//aktualizuje pacjenta
					patient->performer         = performer;
					patient->motionPerformerID = patientIT->second->motionPerformerID;

					for(auto it = verifiedSessions.begin(); it != verifiedSessions.end(); ++it){
						(*it)->performerConf->performer = performer;
						auto val = MotionShallowCopy::PerformerConfs::value_type((*it)->performerConf->performerConfID, (*it)->performerConf);
						outShallow.motionShallowCopy->performerConfs.insert(val);
						performer->performerConfs.insert(val);
						outShallow.motionShallowCopy->sessions.insert(MotionShallowCopy::Sessions::value_type((*it)->sessionID, *it));
					}

					//zapisuje subjecta
					outShallow.motionShallowCopy->performers.insert(MotionShallowCopy::Performers::value_type(performer->performerID, performer));
					//zapisuje pacjenta
					outShallow.medicalShallowCopy->patients.insert(MedicalShallowCopy::Patients::value_type(patient->patientID, patient));
				}
			}
		}
	}

	for(auto subjectIT = inShallow.motionShallowCopy->performers.begin();
		subjectIT != inShallow.motionShallowCopy->performers.end(); ++subjectIT){

		if(allVerifiedPerformers.find(subjectIT->first) != allVerifiedPerformers.end()){
			continue;
		}

		if(filter->filterSubject(subjectIT->second) == true){

			std::set<webservices::MotionShallowCopy::Session*> verifiedSessions;

			//konfiguracje i sesje
			auto performerConfsITEnd = subjectIT->second->performerConfs.end();
			for(auto perfConfIT = subjectIT->second->performerConfs.begin(); perfConfIT != performerConfsITEnd; ++perfConfIT){

				if(filter->filterSession(perfConfIT->second->session) == true){

					std::set<webservices::MotionShallowCopy::File*> verifiedSessionFiles;
					std::set<webservices::MotionShallowCopy::Trial*> verifiedMotions;

					//filtrujemy pliki sesji
					auto filesITEnd = perfConfIT->second->session->files.end();
					for(auto fileIT = perfConfIT->second->session->files.begin(); fileIT != filesITEnd; ++fileIT){
						if(filter->filterFile(fileIT->second) == true){

							auto file = new MotionShallowCopy::File();

							file->fileDescription = fileIT->second->fileDescription;
							file->fileID          = fileIT->second->fileID;
							file->fileName        = fileIT->second->fileName;
							//to potem uzupelniamy
							file->session         = nullptr;
							file->subdirPath      = fileIT->second->subdirPath;
							file->trial           = nullptr;

							verifiedSessionFiles.insert(file);
						}
					}

					//filtrujemy triale
					auto motionsITEnd = perfConfIT->second->session->trials.end();
					for(auto motionIT = perfConfIT->second->session->trials.begin(); motionIT != motionsITEnd; ++motionIT){
						if(filter->filterMotion(motionIT->second) == true){

							std::set<webservices::MotionShallowCopy::File*> verifiedMotionFiles;

							//filtrujemy pliki motiona
							auto filesITEnd = perfConfIT->second->session->files.end();
							for(auto fileIT = perfConfIT->second->session->files.begin(); fileIT != filesITEnd; ++fileIT){
								if(filter->filterFile(fileIT->second) == true){

									auto file = new MotionShallowCopy::File();

									file->fileDescription = fileIT->second->fileDescription;
									file->fileID          = fileIT->second->fileID;
									file->fileName        = fileIT->second->fileName;
									file->session         = nullptr;
									file->subdirPath      = fileIT->second->subdirPath;
									file->trial           = nullptr;

									verifiedMotionFiles.insert(file);
								}
							}

							if(verifiedMotionFiles.empty() == false){
								//motion
								auto motion = new MotionShallowCopy::Trial();

								motion->attrs            = motionIT->second->attrs;
								//to bedziemy później aktualizować
								motion->session          = nullptr;
								motion->trialDescription = motionIT->second->trialDescription;
								motion->trialID          = motionIT->second->trialID;
								motion->trialName        = motionIT->second->trialName;

								//teraz aktualizujemy pliki
								for(auto it = verifiedMotionFiles.begin(); it != verifiedMotionFiles.end(); ++it){
									(*it)->trial = motion;
									auto val     = MotionShallowCopy::Files::value_type((*it)->fileID, *it);
									outShallow.motionShallowCopy->files.insert(val);
									motion->files.insert(val);
								}

								verifiedMotions.insert(motion);
							}
						}
					}

					if(verifiedMotions.empty() == false || verifiedSessionFiles.empty() == false){

						auto perfConf             = new MotionShallowCopy::PerformerConf();
						perfConf->attrs           = perfConfIT->second->attrs;
						//to ustawiamy pozniej
						perfConf->performer       = nullptr;
						perfConf->performerConfID = perfConfIT->second->performerConfID;
						auto session              = new MotionShallowCopy::Session();
						perfConf->session         = session;

						session->attrs              = perfConfIT->second->session->attrs;
						session->labID              = perfConfIT->second->session->labID;
						session->motionKind         = perfConfIT->second->session->motionKind;
						session->performerConf      = perfConf;
						session->sessionDate        = perfConfIT->second->session->sessionDate;
						session->sessionDescription = perfConfIT->second->session->sessionDescription;
						session->sessionID          = perfConfIT->second->session->sessionID;
						session->sessionName        = perfConfIT->second->session->sessionName;
						session->tags               = perfConfIT->second->session->tags;
						session->userID             = perfConfIT->second->session->userID;

						for(auto it = verifiedSessionFiles.begin(); it != verifiedSessionFiles.end(); ++it){
							(*it)->session = session;
							auto val = MotionShallowCopy::Files::value_type((*it)->fileID, *it);
							//zapisz plik
							outShallow.motionShallowCopy->files.insert(val);
							//aktualizuj sesję
							session->files.insert(val);											
						}

						for(auto it = verifiedMotions.begin(); it != verifiedMotions.end(); ++it){
							(*it)->session = session;
							auto val = MotionShallowCopy::Trials::value_type((*it)->trialID, *it);
							//zapisz motion
							outShallow.motionShallowCopy->trials.insert(val);
							//aktualizuj sesję - to później
							session->trials.insert(val);											
						}

						verifiedSessions.insert(session);
					}
				}
			}

			if(verifiedSessions.empty() == false){

				//później trzeba poprawić o dostępny zakres i nowe instancje patient->disorders

				auto performer             = new MotionShallowCopy::Performer();
				performer->attrs           = subjectIT->second->attrs;
				performer->patient         = nullptr;
				performer->performerID     = subjectIT->second->performerID;

				for(auto it = verifiedSessions.begin(); it != verifiedSessions.end(); ++it){
					(*it)->performerConf->performer = performer;
					auto val = MotionShallowCopy::PerformerConfs::value_type((*it)->performerConf->performerConfID, (*it)->performerConf);
					outShallow.motionShallowCopy->performerConfs.insert(val);
					performer->performerConfs.insert(val);
					outShallow.motionShallowCopy->sessions.insert(MotionShallowCopy::Sessions::value_type((*it)->sessionID, *it));
				}

				//zapisuje subjecta
				outShallow.motionShallowCopy->performers.insert(MotionShallowCopy::Performers::value_type(performer->performerID, performer));
			}
		}
	}
}
