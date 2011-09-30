/********************************************************************
	created:	2011/09/29
	created:	29:9:2011   10:09
	filename: 	TreeItemHelper.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__TREEITEMHELPER_H__
#define HEADER_GUARD_HMM__TREEITEMHELPER_H__

#include <QtCore/QString>
#include <plugins/chart/ChartVisualizer.h>
#include <plugins/subject/Motion.h>
#include "Visualizer.h"

//! podstawowa klasa ulatwiajaca tworzenie wizualizatora na podstawie elementu drzewa
class TreeItemHelper
{
public:
    TreeItemHelper(const core::ObjectWrapperPtr& wrapper);
	virtual ~TreeItemHelper() {}
protected:
    TreeItemHelper() {}

public:
    virtual void createSeries(VisualizerPtr visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
    virtual VisualizerPtr createVisualizer();
    virtual bool isDataItem() const { return true; }

protected:
    static void setUpChart(ChartVisualizer* chart, const std::string& title);
    const core::ObjectWrapperPtr wrapper;

private:
    static osg::ref_ptr<osgText::Text> chartTextPrototype;
};
typedef boost::shared_ptr<TreeItemHelper> TreeItemHelperPtr;
typedef boost::shared_ptr<const TreeItemHelper> TreeItemHelperConstPtr;

//! klasa pomocnicza przy tworzeniu wizualizatora wykresow
class ChartItemHelper : public TreeItemHelper
{
public:
    ChartItemHelper(const core::ObjectWrapperPtr& wrapper);
    virtual VisualizerPtr createVisualizer();
    virtual void createSeries(VisualizerPtr visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
};

//! klasa pomocnicza przy tworzeniu wykresow z wektora 3-elementowego
class Vector3ItemHelper : public TreeItemHelper
{
public:
    Vector3ItemHelper(const core::ObjectWrapperPtr& wrapper);
    virtual void createSeries(VisualizerPtr visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
    virtual VisualizerPtr createVisualizer();
};

//! klasa pomocnicza przy tworzeniu wykresow z wieloma seriami
class MultiserieHelper : public TreeItemHelper
{
public:
    MultiserieHelper(const std::vector<core::ObjectWrapperPtr>& charts) :
      wrappers(charts)
      {}

public:
    virtual void createSeries(VisualizerPtr visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
    virtual VisualizerPtr createVisualizer();

private:
    std::vector<core::ObjectWrapperPtr> wrappers;
};

//! klasa pomocnicza przy tworzeniu wizualizatora jointow (leniwe parsowanie)
class JointsItemHelper : public TreeItemHelper
{
public:
    JointsItemHelper(MotionPtr motion);

public:
    virtual void createSeries(VisualizerPtr visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
    virtual VisualizerPtr createVisualizer();

private:
    MotionPtr motion;
};

//! klasa pomocnicza przy tworzeniu zbiorczego widoku 3d (markery + jointy + plyty GRF)
class Multiserie3D : public TreeItemHelper
{
public:
    Multiserie3D(MotionPtr motion);

public:
    virtual void createSeries(VisualizerPtr visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
    virtual VisualizerPtr createVisualizer();

private:
    MotionPtr motion;
};




#endif
