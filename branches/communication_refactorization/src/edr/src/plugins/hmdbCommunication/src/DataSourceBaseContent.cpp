#include "CommunicationPCH.h"
#include "DataSourceBaseContent.h"
#include <QtCore/QObject>

using namespace hmdbCommunication;

DataSourceBaseContent::DataSourceBaseContent()
{

}

DataSourceBaseContent::~DataSourceBaseContent()
{

}

const std::string DataSourceBaseContent::name() const
{
	return "Base Content";
}

const bool DataSourceBaseContent::fillContent(QTreeWidgetItem * item, const hmdbCommunication::ShallowCopy & shallowCopy) const
{
	bool ret = false;
	auto content = dynamic_cast<TreeWidgetContentItem*>(item);
	if(content != nullptr){
		//wystarczy switch + dynamic_cast
		switch (content->contentType()){

		case PatientContent:
		{
			auto patientIT = shallowCopy.medicalShallowCopy.patients.find(content->id());
			if (patientIT != shallowCopy.medicalShallowCopy.patients.end()){
				auto p = patientIT->second;
				item->setText(0, QString::fromUtf8((p->surname + ", " + p->name).c_str()));
				ret = true;
			}
		}
			break;
		case SubjectContent:
		{
			auto subjectIT = shallowCopy.motionShallowCopy.performers.find(content->id());
			if (subjectIT != shallowCopy.motionShallowCopy.performers.end()){
				auto s = subjectIT->second;
				item->setText(0, QObject::tr("Subject") + " " + QString::number(s->performerID));
				ret = true;
			}
		}
			break;
		case SessionContent:
		{
			auto sessionIT = shallowCopy.motionShallowCopy.sessions.find(content->id());
			if (sessionIT != shallowCopy.motionShallowCopy.sessions.end()){
				auto s = sessionIT->second;
				item->setText(0, QString::fromStdString(s->sessionName));
				ret = true;
			}
		}
			break;
		case MotionContent:
		{
			auto motionIT = shallowCopy.motionShallowCopy.trials.find(content->id());
			if (motionIT != shallowCopy.motionShallowCopy.trials.end()){
				auto m = motionIT->second;
				item->setText(0, QString::fromStdString(m->trialName));
				ret = true;
			}
		}
			break;
		case FileContent:
		{
			auto fileIT = shallowCopy.motionShallowCopy.files.find(content->id());
			if (fileIT != shallowCopy.motionShallowCopy.files.end()){
				auto f = fileIT->second;
				item->setText(0, QString::fromStdString(f->fileName));
				ret = true;
			}
		}
			break;
		}
	}

	return ret;
}

const bool DataSourceBaseContent::headers(const QTreeWidgetItem * item, QStringList & headers) const
{
	bool ret = false;
	auto content = dynamic_cast<const TreeWidgetContentItem*>(item);
	if(content != nullptr){
		//wystarczy switch + dynamic_cast
		switch(content->contentType()){

		case PatientContent:
			headers << QObject::tr("Patient");
			ret = true;
			break;
		case SubjectContent:
			headers << QObject::tr("Subject");
			ret = true;
			break;
		case SessionContent:
			headers << QObject::tr("Session");
			ret = true;
			break;
		case MotionContent:
			headers << QObject::tr("Motion");
			ret = true;
			break;
		case FileContent:
			headers << QObject::tr("File");
			ret = true;
			break;
		}
	}

	return ret;
}

const int DataSourceBaseContent::statusField(const QTreeWidgetItem * item) const
{
	return 0;
}

const int DataSourceBaseContent::downloadField(const QTreeWidgetItem * item) const
{
	return 1;
}

void DataSourceBaseContent::release()
{

}
