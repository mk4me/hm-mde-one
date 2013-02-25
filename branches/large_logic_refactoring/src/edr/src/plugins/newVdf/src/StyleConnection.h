/********************************************************************
	created:	2013/02/01
	created:	1:2:2013   13:31
	filename: 	StyleConnection.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__STYLECONNECTION_H__
#define HEADER_GUARD_NEWVDF__STYLECONNECTION_H__

#include <plugins/newVdf/IVisualConnection.h>
#include <plugins/newVdf/IVisualPin.h>

namespace vdf {

class StyleItem;
class StyleConnection : public QObject, public IVisualConnection
{
    Q_OBJECT;
public:
    StyleConnection();
    virtual ~StyleConnection() {}

    virtual QGraphicsItem * visualItem() const;

    virtual void setInputPin(IVisualInputPinPtr pin );
    virtual void setOutputPin(IVisualOutputPinPtr pin );

	virtual IVisualInputPinPtr getInputPin();
	virtual IVisualOutputPinPtr getOutputPin();

	virtual void setVisualStrategy( IVisualStrategyPtr strategy );

    virtual bool isSelected() const;
    virtual void setSelected(bool val);

public slots:
    void update();

private:
    IVisualInputPinPtr inputPin;
    IVisualOutputPinPtr outputPin;
    StyleItem* item;
};

}

#endif
