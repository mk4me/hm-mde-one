#ifndef ICOMMUNICATION_H
#define ICOMMUNICATION_H

class ICommunication
{
public:
    virtual ~ICommunication() {}

	virtual void updateSessionContents() = 0;

	virtual void downloadFile(unsigned int sessionID, unsigned int trialID, unsigned int fileID) = 0;

	virtual void downloadTrial(unsigned int sessionID, unsigned int trialID) = 0;
};
#endif
