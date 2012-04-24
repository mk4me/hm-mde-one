#include "CommunicationPCH.h"
#include "DataSourceBaseContent.h"
#include <QtCore/QObject>

using namespace communication;

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

bool DataSourceBaseContent::fillContent(QTreeWidgetItem * item, const communication::ShallowCopy & shallowCopy) const
{
	bool ret = false;
	auto content = dynamic_cast<IContent*>(item);
	if(content != nullptr){
		//wystarczy switch + dynamic_cast
		switch(content->contentType()){
			
		case DisorderContent:
			{
				auto disorder = dynamic_cast<DisorderItem*>(item);
				auto d = disorder->value();
				disorder->setText(0, QString::fromUtf8(d->name.c_str()));
				ret = true;
			}
			break;
		case PatientContent:
			{
				auto patient = dynamic_cast<PatientItem*>(item);
				auto p = patient->value();
				patient->setText(0, QString::fromUtf8((p->surname + ", " + p->name).c_str()));
				ret = true;
			}
			break;
		case SubjectContent:
			{
				auto subject = dynamic_cast<SubjectItem*>(item);
				auto s = subject->value();
				subject->setText(0, QObject::tr("Subject") + " " + QString::number(s->performerID));
				ret = true;
			}
			break;
		case SessionContent:
			{
				auto session = dynamic_cast<SessionItem*>(item);
				auto s = session->value();
				session->setText(0, QString::fromStdString(s->sessionName));
				ret = true;
			}
			break;
		case MotionContent:
			{
				auto motion = dynamic_cast<MotionItem*>(item);
				auto m = motion->value();
				motion->setText(0, QString::fromStdString(m->trialName));
				ret = true;
			}
			break;
		case FileContent:
			{
				auto file = dynamic_cast<FileItem*>(item);
				auto f = file->value();
				file->setText(0, QString::fromStdString(f->fileName));
				ret = true;
			}
			break;
		case DisordersGroupContent:
			break;
		case PatientsGroupContent:
			break;
		case SubjectsGroupContent:
			break;
		case SessionsGroupContent:
			break;
		case MotionsGroupContent:
			break;
		case FilesGroupContent:
			break;
		case CustomContent:
			break;
		}
	}

	return ret;
}

bool DataSourceBaseContent::headers(const QTreeWidgetItem * item, QStringList & headers) const
{
	bool ret = false;
	auto content = dynamic_cast<const IContent*>(item);
	if(content != nullptr){
		//wystarczy switch + dynamic_cast
		switch(content->contentType()){

		case DisorderContent:
			headers << QObject::tr("Disorder");
			ret = true;
			break;
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
		case DisordersGroupContent:
			break;
		case PatientsGroupContent:
			break;
		case SubjectsGroupContent:		
			break;
		case SessionsGroupContent:			
			break;
		case MotionsGroupContent:		
			break;
		case FilesGroupContent:			
			break;
		case CustomContent:			
			break;
		}
	}

	return ret;
}

int DataSourceBaseContent::statusField(const QTreeWidgetItem * item) const
{
	return 0;
}

int DataSourceBaseContent::downloadField(const QTreeWidgetItem * item) const
{
	return 1;
}

void DataSourceBaseContent::release()
{

}