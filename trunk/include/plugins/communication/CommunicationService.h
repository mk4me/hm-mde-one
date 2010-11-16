#ifndef COMMUNICATION_SERVICE_H
#define COMMUNICATION_SERVICE_H

#include <core/IService.h>
#include "CommunicationWidget.h"

class CommunicationService : public IService
{
    UNIQUE_ID('COMM','SRVC');
private:
    // TODO
    // tymczasowe
    QWidget* widget;
    //! Nazwa.
    std::string name;

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
};
#endif
