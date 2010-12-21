#ifndef COMMUNICATION_SERVICE_H
#define COMMUNICATION_SERVICE_H

#include <core/IService.h>
#include <plugins/communication/ICommunication.h>
#include <plugins/communication/CommunicationManager.h>
#include <plugins/communication/TransportWSDL_FTPS.h>
#include <plugins/communication/QueryWSDL.h>
#include "CommunicationWidget.h"

class CommunicationWidget;

class CommunicationService : public IService, public ICommunication
{
	UNIQUE_ID('COMM','SRVC');
private:
	CommunicationWidget* widget;
	communication::CommunicationManager* model;
	communication::TransportWSDL_FTPS* transport;
	communication::QueryWSDL* query;
	std::string name;

public:
	CommunicationService();
	virtual ~CommunicationService();

	//IService
	virtual AsyncResult init(IServiceManager* serviceManager, osg::Node* sceneRoot, IDataManager* dataManager);
    virtual AsyncResult loadData(IServiceManager* serviceManager, IDataManager* dataManager);
	virtual AsyncResult update(double time, double timeDelta);
	virtual IWidget* getWidget()
	{ 
		// HACK: ca³y ten system jest shackowany!
		//return reinterpret_cast<IWidget*>(widget);
        return NULL;
	}
    virtual IWidget* getSettingsWidget()
    {
        return reinterpret_cast<IWidget*>(widget);
    }

	virtual const std::string& getName() const
	{
		return name;
	}
	//ICommunication
	virtual void updateSessionContents();
	virtual void downloadTrial(unsigned int trialID);
	virtual void downloadFile(unsigned int fileID);
	virtual void loadTrial(const std::string& name);
	virtual bool ping();
	//TODO: sprzatanie w folderach po przerwaniu jak i po bledach
	virtual void cancelDownloading();

	void load();
	void save();

	const communication::TransportWSDL_FTPS* getTransportManager() const;
	const communication::QueryWSDL* getQueryManager() const;

	void setQueryCredentials(const std::string& user, const std::string& password, const std::string& bqsUri, const std::string& busUri);
	void setTransportFTPCredentials(const std::string& user, const std::string& password, const std::string& uri);
	void setTransportWSCredentials(const std::string& user, const std::string& password, const std::string& uri);
};
#endif
