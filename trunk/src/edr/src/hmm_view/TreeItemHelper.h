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
#include <plugins/subject/Motion.h>
#include <plugins/newChart/INewChartSerie.h>
#include "MultiserieColorStrategy.h"
#include "Visualizer.h"

//! klasa podpinana pod elementy drzewa, pozwala tworzyć i zasilac wizualizatory
class TreeItemHelper
{
public:
    TreeItemHelper() {}
	virtual ~TreeItemHelper() {}

protected:
    //! Tworzy serie danych
    //! \param visualizer wizualizator, który będzie tworzył serie
    //! \param path ścieżka dla timeline
    //! \param series tutaj trafia stworzone serie
    virtual void createSeries(const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series) = 0;

public:
    //! \return utworzony obiekt wizualizatora
    virtual VisualizerPtr createVisualizer() = 0;
    //! \return czy obiekt zawiera dane
    virtual bool isDataItem() const { return true; }
    //! zwraca utworzone serie danych
    //! \param visualizer wizualizator, który będzie tworzył serie
    //! \param path ścieżka dla timeline
    //! \param series tutaj trafia stworzone serie
    void getSeries(const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
    //! \return zwraca próbę pomiarowa powiazana z danym helperem 
    PluginSubject::MotionConstPtr getMotion() const { return motion; }
    //! ustawia próbę pomiarowa powiazana z danym helperem
    //! \param val 
    void setMotion(PluginSubject::MotionConstPtr val) { motion = val; }
    //! \return tekst do GUI (np. summary window)
    QString getText() const { return text; }
    //! ustawia tekst GUI (np. summary window)
    //! \param val 
    void setText(QString val) { text = val; }
    //! \return typy wspierane przez helper
    virtual std::vector<core::TypeInfo> getTypeInfos() const = 0;
private:
    //! utworzony przez helper wizualizator
    VisualizerPtr createdVisualizer;
    //! próba pomiarowa powiazana z danym helperem
    PluginSubject::MotionConstPtr motion;
    //! tekst GUI (np. summary window)
    QString text;
};

typedef boost::shared_ptr<TreeItemHelper> TreeItemHelperPtr;
typedef boost::shared_ptr<const TreeItemHelper> TreeItemHelperConstPtr;


//! podstawowa klasa ułatwiająca tworzenie wizualizatora na podstawie elementu drzewa
class HmmTreeItem : public QTreeWidgetItem
{
public:
    //! Konstruktor wymaga podana helpera 
    //! \param helper elment odpowiedzialny za tworzenie wizualizatora i jego serii
    HmmTreeItem(TreeItemHelperPtr helper);
    virtual ~HmmTreeItem() {}

public:
    //! \return obiekt helpera
    TreeItemHelperPtr getHelper() { return helper; }
    //! ustawia tekst samego elementu drzewa jak i obiektu helpera 
    //! \param text ustawiany teskt
    void setItemAndHelperText(const QString& text);
private:
    //! elment odpowiedzialny za tworzenie wizualizatora i jego serii
    TreeItemHelperPtr helper;
};

//! Klasa, dzięki której można stworzyć wiecej wizualizatorow (na podstawie childów)
//! dodatkowo można wymusic ich specyficzny układ
class ChildrenVisualizers : public QTreeWidgetItem
{
public:
    //! układ tworzonych wizualizatorów
    enum PlacePolicy 
    {
        Horizontal,
        Vertical,
        Auto
    };

    //! Konstuktor, można ustawic układ wizualizatorów
    //! \param policy 
    ChildrenVisualizers(PlacePolicy policy = Auto);
    //! \return układ tworzonych wizualizatorów
    ChildrenVisualizers::PlacePolicy getPolicy() const { return policy; }
    //! Polityka podziału wizualizatorów dla potomnych elementów
    //! \param val polityka (podział pionowy, poziomy, ...)
    void setPolicy(ChildrenVisualizers::PlacePolicy val) { policy = val; }

private:
    //! układ tworzonych wizualizatorów
    PlacePolicy policy;
};

//! Najbardziej podstawowy helper dla drzewa danych
//! Tworzy domyślny wizualizator dla wrappera, bez dodatkowych operacji
class TreeWrappedItemHelper : public TreeItemHelper
{
public:
    //! Konstruktor wymaga podania wrappera z danymi
    //! \param wrapper 
    TreeWrappedItemHelper(const core::ObjectWrapperConstPtr & wrapper);
    virtual ~TreeWrappedItemHelper() {}

public:
    //! zwraca utworzone serie danych
    //! \param visualizer wizualizator, który będzie tworzył serie
    //! \param path ścieżka dla timeline
    //! \param series tutaj trafia stworzone serie 
    virtual void createSeries(const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
    //! \return tworzy wizualizator o typie zgodnym z wrapperem (gdy w systemie jest zarejstrowanych wiecej wizualizatorów danego typu to tworzony jest przpadkowy)
    virtual VisualizerPtr createVisualizer();
    //! \return typ wspierany przez helper (typ wpisany do wrappera)
    std::vector<core::TypeInfo> getTypeInfos() const;
    //! \return przypisany do helpera wrapper
    const core::ObjectWrapperConstPtr getWrapper() const { return wrapper; }
protected:
    //! przypisany do helpera wrapper
    core::ObjectWrapperConstPtr wrapper;
};
typedef core::shared_ptr<TreeWrappedItemHelper> TreeWrappedItemHelperPtr;
typedef core::shared_ptr<const TreeWrappedItemHelper> TreeWrappedItemHelperConstPtr;

//! klasa pomocnicza przy tworzeniu wizualizatora wykresów
//! klasa musi dostarczyć ScalarChannelReaderInterface z danymi we wrapperze
class NewChartItemHelper : public TreeWrappedItemHelper
{
public:
    NewChartItemHelper(const core::ObjectWrapperConstPtr& wrapper);

public:
    //! Tworzy wizualizator newChart
    virtual VisualizerPtr createVisualizer();
    //! zwraca utworzone serie danych
    //! \param visualizer wizualizator, który będzie tworzył serie
    //! \param path ścieżka dla timeline
    //! \param series tutaj trafia stworzone serie 
    virtual void createSeries(const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
    //! \return typ ScalarChannelReaderInterface
    std::vector<core::TypeInfo> getTypeInfos() const;

};
typedef core::shared_ptr<NewChartItemHelper> NewChartItemHelperPtr;
typedef core::shared_ptr<const NewChartItemHelper> NewChartItemHelperConstPtr;

//! klasa pomocnicza przy tworzeniu wykresów z wektora 3-elementowego
class NewVector3ItemHelper : public TreeWrappedItemHelper
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
    virtual void createSeries(const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
    //! \return wizualizator 2D
    virtual VisualizerPtr createVisualizer();
    //! \return typ ScalarChannelReaderInterface
    std::vector<core::TypeInfo> getTypeInfos() const;
};
typedef core::shared_ptr<NewVector3ItemHelper> NewVector3ItemHelperPtr;
typedef core::shared_ptr<const NewVector3ItemHelper> NewVector3ItemHelperConstPtr;

//! pomaga tworzyć wizualizator 2D z wieloma seriami danych
class NewMultiserieHelper : public TreeItemHelper
{
public:
    //! Krotka : obiekt; związane z nim eventy; próba, z której pochodzi
    struct ChartWithDescription 
    {
        //! obiekt, z danymi serii
        core::ObjectWrapperConstPtr wrapper;
        //! związane z obiektem zdarzenia
        EventsCollectionConstPtr events;
        //! próba, z której pochodzi obiekt z danymi
        PluginSubject::MotionConstPtr motion;

