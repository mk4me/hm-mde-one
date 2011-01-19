#ifndef OSG_CONTROL_WIDGET_H
#define OSG_CONTROL_WIDGET_H

#include <core/SmartPtr.h>
#include <QtGui/QWidget>
#include <osgViewer/Scene>
#include <osg/Group>
#include <osg/Geode>
#include <osg/NodeVisitor>
#include <string>

#include "ui_OsgWidget.h"
#include <plugins/animation/AnimationService.h>

class IServiceManager;

class OsgControlWidget: public QWidget, Ui::OsgTest
{
    Q_OBJECT
public:


    OsgControlWidget(void);
    ~OsgControlWidget(void);

    void SetScene(osg::Node *scene); 
    void SetScene(osg::Node *scene, IServiceManager *pServiceManager); 
    void ClearScene();

    void AddMarkerTODisplayPathList(int markerNumber);
    void ClearMarkerList();

    public slots: 

        virtual void followTimelineChecked(int checked);
        virtual void animationSelectionChanged();
		virtual void scaleValueChanged(QString scale);
        virtual void MeshChecked(int checked);
        virtual void BoneChecked(int checked);
        virtual void MarkerChecked(int checked);
        virtual void MarkerListSelectionChanged();

private:
    AnimationService* m_pAnimationService;

    std::vector<int > m_markerSelectedList;
};


#endif
