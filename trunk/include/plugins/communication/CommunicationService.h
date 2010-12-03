#ifndef COMMUNICATION_SERVICE_H
#define COMMUNICATION_SERVICE_H

#include <core/IService.h>
#include <OpenThreads/Thread>
#include <core/IDataManager.h>
#include <plugins/communication/ICommunication.h>
#include <plugins/communication/CommunicationManager.h>
#include <plugins/communication/TransportWSDL_FTPS.h>
#include <plugins/communication/QueryWSDL.h>
#include "CommunicationWidget.h"

class CommunicationWidget;

class CommunicationService : public IService, public ICommunication, public OpenThreads::Thread
{
	UNIQUE_ID('COMM','SRVC');
private:
	CommunicationWidget* widget;
	communication::CommunicationManager* model;
	communication::TransportWSDL_FTPS* transport;
	communication::QueryWSDL* query;
	std::string name;
    OpenThreads::Mutex downloadMutex;
	bool downloading;
	unsigned int actualSession;
	unsigned int actualTrial;
	unsigned int actualFile;

public:
	CommunicationService();
	virtual ~CommunicationService();

	virtual IWidget* getWidget()
	{ 
		// HACK: ca³y ten system jest shackowany!
		return reinterpret_cast<IWidget*>(widget);
	}

	virtual const std::string& getName() const
	{
		return name;
	}

	virtual void downloadFile(unsigned int sessionID, unsigned int trialID, unsigned int fileID);
	virtual void downloadTrial(unsigned int sessionID, unsigned int trialID);

	virtual void updateSessionContents();

	void load();
	void save();

	const communication::TransportWSDL_FTPS* getTransportManager() const;
	const communication::QueryWSDL* getQueryManager() const;

	void setQueryCredentials(const std::string& user, const std::string& password, const std::string& bqsUri, const std::string& busUri);
	void setTransportFTPCredentials(const std::string& user, const std::string& password, const std::string& uri);
	void setTransportWSCredentials(const std::string& user, const std::string& password, const std::string& uri);

    virtual AsyncResult loadData(IServiceManager* serviceManager, IDataManager* dataManager);

	virtual AsyncResult update(double time, double timeDelta);
	virtual void run();
};
#endif
