/********************************************************************
	created:	2011/07/21
	created:	21:7:2011   18:43
	filename: 	SubjectService.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_SUBJECT__SUBJECTSERVICE_H__
#define HEADER_GUARD_SUBJECT__SUBJECTSERVICE_H__

#include <core/IService.h>
#include <core/IDataManager.h>
#include <utils/ObserverPattern.h>
#include <core/PluginCommon.h>
#include <utils/signalslib.hpp>
#include <plugins/subject/Motion.h>
#include <plugins/subject/Session.h>
#include <plugins/subject/Subject.h>


class SubjectService : public core::IService, boost::signalslib::trackable
{
	UNIQUE_ID("{F39418DE-4BCB-46C1-8D84-93F435641C63}", "Subject Service");
public:
	SubjectService();
	virtual ~SubjectService();

public:
	//! 
	virtual void finalize();
	//! 
	//! \param actions 
	virtual QWidget* getWidget( std::vector<QObject*>& actions );
	//! 
	//! \param actions 
	virtual QWidget* getSettingsWidget( std::vector<QObject*>& actions );
	//! 
	virtual const std::string& getName() const;
	//! 
	//! \param actions 
	virtual QWidget* getControlWidget( std::vector<QObject*>& actions );

	void onFileLoaded(const core::Filesystem::Path& path, bool loaded);
	void onWrappersAdded(const std::vector<core::ObjectWrapperPtr>& wrappers, bool loaded);

private:
	struct fileDescriptor 
	{
		int year;
		int month;
		int day;
		std::string body;
		std::string session;
		std::string motion;
		std::string extension;
		std::string additional;

		std::string getSessionDesc() const
		{
			std::stringstream ss;
			if (year && month && day && body.size() && session.size()) {
				ss << year << "-" << month << "-" << day << "-";
				ss << body << "-" << session;
				return ss.str();
			} else {
				throw std::runtime_error("Wrong file descriptor");
			}

		}

		bool hasMotionDesc() const
		{
			return year && month && day && body.size() && session.size() && motion.size();
		}

		std::string getMotionDesc() const
		{
			if (!motion.size()) {
				throw std::runtime_error("fileDescriptor: motion was not set");
			}
			std::stringstream ss;
			ss << getSessionDesc() << "-" << motion;
			return ss.str();
		}

		fileDescriptor()
		{
			clear();
		}

		void clear()
		{
			year = -1;
			month = -1;
			day = -1;
			body = "";
			session = "";
			extension = "";
			additional = "";
			motion = "";
		}
	};
	typedef boost::shared_ptr<fileDescriptor> fileDescriptorPtr;
	typedef boost::shared_ptr<const fileDescriptor> fileDescriptorConstPtr;

private:
	typedef std::map<std::string, SessionPtr> SessionMap;
	typedef std::map<std::string, MotionPtr> MotionMap;
	SessionMap sessions;
	MotionMap motions;
	fileDescriptor lastFileDescriptor;

private:
	void getFileDescriptor(const std::string& filename, fileDescriptor& descriptor);
};
typedef core::shared_ptr<SubjectService> SubjectServicePtr;
typedef core::shared_ptr<const SubjectService> SubjectServiceConstPtr;

#endif
