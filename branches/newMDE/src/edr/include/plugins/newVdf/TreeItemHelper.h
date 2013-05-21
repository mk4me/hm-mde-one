/********************************************************************
	created:	2011/09/29
	created:	29:9:2011   10:09
	filename: 	TreeItemHelper.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__TREEITEMHELPER_H__
#define HEADER_GUARD_NEWVDF__TREEITEMHELPER_H__

#include <QtCore/QString>
#include <QtGui/QTreeWidgetItem>
#include <plugins/subject/IMotion.h>
#include <plugins/newChart/INewChartSerie.h>
//#include "MultiserieColorStrategy.h"
#include <corelib/Visualizer.h>
#include <corelib/HierarchyHelper.h>
#include <plugins/newVdf/Export.h>

//! klasa pomocnicza przy tworzeniu wizualizatora wykresów
//! klasa musi dostarczyć ScalarChannelReaderInterface z danymi we wrapperze
class PLUGIN_NEWVDF_EXPORT NewChartItemHelper : public core::WrappedItemHelper
{
public:
    NewChartItemHelper(const core::ObjectWrapperConstPtr& wrapper);

public:
    //! Tworzy wizualizator newChart
    virtual core::VisualizerPtr createVisualizer(core::IVisualizerManager* manager);
    //! zwraca utworzone serie danych
    //! \param visualizer wizualizator, który będzie tworzył serie
    //! \param path ścieżka dla timeline
    //! \param series tutaj trafia stworzone serie 
    virtual void createSeries(const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::VisualizerSerie*>& series);
    //! \return typ ScalarChannelReaderInterface
    std::vector<core::TypeInfo> getTypeInfos() const;

};
typedef utils::shared_ptr<NewChartItemHelper> NewChartItemHelperPtr;
typedef utils::shared_ptr<const NewChartItemHelper> NewChartItemHelperConstPtr;

//! klasa pomocnicza przy tworzeniu wykresów z wektora 3-elementowego
class PLUGIN_NEWVDF_EXPORT NewVector3ItemHelper : public core::WrappedItemHelper
{
public:
    //! Konstruktor pobiera obiekt z wrapperem wektora 3D
    //! \param wrapper 
    NewVector3ItemHelper(const core::ObjectWrapperConstPtr& wrapper);

public:
    //! zwraca utworzone serie danych
    //! \param visualizer wizualizator, który będzie tworzył serie
    //! \param path ścieżka dla timeline
    //! \param series tutaj trafia stworzone serie
    virtual void createSeries(const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::VisualizerSerie*>& series);
    //! \return wizualizator 2D
    virtual core::VisualizerPtr createVisualizer(core::IVisualizerManager* manager);
    //! \return typ ScalarChannelReaderInterface
    std::vector<core::TypeInfo> getTypeInfos() const;
};
typedef utils::shared_ptr<NewVector3ItemHelper> NewVector3ItemHelperPtr;
typedef utils::shared_ptr<const NewVector3ItemHelper> NewVector3ItemHelperConstPtr;

////! pomaga tworzyć wizualizator 2D z wieloma seriami danych
//class NewMultiserieHelper : public TreeItemHelper
//{
//public:
//    //! Krotka : obiekt; związane z nim eventy; próba, z której pochodzi
//    struct ChartWithDescription 
//    {
//        //! obiekt, z danymi serii
//        core::ObjectWrapperConstPtr wrapper;
//        //! związane z obiektem zdarzenia
//        EventsCollectionConstPtr events;
//        //! próba, z której pochodzi obiekt z danymi
//        PluginSubject::MotionConstPtr motion;
//
//        //! konstruktor
//        //! \param w obiekt, z danymi serii
//        //! \param e związane z obiektem zdarzenia
//        //! \param m próba, z której pochodzi obiekt z danymi
//        ChartWithDescription(core::ObjectWrapperConstPtr w, EventsCollectionConstPtr e, PluginSubject::MotionConstPtr m) :
//            wrapper(w),
//            events(e),
//            motion(m)
//        {}
//    };
//    typedef std::vector<ChartWithDescription> ChartWithDescriptionCollection;
//
//public:
//    //! Konstruktor
//    //! \param charts kolekcja wrapperów, wraz z opisem (eventy + motion)
//    NewMultiserieHelper(const ChartWithDescriptionCollection& charts);
//    //! Koknstruktor
//    //! \param charts kolekcja wrapperów, ich opis wypełniany pustymi obiektami
//    NewMultiserieHelper(const std::vector<core::ObjectWrapperConstPtr>& charts);
//    
//public:
//    //! Ustawia strategie kolorów (np. random)
//    //! \param strategy 
//    void setColorStrategy(IMultiserieColorStrategyConstPtr strategy) { colorStrategy = strategy; }
//    //! zwraca utworzone serie danych
//    //! \param visualizer wizualizator, który będzie tworzył serie
//    //! \param path ścieżka dla timeline
//    //! \param series tutaj trafią stworzone serie 
//    virtual void createSeries(const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::VisualizerSerie*>& series);
//    //! \return wizualizator 2D
//    virtual core::VisualizerPtr createVisualizer();
//    //! \return tytuł wizualizatora 2D
//    const QString& getTitle() const { return title; }
//    //! ustawia tytuł
//    //! \param val 
//    void setTitle(const QString& val) { title = val; }
//    //! \return typ ScalarChannelReaderInterface
//    virtual std::vector<core::TypeInfo> getTypeInfos() const;
//    //! \return ustawiona kolekcja wrapperów wraz z opisami
//    const ChartWithDescriptionCollection& getChartWithDescriptions() const { return wrappers; }
//private:
//    //! ustawiona kolekcja wrapperów wraz z opisami
//    ChartWithDescriptionCollection wrappers;
//    //! tytuł w wizualizatorze 2D
//    QString title;
//    //! wykorzystana strategia do kolorowania serii
//    IMultiserieColorStrategyConstPtr colorStrategy;
//};
//typedef utils::shared_ptr<NewMultiserieHelper> NewMultiserieHelperPtr;
//typedef utils::shared_ptr<const NewMultiserieHelper> NewMultiserieHelperConstPtr;

//! klasa pomocnicza przy tworzeniu wizualizatora jointów (leniwe parsowanie)
class PLUGIN_NEWVDF_EXPORT JointsItemHelper : public core::HierarchyHelper
{
public:
    //! Konstruktor
    //! \param motion motion, z którego beda wyciagane elementy do stworzenia JointAnglesCollection
    JointsItemHelper(const PluginSubject::MotionConstPtr & motion);

public:
    //! zwraca utworzone serie danych
    //! \param visualizer wizualizator, który będzie tworzył serie
    //! \param path ścieżka dla timeline
    //! \param series tutaj trafia stworzone serie 
    virtual void createSeries(const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::VisualizerSerie*>& series);
    //! \return wizualizator 3D
    virtual core::VisualizerPtr createVisualizer(core::IVisualizerManager* manager);
    //! \return typ JointAnglesCollection
    std::vector<core::TypeInfo> getTypeInfos() const;

private:
    //! motion, z którego beda wyciagane elementy do stworzenia JointAnglesCollection
    PluginSubject::MotionConstPtr motion;
};
typedef utils::shared_ptr<JointsItemHelper> JointsItemHelperPtr;
typedef utils::shared_ptr<const JointsItemHelper> JointsItemHelperConstPtr;

//! klasa pomocnicza przy tworzeniu zbiorczego widoku 3d (markery + jointy + płyty GRF)
class PLUGIN_NEWVDF_EXPORT Multiserie3D : public core::HierarchyHelper
{
public:
    //! Konstruktor
    //! \param motion próba pomiarowa, z której beda wyciagane elementy 3D
    Multiserie3D(const PluginSubject::MotionConstPtr & motion);

public:
    //! zwraca utworzone serie danych
    //! \param visualizer wizualizator, który będzie tworzył serie
    //! \param path ścieżka dla timeline
    //! \param series tutaj trafia stworzone serie 
    virtual void createSeries(const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::VisualizerSerie*>& series);
    //! \return wizualizator 3D
    virtual core::VisualizerPtr createVisualizer(core::IVisualizerManager* manager);
    //! \return kolekcja ze wspieranymi przez wizualizator 3D typami
    std::vector<core::TypeInfo> getTypeInfos() const;

private:
    //! próba pomiarowa, z której beda wyciagane elementy 3D
    PluginSubject::MotionConstPtr motion;
};
typedef utils::shared_ptr<Multiserie3D> Multiserie3DPtr;
typedef utils::shared_ptr<const Multiserie3D> Multiserie3DConstPtr;

//
//! Klasa tworzy wizualizator wykresów, i wypełnią go przefiltrowanymi danymi EMG
//class PLUGIN_NEWVDF_EXPORT EMGFilterHelper : public NewChartItemHelper
//{
//public:
//    //! Konstruktor
//    //! \param wrapper 
//    EMGFilterHelper(const core::ObjectWrapperConstPtr& wrapper);
//    virtual ~EMGFilterHelper() {}
//
//public:
//    //! \return wizualizator stworzony dla dostarczonego wrappera
//    virtual core::VisualizerPtr createVisualizer();
//    //! tworzy serie ze zmodyfikowanym EMG na podstawiawie dostarczonego wrappera
//    //! \param visualizer wizualizator, który będzie tworzył serie
//    //! \param path ścieżka dla timeline'a
//    //! \param series zwracana seria 
//    virtual void createSeries(const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::VisualizerSerie*>& series);
//};
//typedef utils::shared_ptr<EMGFilterHelper> EMGFilterHelperPtr;
//typedef utils::shared_ptr<const EMGFilterHelper> EMGFilterHelperConstPtr;

#endif