        //! konstruktor
        //! \param w obiekt, z danymi serii
        //! \param e związane z obiektem zdarzenia
        //! \param m próba, z której pochodzi obiekt z danymi
        ChartWithDescription(core::ObjectWrapperConstPtr w, EventsCollectionConstPtr e, PluginSubject::MotionConstPtr m) :
            wrapper(w),
            events(e),
            motion(m)
        {}
    };
    typedef std::vector<ChartWithDescription> ChartWithDescriptionCollection;

public:
    //! Konstruktor
    //! \param charts kolekcja wrapperów, wraz z opisem (eventy + motion)
    NewMultiserieHelper(const ChartWithDescriptionCollection& charts);
    //! Koknstruktor
    //! \param charts kolekcja wrapperów, ich opis wypełniany pustymi obiektami
    NewMultiserieHelper(const std::vector<core::ObjectWrapperConstPtr>& charts);
    
public:
    //! Ustawia strategie kolorów (np. random)
    //! \param strategy 
    void setColorStrategy(IMultiserieColorStrategyConstPtr strategy) { colorStrategy = strategy; }
    //! zwraca utworzone serie danych
    //! \param visualizer wizualizator, który będzie tworzył serie
    //! \param path ścieżka dla timeline
    //! \param series tutaj trafią stworzone serie 
    virtual void createSeries(const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
    //! \return wizualizator 2D
    virtual VisualizerPtr createVisualizer();
    //! \return tytuł wizualizatora 2D
    const QString& getTitle() const { return title; }
    //! ustawia tytuł
    //! \param val 
    void setTitle(const QString& val) { title = val; }
    //! \return typ ScalarChannelReaderInterface
    virtual std::vector<core::TypeInfo> getTypeInfos() const;
    //! \return ustawiona kolekcja wrapperów wraz z opisami
    const ChartWithDescriptionCollection& getChartWithDescriptions() const { return wrappers; }
private:
    //! ustawiona kolekcja wrapperów wraz z opisami
    ChartWithDescriptionCollection wrappers;
    //! tytuł w wizualizatorze 2D
    QString title;
    //! wykorzystana strategia do kolorowania serii
    IMultiserieColorStrategyConstPtr colorStrategy;
};
typedef core::shared_ptr<NewMultiserieHelper> NewMultiserieHelperPtr;
typedef core::shared_ptr<const NewMultiserieHelper> NewMultiserieHelperConstPtr;

//! klasa pomocnicza przy tworzeniu wizualizatora jointów (leniwe parsowanie)
class JointsItemHelper : public TreeItemHelper
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
    virtual void createSeries(const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
    //! \return wizualizator 3D
    virtual VisualizerPtr createVisualizer();
    //! \return typ JointAnglesCollection
    std::vector<core::TypeInfo> getTypeInfos() const;

private:
    //! motion, z którego beda wyciagane elementy do stworzenia JointAnglesCollection
    PluginSubject::MotionConstPtr motion;
};
typedef core::shared_ptr<JointsItemHelper> JointsItemHelperPtr;
typedef core::shared_ptr<const JointsItemHelper> JointsItemHelperConstPtr;

//! klasa pomocnicza przy tworzeniu zbiorczego widoku 3d (markery + jointy + płyty GRF)
class Multiserie3D : public TreeItemHelper
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
    virtual void createSeries(const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
    //! \return wizualizator 3D
    virtual VisualizerPtr createVisualizer();
    //! \return kolekcja ze wspieranymi przez wizualizator 3D typami
    std::vector<core::TypeInfo> getTypeInfos() const;

private:
    //! próba pomiarowa, z której beda wyciagane elementy 3D
    PluginSubject::MotionConstPtr motion;
};
typedef core::shared_ptr<Multiserie3D> Multiserie3DPtr;
typedef core::shared_ptr<const Multiserie3D> Multiserie3DConstPtr;

#endif
