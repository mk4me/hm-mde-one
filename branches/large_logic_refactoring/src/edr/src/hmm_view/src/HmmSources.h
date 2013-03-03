/********************************************************************
	created:	2013/02/28
	created:	28:2:2013   18:31
	filename: 	HmmSources.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__HMMSOURCES_H__
#define HEADER_GUARD_HMM__HMMSOURCES_H__

#include "HmmPins.h"
#include <utils/ObjectWrapper.h>
#include <QtGui/QTreeWidget>
#include <QtGui/QIcon>
#include "HmmMainWindow.h"
#include "FilterCommand.h"
#include "Vector3DFilterCommand.h"


class XSource : public df::SourceNode, public df::IDFSource, public vdf::INodeConfiguration
{
public:
    XSource (HmmMainWindow* hmm, BuilderFilterCommand::BranchFunction fun, const QIcon& rootIcon, const QIcon& leafIcon);
    virtual ~XSource();
public:
    virtual void reset();
    virtual const bool empty() const;
    virtual void produce();
    virtual QWidget* getConfigurationWidget() const;

public:
    HmmMainWindow* getHmm() const { return hmm; }
    const QIcon& getLeafIcon() const { return leafIcon; }
    BuilderFilterCommand::BranchFunction getBranchFunction() const { return branchFunction; }
    const QIcon& getRootIcon() const { return rootIcon; }

private:
    virtual void refreshConfiguration();

private:
    XOutputPin * outPinA;
    QTreeWidget* tree;
    VectorChannelReaderInterfaceConstPtr channel;
    bool used;
    HmmMainWindow* hmm;
    BuilderFilterCommand::BranchFunction branchFunction;
    QIcon rootIcon;
    QIcon leafIcon;
};

#endif
