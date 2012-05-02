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
#include <QtGui/QTreeWidgetItem>
//#include <plugins/chart/ChartVisualizer.h>
#include <plugins/subject/Motion.h>
#include <plugins/newChart/INewChartSerie.h>
#include "Visualizer.h"

//! podstawowa klasa ulatwiajaca tworzenie wizualizatora na podstawie elementu drzewa
class TreeItemHelper : public QTreeWidgetItem
{
public:
    TreeItemHelper() {}
	virtual ~TreeItemHelper() {}

protected:
    virtual void createSeries(const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series) = 0;

public:
    virtual VisualizerPtr createVisualizer() = 0;
    virtual bool isDataItem() const { return true; }

    void getSeries(const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
    PluginSubject::MotionConstPtr getMotion() const { return motion; }
    void setMotion(PluginSubject::MotionConstPtr val) { motion = val; }

    virtual std::vector<core::TypeInfo> getTypeInfos() const = 0;

private:
     VisualizerPtr createdVisualizer;
     PluginSubject::MotionConstPtr motion;
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
    TreeWrappedItemHelper(const core::ObjectWrapperConstPtr & wrapper) : wrapper(wrapper)
    {
    }

public:
    virtual void createSeries(const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
    virtual VisualizerPtr createVisualizer();

    std::vector<core::TypeInfo> getTypeInfos() const
    {
        std::vector<core::TypeInfo> ret;
        ret.push_back(wrapper->getTypeInfo());
        return ret;
    }

    const core::ObjectWrapperConstPtr getWrapper() const { return wrapper; }


protected:
    core::ObjectWrapperConstPtr wrapper;
};

////! klasa pomocnicza przy tworzeniu wizualizatora wykresow
//class ChartItemHelper : public TreeWrappedItemHelper
//{
//public:
//    ChartItemHelper(const core::ObjectWrapperConstPtr& wrapper) : TreeWrappedItemHelper(wrapper) {}
//    virtual VisualizerPtr createVisualizer();
//    virtual void createSeries(const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
//
//private:
//    void setUpChart(ChartVisualizer* chart, const std::string& title);
//
//private:
//    osg::ref_ptr<osgText::Text> chartTextPrototype;
//};

//! klasa pomocnicza przy tworzeniu wizualizatora wykresow
class NewChartItemHelper : public TreeWrappedItemHelper
{
public:
    NewChartItemHelper(const core::ObjectWrapperConstPtr& wrapper) : TreeWrappedItemHelper(wrapper) { }
    virtual VisualizerPtr createVisualizer();
    virtual void createSeries(const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
    std::vector<core::TypeInfo> getTypeInfos() const
    {
        std::vector<core::TypeInfo> ret;
        ret.push_back(typeid(ScalarChannelReaderInterface));
        return ret;
    }

};

////! klasa pomocnicza przy tworzeniu wykresow z wektora 3-elementowego
//class Vector3ItemHelper : public TreeWrappedItemHelper
//{
//public:
//    Vector3ItemHelper(const core::ObjectWrapperConstPtr& wrapper) : TreeWrappedItemHelper(wrapper) {}
//    virtual void createSeries(const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
//    virtual VisualizerPtr createVisualizer();
//};

//! klasa pomocnicza przy tworzeniu wykresow z wektora 3-elementowego
class NewVector3ItemHelper : public TreeWrappedItemHelper
{
public:
    NewVector3ItemHelper(const core::ObjectWrapperConstPtr& wrapper) : TreeWrappedItemHelper(wrapper) {}
    virtual void createSeries(const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
    virtual VisualizerPtr createVisualizer();

    std::vector<core::TypeInfo> getTypeInfos() const
    {
        std::vector<core::TypeInfo> ret;
        ret.push_back(typeid(ScalarChannelReaderInterface));
        return ret;
    }
};


class IMultiserieColorStrategy
{
public:
    virtual ~IMultiserieColorStrategy() {}
    virtual QColor getColor(INewChartSerie*, core::ObjectWrapperConstPtr) const = 0;
};
typedef core::shared_ptr<IMultiserieColorStrategy> IMultiserieColorStrategyPtr;
typedef core::shared_ptr<const IMultiserieColorStrategy> IMultiserieColorStrategyConstPtr;

class RandomMultiserieColorStrategy : public IMultiserieColorStrategy
{
public:
    QColor getColor(INewChartSerie* s, core::ObjectWrapperConstPtr w) const
    {
        return QColor(rand() % 256, rand() % 256, rand() % 256);
    }
};

class CopyColorMultiserieColorStrategy : public IMultiserieColorStrategy
{
public:
    QColor getColor(INewChartSerie* s, core::ObjectWrapperConstPtr w) const
    {
        return s->getColor();
    }
};

class RandomBetweenMultiserieColorStrategy : public IMultiserieColorStrategy
{
public:
    RandomBetweenMultiserieColorStrategy(QColor c1, QColor c2) : c1(c1), c2(c2) {}

    QColor getColor(INewChartSerie* s, core::ObjectWrapperConstPtr w) const
    {
        float r = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        return QColor(
            lerp(c1.red(), c2.red(), r),
            lerp(c1.green(), c2.green(), r),
            lerp(c1.blue(), c2.blue(), r)
            );
    }

private:
    int lerp(int from, int to, float r) const
    {
        return static_cast<int>(from * (1.0f - r) + to * r);
    }
    QColor c1, c2;
};

class ColorMapMultiserieStrategy : public IMultiserieColorStrategy
{
public:
    ColorMapMultiserieStrategy(const std::map<core::ObjectWrapperConstPtr, QColor>& colorMap) : colorMap(colorMap) {}
    virtual QColor getColor(INewChartSerie* s, core::ObjectWrapperConstPtr w) const 
    {
        auto it = colorMap.find(w);
        if (it != colorMap.end()) {
            return it->second;
        }

        return QColor();
    }

private:
    std::map<core::ObjectWrapperConstPtr, QColor> colorMap;
};


class NewMultiserieHelper : public TreeItemHelper
{
public:
    typedef std::pair<core::ObjectWrapperConstPtr, EventsCollectionConstPtr> ChartWithEvents;
    typedef std::vector<ChartWithEvents> ChartWithEventsCollection;

public:
    NewMultiserieHelper(const ChartWithEventsCollection& charts): 
      colorStrategy(new RandomMultiserieColorStrategy()), title(""), wrappers(charts)
    {
        
    }

    NewMultiserieHelper(const std::vector<core::ObjectWrapperConstPtr>& charts): 
    colorStrategy(new RandomMultiserieColorStrategy()), title("")
    {
        for (auto it = charts.begin(); it != charts.end(); it++) {
            wrappers.push_back(std::make_pair(*it, EventsCollectionConstPtr()));
        }
    }

    void setColorStrategy(IMultiserieColorStrategyConstPtr strategy) { colorStrategy = strategy; }

public:
    virtual void createSeries(const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
    virtual VisualizerPtr createVisualizer();
    const QString& getTitle() const { return title; }
    void setTitle(const QString& val) { title = val; }

    virtual std::vector<core::TypeInfo> getTypeInfos() const;

private:
    ChartWithEventsCollection wrappers;
    QString title;
    
    IMultiserieColorStrategyConstPtr colorStrategy;
};

//! klasa pomocnicza przy tworzeniu wizualizatora jointow (leniwe parsowanie)
class JointsItemHelper : public TreeItemHelper
{
public:
    JointsItemHelper(const PluginSubject::MotionConstPtr & motion) : motion(motion)
    {
        
    }

public:
    virtual void createSeries(const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
    virtual VisualizerPtr createVisualizer();

    std::vector<core::TypeInfo> getTypeInfos() const
    {
        std::vector<core::TypeInfo> ret;
        ret.push_back(typeid(kinematic::JointAnglesCollection));
        return ret;
    }


private:
    PluginSubject::MotionConstPtr motion;
};

//! klasa pomocnicza przy tworzeniu zbiorczego widoku 3d (markery + jointy + plyty GRF)
class Multiserie3D : public TreeItemHelper
{
public:
    Multiserie3D(const PluginSubject::MotionConstPtr motion) : motion(motion)
    {

    }

public:
    virtual void createSeries(const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
    virtual VisualizerPtr createVisualizer();

    std::vector<core::TypeInfo> getTypeInfos() const
    {
        std::vector<core::TypeInfo> ret;
        ret.push_back(typeid(kinematic::JointAnglesCollection));
        ret.push_back(typeid(GRFCollection));
        ret.push_back(typeid(MarkerCollection));
        return ret;
    }


private:
    PluginSubject::MotionConstPtr motion;
};

#endif
