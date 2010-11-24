#ifndef COMMUNICATION_SERVICE_H
#define COMMUNICATION_SERVICE_H

#include <core/IService.h>
#include <OpenThreads/Thread>
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
    CommunicationWidget* m_widget;
	communication::CommunicationManager* m_model;
	communication::TransportWSDL_FTPS* m_transport;
	communication::QueryWSDL* m_query;
    std::string m_name;

public:
    CommunicationService();
	virtual ~CommunicationService();

    virtual IWidget* getWidget()
    { 
        // HACK: ca³y ten system jest shackowany!
        return reinterpret_cast<IWidget*>(m_widget);
    }

    virtual const std::string& getName() const
    {
        return m_name;
    }

	virtual void listLabSessionsWithAttributes(unsigned int lab_id);
	virtual void listSessionTrials(unsigned int session_id);
	virtual void listTrialFiles(unsigned int session_id, unsigned int trial_id);

	virtual void downloadFile(unsigned int session_id, unsigned int trial_id, unsigned int file_id);
	virtual void updateSessionContents();

	void load();
	void save();

	const communication::TransportWSDL_FTPS* getTransportManager() const;
	const communication::QueryWSDL* getQueryManager() const;

	void setQueryCredentials(const std::string& user, const std::string& password, const std::string& bqs_uri, const std::string& bus_uri);
	void setTransportFTPCredentials(const std::string& user, const std::string& password, const std::string& uri);
	void setTransportWSCredentials(const std::string& user, const std::string& password, const std::string& uri);

	virtual void run();
};
#endif
