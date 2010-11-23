#ifndef ICOMMUNICATION_H
#define ICOMMUNICATION_H

class ICommunication
{
public:
    virtual ~ICommunication() {}

	virtual void listLabSessionsWithAttributes(unsigned int lab_id) = 0;
	virtual void listSessionTrials(unsigned int session_id) = 0;
	virtual void listTrialFiles(unsigned int session_id, unsigned int trial_id) = 0;

	virtual void updateSessionContents() = 0;

	virtual void downloadFile(unsigned int file_id) = 0;
};
#endif
