/********************************************************************
	created:	2013/02/28
	created:	28:2:2013   18:31
	filename: 	HmmSources.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__HMMSOURCES_H__
#define HEADER_GUARD_HMM__HMMSOURCES_H__

#include <plugins/dfElements/DFPins.h>
#include <utils/ObjectWrapper.h>
#include <QtGui/QTreeWidget>
#include <QtGui/QIcon>
#include "HmmMainWindow.h"
#include "FilterCommand.h"
#include "Vector3DFilterCommand.h"


// TODO : ujednolicic

class VectorSource : public df::SourceNode, public df::IDFSource, public vdf::INodeConfiguration, public vdf::INodeValidation
{
public:
    VectorSource (HmmMainWindow* hmm, BuilderFilterCommand::BranchFunction fun, const QIcon& rootIcon, const QIcon& leafIcon);
    virtual ~VectorSource();
public:
    virtual void reset();
    virtual const bool empty() const;
    virtual void produce();
    virtual QWidget* getConfigurationWidget();
    virtual bool isNodeValid();
    virtual QString getErrorMessage();


public:
    HmmMainWindow* getHmm() const { return hmm; }
    const QIcon& getLeafIcon() const { return leafIcon; }
    BuilderFilterCommand::BranchFunction getBranchFunction() const { return branchFunction; }
    const QIcon& getRootIcon() const { return rootIcon; }

private:
    virtual void refreshConfiguration();

private:
    VectorOutputPin * outPinA;
    QTreeWidget* tree;
    VectorChannelReaderInterfaceConstPtr channel;
    bool used;
    HmmMainWindow* hmm;
    BuilderFilterCommand::BranchFunction branchFunction;
    QIcon rootIcon;
    QIcon leafIcon;
};


class EMGSource : public df::SourceNode, public df::IDFSource, public vdf::INodeConfiguration, public vdf::INodeValidation
{
public:
    EMGSource (HmmMainWindow* hmm);
    virtual ~EMGSource();
public:
    virtual void reset();
    virtual const bool empty() const;
    virtual void produce();
    virtual QWidget* getConfigurationWidget();
    virtual bool isNodeValid();
    virtual QString getErrorMessage();


public:
    HmmMainWindow* getHmm() const { return hmm; }

private:
    virtual void refreshConfiguration();

private:
    ScalarOutputPin * outPinA;
    QTreeWidget* tree;
    ScalarChannelReaderInterfaceConstPtr channel;
    bool used;
    HmmMainWindow* hmm;
};

#endif
