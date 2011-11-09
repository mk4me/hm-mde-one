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

//class HmmTreeItem : public QTreeWidgetItem
//{
//public:
//    virtual TreeItemHelper* getHelper() = 0;
//};

//template <class HelperPolicy>
//class HmmTreePolicyItem : public HmmTreeItem, public HelperPolicy
//{
//public:
//    template<class Arg>
//    HmmTreePolicyItem(const Arg& arg) 
//    {
//        HelperPolicy::init(arg);
//    }
//    virtual TreeItemHelper* getHelper()
//    {
//        return dynamic_cast<TreeItemHelper*>(this);
//    }
//};


//! podstawowa klasa ulatwiajaca tworzenie wizualizatora na podstawie elementu drzewa
class TreeItemHelper : public QTreeWidgetItem
{
public:
    //TreeItemHelper(const core::ObjectWrapperPtr& wrapper);
    TreeItemHelper() {}
	virtual ~TreeItemHelper() {}
/*protected:
    TreeItemHelper() {}*/

public:
    virtual void createSeries(VisualizerPtr visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series) = 0;
    virtual VisualizerPtr createVisualizer() = 0;
    virtual bool isDataItem() const { return true; }

protected:
    static void setUpChart(ChartVisualizer* chart, const std::string& title);
    //const core::ObjectWrapperPtr wrapper;

private:
    static osg::ref_ptr<osgText::Text> chartTextPrototype;
    VisualizerPtr createdVisualizer;
};
typedef boost::shared_ptr<TreeItemHelper> TreeItemHelperPtr;
typedef boost::shared_ptr<const TreeItemHelper> TreeItemHelperConstPtr;

class ChildrenVisualizers : public QTreeWidgetItem
{
public:
    enum PlacePolicy 
    {
        Horizontal,
        Vertical,
        Auto
    };

    ChildrenVisualizers(PlacePolicy policy = Auto, bool close = false) :
      policy(policy),
      close(close) 
      {

      }

    ChildrenVisualizers::PlacePolicy getPolicy() const { return policy; }
    void setPolicy(ChildrenVisualizers::PlacePolicy val) { policy = val; }
    bool getClose() const { return close; }
    void setClose(bool val) { close = val; }

private:
    PlacePolicy policy;
    
    bool close;
    
};

class TreeWrappedItemHelper : public TreeItemHelper
{
public:
    TreeWrappedItemHelper(core::ObjectWrapperPtr wrapper) : wrapper(wrapper)
    {
    }

public:
    virtual void createSeries(VisualizerPtr visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
    virtual VisualizerPtr createVisualizer();
protected:
    core::ObjectWrapperPtr wrapper;
};

//! klasa pomocnicza przy tworzeniu wizualizatora wykresow
class ChartItemHelper : public TreeWrappedItemHelper
{
public:
    ChartItemHelper(const core::ObjectWrapperPtr& wrapper) : TreeWrappedItemHelper(wrapper) {}
    virtual VisualizerPtr createVisualizer();
    virtual void createSeries(VisualizerPtr visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
};

//! klasa pomocnicza przy tworzeniu wykresow z wektora 3-elementowego
class Vector3ItemHelper : public TreeWrappedItemHelper
{
public:
    Vector3ItemHelper(const core::ObjectWrapperPtr& wrapper) : TreeWrappedItemHelper(wrapper) {}
    virtual void createSeries(VisualizerPtr visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
    virtual VisualizerPtr createVisualizer();
};

//! klasa pomocnicza przy tworzeniu wykresow z wieloma seriami
class MultiserieHelper : public TreeItemHelper
{
public:
    enum ColorPolicy {
        Random,
        GreenRandom,
        RedRandom,
        Red,
        Green,
        HalfRedHalfGreen
    };
    
public:
    MultiserieHelper(const std::vector<core::ObjectWrapperPtr>& charts): colorPolicy(Random) 
    {
      wrappers = charts;
    }

    void setColorPolicy(ColorPolicy policy) { colorPolicy = policy; }

public:
    virtual void createSeries(VisualizerPtr visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
    virtual VisualizerPtr createVisualizer();

private:
    std::vector<core::ObjectWrapperPtr> wrappers;
    ColorPolicy colorPolicy;
};

//! klasa pomocnicza przy tworzeniu wizualizatora jointow (leniwe parsowanie)
class JointsItemHelper : public TreeItemHelper
{
public:
    JointsItemHelper(MotionPtr motion)
    {
        this->motion = motion;
    }

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
    Multiserie3D(MotionPtr motion)
    {
        this->motion = motion;
    }

public:
    virtual void createSeries(VisualizerPtr visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
    virtual VisualizerPtr createVisualizer();

private:
    MotionPtr motion;
};






#endif
