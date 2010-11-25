#ifndef ICOMMUNICATION_H
#define ICOMMUNICATION_H

class ICommunication
{
public:
    virtual ~ICommunication() {}

	virtual void listLabSessionsWithAttributes(unsigned int labID) = 0;
	virtual void listSessionTrials(unsigned int sessionID) = 0;
	virtual void listTrialFiles(unsigned int sessionID, unsigned int trialID) = 0;

	virtual void updateSessionContents() = 0;

	virtual void downloadFile(unsigned int sessionID, unsigned int trialID, unsigned int fileID) = 0;
};
#endif
