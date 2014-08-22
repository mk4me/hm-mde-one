#include "CommunicationPCH.h"
#include "DataSourcePerspective.h"
#include <QtGui/QMouseEvent>

using namespace hmdbCommunication;

PerspectiveTreeWidget::PerspectiveTreeWidget(QWidget * parent) : QTreeWidget(parent)
{
}

PerspectiveTreeWidget::~PerspectiveTreeWidget()
{
}

void PerspectiveTreeWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton && event->modifiers() == Qt::ControlModifier){
		auto item = itemAt(event->pos());

		if (item == nullptr || item->isSelected() == true){
			clearSelection();
			return;
		}
	}

	QTreeWidget::mousePressEvent(event);
}

DataSourcePerspective::DataSourcePerspective(const hmdbCommunication::IDataSourcePerspectivePtr & perspective)
: perspective_(perspective), treeWidget(new PerspectiveTreeWidget()), invalid_(true)
{
}

DataSourcePerspective::~DataSourcePerspective()
{
}

const hmdbCommunication::IDataSourcePerspectivePtr & DataSourcePerspective::perspective() const
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

void DataSourcePerspective::rebuildPerspective(const hmdbCommunication::ShallowCopy & shallowCopy)
{
	perspective_->rebuildPerspective(treeWidget->invisibleRootItem(), shallowCopy);
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

const QString DataSourcePatientPerspective::name() const
{
	return QObject::tr("Patients");
}

void DataSourcePatientPerspective::rebuildPerspective(QTreeWidgetItem * treeWidgetItem, const ShallowCopy & shallowCopy)
{
	auto subjectsITEnd = shallowCopy.motionShallowCopy.performers.end();
	for (auto subjectIT = shallowCopy.motionShallowCopy.performers.begin(); subjectIT != subjectsITEnd; ++subjectIT){
		//jeśli pusty pacjent to go pomijamy
		if (subjectIT->second->performerConfs.empty() == true){
			continue;
		}

		//generuje item pacjenta
		QTreeWidgetItem * item = nullptr;
		if (subjectIT->second->patient != nullptr){
			item = new TreeWidgetContentItem(PatientContent, subjectIT->second->patient->patientID);
		}
		else{
			item = new TreeWidgetContentItem(SubjectContent, subjectIT->second->performerID);
		}

		treeWidgetItem->addChild(item);

		auto perfConfsITEnd = subjectIT->second->performerConfs.end();
		for (auto perfConfIT = subjectIT->second->performerConfs.begin(); perfConfIT != perfConfsITEnd; ++perfConfIT){
			if (perfConfIT->second->session->trials.empty() == true){
				continue;
			}

			//generuje item sesji
			auto sessionItem = new TreeWidgetContentItem(SessionContent, perfConfIT->second->session->sessionID);
			item->addChild(sessionItem);

			auto motionsITEnd = perfConfIT->second->session->trials.end();
			for (auto motionIT = perfConfIT->second->session->trials.begin(); motionIT != motionsITEnd; ++motionIT){
				if (motionIT->second->files.empty() == true){
					continue;
				}

				//generuje item motiona
				auto motionItem = new TreeWidgetContentItem(MotionContent, motionIT->second->trialID);
				sessionItem->addChild(motionItem);
			}
		}
	}
}

DataSourceMedusaPerspective::DataSourceMedusaPerspective()
{

}

DataSourceMedusaPerspective::~DataSourceMedusaPerspective()
{

}

void DataSourceMedusaPerspective::defaultHeaders(QStringList & headers) const
{
	headers << QObject::tr("Sessions");
}

const QString DataSourceMedusaPerspective::name() const
{
	return QObject::tr("Sessions only");
}

const int sessionLocalNumebr(const std::string & sessionName)
{
	int ret = -1;

	auto sIdx = sessionName.find("-S");
	if (sIdx != std::string::npos){
		sIdx += 2;
		const auto indexString = sessionName.substr(sIdx, sessionName.size() - sIdx);
		if (indexString.empty() == false){
			try{
				ret = boost::lexical_cast<int>(indexString);
			}
			catch (...){

			}
		}
	}
	
	return ret;
}

void DataSourceMedusaPerspective::rebuildPerspective(QTreeWidgetItem * treeWidgetItem, const ShallowCopy & shallowCopy)
{
	std::multimap<hmdbServices::ID, TreeWidgetContentItem*> sessionItems;
	std::list<TreeWidgetContentItem*> unrecognizedSessionItems;

	auto subjectsITEnd = shallowCopy.motionShallowCopy.performers.end();
	for (auto subjectIT = shallowCopy.motionShallowCopy.performers.begin(); subjectIT != subjectsITEnd; ++subjectIT){
		//jeśli pusty pacjent to go pomijamy
		if (subjectIT->second->performerConfs.empty() == true){
			continue;
		}

		auto perfConfsITEnd = subjectIT->second->performerConfs.end();
		for (auto perfConfIT = subjectIT->second->performerConfs.begin(); perfConfIT != perfConfsITEnd; ++perfConfIT){
			if (perfConfIT->second->session->trials.empty() == true){
				continue;
			}

			//generuje item sesji
			auto sessionItem = new TreeWidgetContentItem(SessionContent, perfConfIT->second->session->sessionID);

			const auto sNumber = sessionLocalNumebr(perfConfIT->second->session->sessionName);

			if (sNumber > -1){
				sessionItems.insert(std::multimap<hmdbServices::ID, TreeWidgetContentItem*>::value_type(sNumber,sessionItem));
			}
			else{
				unrecognizedSessionItems.push_back(sessionItem);
			}

			auto motionsITEnd = perfConfIT->second->session->trials.end();
			for (auto motionIT = perfConfIT->second->session->trials.begin(); motionIT != motionsITEnd; ++motionIT){
				if (motionIT->second->files.empty() == true){
					continue;
				}

				//generuje item motiona
				auto motionItem = new TreeWidgetContentItem(MotionContent, motionIT->second->trialID);

				sessionItem->addChild(motionItem);
			}
		}
	}

	for (auto it = sessionItems.begin(); it != sessionItems.end(); ++it){
		treeWidgetItem->addChild(it->second);
	}

	for (auto it = unrecognizedSessionItems.begin(); it != unrecognizedSessionItems.end(); ++it){
		treeWidgetItem->addChild(*it);
	}
}

const bool DataSourceMedusaPerspective::headers(const QTreeWidgetItem * item, QStringList & headers) const
{
	headers << QObject::tr("Database");
	return true;
}

DataSourceDisorderPerspective::DataSourceDisorderPerspective()
{

}

DataSourceDisorderPerspective::~DataSourceDisorderPerspective()
{

}

const QString DataSourceDisorderPerspective::name() const
{
	return QObject::tr("Disorders");
}

void DataSourceDisorderPerspective::rebuildPerspective(QTreeWidgetItem * treeWidgetItem, const hmdbCommunication::ShallowCopy & shallowCopy)
{
	std::map<hmdbServices::MedicalShallowCopy::Disorder*, std::set<hmdbServices::MedicalShallowCopy::Patient*>> patientsByDisorders;

	auto patientsITEnd = shallowCopy.medicalShallowCopy.patients.end();
	for (auto patientIT = shallowCopy.medicalShallowCopy.patients.begin(); patientIT != patientsITEnd; ++patientIT){
		for (auto disorderIT = patientIT->second->disorders.begin(); disorderIT != patientIT->second->disorders.end(); ++disorderIT){
			patientsByDisorders[shallowCopy.medicalShallowCopy.disorders.find(disorderIT->first)->second].insert(patientIT->second);
		}
	}

	auto disordersITEnd = patientsByDisorders.end();
	for (auto disorderIT = patientsByDisorders.begin(); disorderIT != disordersITEnd; ++disorderIT){
		if (disorderIT->second.empty() == true){
			continue;
		}

		auto disorderItem = new TreeWidgetContentItem(CustomContent, disorderIT->first->disorderID);

		treeWidgetItem->addChild(disorderItem);

		auto patientsITEnd = disorderIT->second.end();
		for (auto patientIT = disorderIT->second.begin(); patientIT != patientsITEnd; ++patientIT){
			//generuje item pacjenta
			auto patientItem = new TreeWidgetContentItem(PatientContent, (*patientIT)->patientID);

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

const QString DataSourceGenderPerspective::name() const
{
	return QObject::tr("Gender");
}

void DataSourceGenderPerspective::rebuildPerspective(QTreeWidgetItem * treeWidgetItem, const hmdbCommunication::ShallowCopy & shallowCopy)
{
	std::map<hmdbServices::xmlWsdl::Gender::Type, std::set<hmdbServices::MedicalShallowCopy::Patient*>> patientsByGender;

	auto patientsITEnd = shallowCopy.medicalShallowCopy.patients.end();
	for (auto patientIT = shallowCopy.medicalShallowCopy.patients.begin(); patientIT != patientsITEnd; ++patientIT){
		patientsByGender[patientIT->second->gender].insert(patientIT->second);
	}

	auto gendersITEnd = patientsByGender.end();
	for (auto genderIT = patientsByGender.begin(); genderIT != gendersITEnd; ++genderIT){
		auto genderItem = new TreeWidgetContentItem(PatientsGroupContent, -1);
		contentMap[genderItem] = genderIT->first;

		treeWidgetItem->addChild(genderItem);

		auto patientsITEnd = genderIT->second.end();
		for (auto patientIT = genderIT->second.begin(); patientIT != patientsITEnd; ++patientIT){
			//generuje item pacjenta
			auto patientItem = new TreeWidgetContentItem(PatientContent, (*patientIT)->patientID);

			genderItem->addChild(patientItem);
		}
	}
}

const bool DataSourceGenderPerspective::fillContent(QTreeWidgetItem * item, const hmdbCommunication::ShallowCopy & shallowCopy) const
{
	auto contentIT = contentMap.find(item);
	if (contentIT != contentMap.end()){
		item->setText(0, decodeGender(contentIT->second));
		return true;
	}

	return false;
}

const bool DataSourceGenderPerspective::headers(const QTreeWidgetItem * item, QStringList & headers) const
{
	auto contentIT = contentMap.find(item);
	if (contentIT != contentMap.end()){
		headers << QObject::tr("Gender");
		return true;
	}

	return false;
}

void DataSourceGenderPerspective::release()
{
	std::map<const QTreeWidgetItem*, hmdbServices::xmlWsdl::Gender::Type>().swap(contentMap);
}