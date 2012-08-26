/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   9:10
	filename: 	IConfigurable.h
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__ICONFIGURABLE_H__
#define HEADER_GUARD_CORE__ICONFIGURABLE_H__

//! Forward declaration
class QWidget;

namespace core {

//! Klasa zapewniająca widget konfiguracyjny
class IConfigurable
{
public:
    virtual ~IConfigurable() 
    {
    }
    //! \return Widget konfigurujący dany obiekt
    virtual QWidget* getConfigurationWidget() = 0;
};

}
#endif
