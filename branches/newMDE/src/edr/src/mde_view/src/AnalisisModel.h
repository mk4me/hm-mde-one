/********************************************************************
	created:	2013/04/18
	created:	18:4:2013   18:38
	filename: 	AnalisisModel.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MDE_VIEW__ANALISISMODEL_H__
#define HEADER_GUARD_MDE_VIEW__ANALISISMODEL_H__

#include <QtCore/QObject>
#include <coreui/HierarchyTreeModel.h>
#include <corelib/IDataManagerReader.h>
#include <corelib/IFilterProvider.h>
#include <corelib/IFilterBundle.h>
#include "ContextEventFilter.h"
#include <coreui/CoreVisualizerWidget.h>
#include <plugins/newTimeline/VisualizerSerieTimelineChannel.h>

class QDockWidget;

class AnalisisModel : public QObject, public core::IDataManagerReader::IObjectObserver, public core::Visualizer::IVisualizerObserver
{
    Q_OBJECT
public:
    //! s�u�y do przechowywania informacji o utworzonych wizualizatorach i ich seriach, u�ywane w mapie itemHelper -> itemDesc
    struct DataItemDescription 
    {
        DataItemDescription() {}

        //! Konstruktor wype�nia wszystkie pola struktury 
        //! \param visualizer 
        //! \param series 
        //! \param widget 
        DataItemDescription(coreUI::CoreVisualizerWidget* widget, QDockWidget * dockWidget);
        //! widget, w kt�ry reprezentuje wizualizator
        coreUI::CoreVisualizerWidget* visualizerWidget;

        QDockWidget * visualizerDockWidget;

        utils::shared_ptr<VisualizerSerieTimelineMultiChannel> channel;

        std::string path;
    };
    DEFINE_SMART_POINTERS(DataItemDescription)

public:
    AnalisisModel();
	virtual ~AnalisisModel() {}

public:
    virtual void observe(const core::IDataManagerReader::ChangeList & changes);
    coreui::HierarchyTreeModel* getTreeModel() { return &model; }
    void addFilterBundles( const core::IFilterProvider::FilterBundles& bundles );
    void applyFilter( core::IFilterCommandPtr filter );
    void addVisualizerDataDescription( core::HierarchyHelperPtr helper, AnalisisModel::DataItemDescriptionPtr desc );
    std::list<DataItemDescriptionConstPtr> getVisualizerDataDescriptions( const core::HierarchyHelperPtr& helper );
    DataItemDescriptionConstPtr getVisualizerDataDescription(const core::VisualizerPtr& visualizer);
    core::HierarchyHelperPtr getHelper(const DataItemDescriptionConstPtr& desc);

    virtual void update(core::Visualizer::VisualizerSerie * serie, core::Visualizer::SerieModyfication modyfication );
    void addSeriesToVisualizer(core::VisualizerPtr visualizer, core::HierarchyHelperPtr helper, QString &path, QDockWidget * visualizerDockWidget );

Q_SIGNALS:
    void filterBundleAdded(core::IFilterBundlePtr);
    void reportCreated(const QString& html);
    void expandTree(int);

private:
    // TODO : przyda sie madrzejszy sposob zarzadzania elementami przefiltrowanymi i nieprzefiltrowanymi 
    void saveSourceItems();
    void loadSourceItems();

private:
    coreui::HierarchyTreeModel model;
    core::IHierarchyItemPtr dataManagerTreeItem; 
    core::IFilterProvider::FilterBundles filters;
    std::vector<core::IHierarchyItemConstPtr> itms;
    //! struktura z informacjami o stworzonych wizualizatorach, ich seriach oraz z kt�rego elementu powsta�y
    std::multimap<core::HierarchyHelperWeakPtr, DataItemDescriptionPtr> items2Descriptions;
    std::map<core::HierarchyHelperWeakPtr, core::IHierarchyDataItemConstWeakPtr> helper2hierarchyItem;

    std::map<core::Visualizer::VisualizerSerie*, std::string> seriesToChannels;
};
DEFINE_SMART_POINTERS(AnalisisModel);

#endif
