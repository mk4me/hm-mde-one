/********************************************************************
    created:  2011/04/19
    created:  19:4:2011   7:55
    filename: VdfService.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___VDFSERVICE_H__
#define HEADER_GUARD___VDFSERVICE_H__

#include <core/IService.h>

class VdfWidget;

class VdfService : public core::IService
{
    UNIQUE_ID("{393D2CE4-C254-408B-AF94-61FAD29E30F3}", "Visual Data Flow (VDF) Service");
private:
    //! Widget.
    VdfWidget* widget;    
    //!
    std::string name;

public:
    VdfService();
    virtual ~VdfService();

    // IService
public:
    //!
    virtual QWidget* getWidget(std::vector<QObject*>& actions);
    
    //!
    void init(core::IServiceManager* serviceManager, core::IDataManager* dataManager);

    //!
    virtual const std::string& getName() const;
};

#endif  //HEADER_GUARD___VDFSERVICE_H__

