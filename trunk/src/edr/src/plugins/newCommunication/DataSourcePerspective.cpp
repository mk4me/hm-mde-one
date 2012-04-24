#include "CommunicationPCH.h"
#include "DataSourcePerspective.h"

using namespace communication;

DataSourcePerspective::DataSourcePerspective(const communication::DataSourcePerspectivePtr & perspective)
	: perspective_(perspective), treeWidget(new QTreeWidget()), invalid_(true)
{

}

DataSourcePerspective::~DataSourcePerspective()
{

}

const communication::DataSourcePerspectivePtr & DataSourcePerspective::perspective() const
{
	return perspective_;
}

void DataSourcePerspective::invalidate()
{
	invalid_ = true;
}

bool DataSourcePerspective::invalid() const
{
	return invalid_;
}

QTreeWidget * DataSourcePerspective::widget()
{
	return treeWidget;
}

void DataSourcePerspective::rebuildPerspective(const communication::ShallowCopy & shallowCopy)
{
	perspective_->rebuildPerspective(treeWidget, shallowCopy);
	invalid_ = false;
}

void DataSourcePerspective::clearPerspective()
{
	treeWidget->clear();
}

DataSourcePatientPerspective::DataSourcePatientPerspective()
{

}

DataSourcePatientPerspective::~DataSourcePatientPerspective()
{

}

const std::string DataSourcePatientPerspective::name() const
{
	return std::string("Patient");
}

void DataSourcePatientPerspective::rebuildPerspective(QTreeWidget * treeWidget, const communication::ShallowCopy & shallowCopy)
{
	//auto patientsITEnd = shallowCopy.medicalShallowCopy->patients.end();
	//for(auto patientIT = shallowCopy.medicalShallowCopy->patients.begin(); patientIT != patientsITEnd; ++patientIT){
	auto subjectsITEnd = shallowCopy.motionShallowCopy->performers.end();
	for(auto subjectIT = shallowCopy.motionShallowCopy->performers.begin(); subjectIT != subjectsITEnd; ++subjectIT){
		//generuje item pacjenta
		QTreeWidgetItem * item = nullptr;
		if(subjectIT->second->patient != nullptr){
			item = new PatientItem(subjectIT->second->patient);
		}else{
			item = new SubjectItem(subjectIT->second);
		}

		treeWidget->addTopLevelItem(item);

		auto perfConfsITEnd = subjectIT->second->performerConfs.end();
		for(auto perfConfIT = subjectIT->second->performerConfs.begin(); perfConfIT != perfConfsITEnd; ++ perfConfIT){

			//generuje item sesji
			auto sessionItem = new SessionItem(perfConfIT->second->session);

			item->addChild(sessionItem);

			auto motionsITEnd = perfConfIT->second->session->trials.end();
			for(auto motionIT = perfConfIT->second->session->trials.begin(); motionIT != motionsITEnd; ++motionIT){

				//generuje item motiona
				auto motionItem = new MotionItem(motionIT->second);

				sessionItem->addChild(motionItem);
			}
		}
	}
}

DataSourceDisorderPerspective::DataSourceDisorderPerspective()
{

}

DataSourceDisorderPerspective::~DataSourceDisorderPerspective()
{

}

const std::string DataSourceDisorderPerspective::name() const
{
	return std::string("Disorder");
}

void DataSourceDisorderPerspective::rebuildPerspective(QTreeWidget * treeWidget, const communication::ShallowCopy & shallowCopy)
{
	auto disordersITEnd = shallowCopy.medicalShallowCopy->patientsByDisorder.end();
	for(auto disorderIT = shallowCopy.medicalShallowCopy->patientsByDisorder.begin(); disorderIT != disordersITEnd; ++disorderIT){

		auto disorderItem = new DisorderItem(shallowCopy.medicalShallowCopy->disorders.find(disorderIT->first)->second);

		treeWidget->addTopLevelItem(disorderItem);

		auto patientsITEnd = disorderIT->second.end();
		for(auto patientIT = disorderIT->second.begin(); patientIT != patientsITEnd; ++patientIT){

			//generuje item pacjenta
			auto patientItem = new PatientItem(*patientIT);

			disorderItem->addChild(patientItem);
		}
	}
}

DataSourceGenderPerspective::DataSourceGenderPerspective()
{

}

DataSourceGenderPerspective::~DataSourceGenderPerspective()
{

}

const std::string DataSourceGenderPerspective::name() const
{
	return std::string("Gender");
}

void DataSourceGenderPerspective::rebuildPerspective(QTreeWidget * treeWidget, const communication::ShallowCopy & shallowCopy)
{
	auto gendersITEnd = shallowCopy.medicalShallowCopy->patientsByGender.end();
	for(auto genderIT = shallowCopy.medicalShallowCopy->patientsByGender.begin(); genderIT != gendersITEnd; ++genderIT){

		auto genderItem = new PatientsGroupItem(genderIT->second);
		contentMap[genderItem] = genderIT->first;

		treeWidget->addTopLevelItem(genderItem);

		auto patientsITEnd = genderIT->second.end();
		for(auto patientIT = genderIT->second.begin(); patientIT != patientsITEnd; ++patientIT){

			//generuje item pacjenta
			auto patientItem = new PatientItem(*patientIT);

			genderItem->addChild(patientItem);
		}
	}
}

bool DataSourceGenderPerspective::fillContent(QTreeWidgetItem * item, const communication::ShallowCopy & shallowCopy) const
{
	auto contentIT = contentMap.find(item);
	if(contentIT != contentMap.end()){
		item->setText(0, decodeGender(contentIT->second));
		return true;
	}

	return false;
}

bool DataSourceGenderPerspective::headers(const QTreeWidgetItem * item, QStringList & headers) const
{
	auto contentIT = contentMap.find(item);
	if(contentIT != contentMap.end()){
		headers << QObject::tr("Gender");
		return true;
	}

	return false;
}

void DataSourceGenderPerspective::release()
{
	contentMap.swap(std::map<const QTreeWidgetItem*, char>());
}