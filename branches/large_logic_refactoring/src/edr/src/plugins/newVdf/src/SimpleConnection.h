/********************************************************************
	created:	2012/12/20
	created:	20:12:2012   13:57
	filename: 	SimpleConnection.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__SIMPLECONNECTION_H__
#define HEADER_GUARD_NEWVDF__SIMPLECONNECTION_H__

#include <plugins/newVdf/IVisualConnection.h>


class SimpleConnection : public QObject, public IVisualConnection
{
    Q_OBJECT;
public:
    SimpleConnection();
    virtual ~SimpleConnection() {}

    virtual QGraphicsItem * visualItem() const;

    virtual void setBegin( IVisualPinPtr pin );
	virtual void setEnd( IVisualPinPtr pin );

	virtual IVisualPinPtr getBegin();
	virtual IVisualPinPtr getEnd();

	virtual void setVisualStrategy(IVisualStrategyPtr strategy);

public slots:
    void update();


private:
    IVisualPinPtr begin;
    IVisualPinPtr end;
    QGraphicsLineItem* item;
};


#endif
