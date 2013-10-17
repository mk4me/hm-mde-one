/********************************************************************
	created:	2013/05/08
	created:	8:5:2013   11:56
	filename: 	DFSinks.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DFELEMENTS__DFSINKS_H__
#define HEADER_GUARD_DFELEMENTS__DFSINKS_H__

#include <plugins/dfElements/DFPins.h>
#include <QtGui/QLineEdit>
#include <corelib/HierarchyHelper.h>
#include <corelib/HierarchyDataItem.h>
#include <plugins/dfElements/DFPins.h>
#include <QtGui/QDoubleSpinBox>



class UniversalSink : public QObject, public df::SinkNode, public df::IDFSink, public vdf::INodeConfiguration
{
    Q_OBJECT;
public:
    UniversalSink();

    virtual void reset();
    virtual void consume();

    //void insertToTree( core::HierarchyHelperPtr channelHelper);//, utils::ObjectWrapperPtr wrp );

    virtual QWidget* getConfigurationWidget();
    // todo: ta metoda nie jest potrzebna, bo mamy sygnal
    // problem w tym ,ze sygnal nie dziala z jakiegos wzgledu
    core::IHierarchyItemPtr getDataItem() const { return dataItem; }

private:
    void _UniversalSink();

signals:
    void itemConsumed(core::IHierarchyItemPtr dataItem);

private:
    UniversalInputPinBase * inPinA;
    QWidget* widget;
    QLineEdit* line;
    QString defaultName;
    int runNo;
    core::IHierarchyItemPtr dataItem;
};


#endif