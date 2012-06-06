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

//! klasa podpinana pod elementy drzewa, pozwala tworzyc i zasilac wizualizatory
class TreeItemHelper
{
public:
    TreeItemHelper() {}
	virtual ~TreeItemHelper() {}

protected:
    //! Tworzy serie danych
    //! \param visualizer wizualizator, ktory bedzie tworzyl serie
    //! \param path sciezka dla timeline
    //! \param series tutaj trafia stworzone serie
    virtual void createSeries(const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series) = 0;

public:
    //! \return utworzony obiekt wizualizatora
    virtual VisualizerPtr createVisualizer() = 0;
    //! \return czy obiekt zawiera dane
    virtual bool isDataItem() const { return true; }
    //! zwraca utworzone serie danych
    //! \param visualizer wizualizator, ktory bedzie tworzyl serie
    //! \param path sciezka dla timelin
    //! \param series tutaj trafia stworzone serie
    void getSeries(const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
    //! \return zwraca probe pomiarowa powiazana z danym helperem 
    PluginSubject::MotionConstPtr getMotion() const { return motion; }
    //! ustawia probe pomiarowa powiazana z danym helperem
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
    //! proba pomiarowa powiazana z danym helperem
    PluginSubject::MotionConstPtr motion;
    //! tekst GUI (np. summary window)
    QString text;
};

typedef boost::shared_ptr<TreeItemHelper> TreeItemHelperPtr;
typedef boost::shared_ptr<const TreeItemHelper> TreeItemHelperConstPtr;


//! podstawowa klasa ulatwiajaca tworzenie wizualizatora na podstawie elementu drzewa
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

//! Klasa, dzieki ktorej mozna stworzyc wiecej wizualizatorow (na podstawie childow)
//! dodatkowo mozna wymusic ich specyficzny uklad
class ChildrenVisualizers : public QTreeWidgetItem
{
public:
    //! uklad tworzonych wizualizatorow
    enum PlacePolicy 
    {
        Horizontal,
        Vertical,
        Auto
    };

    //! Konstuktor, mozna ustawic uklad wizualizatorow
    //! \param policy 
    ChildrenVisualizers(PlacePolicy policy = Auto);
    //! \return uklad tworzonych wizualizatorow
    ChildrenVisualizers::PlacePolicy getPolicy() const { return policy; }
    //! 
    //! \param val 
    void setPolicy(ChildrenVisualizers::PlacePolicy val) { policy = val; }

private:
    //! uklad tworzonych wizualizatorow
    PlacePolicy policy;
};

//! Najbardziej podstawowy helper dla drzewa danych
//! Tworzy domyslny wizualizator dla wrappera, bez dodatkowych operacji
class TreeWrappedItemHelper : public TreeItemHelper
{
public:
    //! Konstruktor wymaga podania wrappera z danymi
    //! \param wrapper 
    TreeWrappedItemHelper(const core::ObjectWrapperConstPtr & wrapper);
    virtual ~TreeWrappedItemHelper() {}

public:
    //! zwraca utworzone serie danych
    //! \param visualizer wizualizator, ktory bedzie tworzyl serie
    //! \param path sciezka dla timelin
    //! \param series tutaj trafia stworzone serie 
    virtual void createSeries(const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
    //! \return tworzy wizualizator o typie zgodnym z wrapperem (gdy w systemie jest zarejstrowanych wiecej wizualizatorow danego typu to tworzony jest przpadkowy)
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

//! klasa pomocnicza przy tworzeniu wizualizatora wykresow
//! klasa musi dostarczyc ScalarChannelReaderInterface z danymi we wrapperze
class NewChartItemHelper : public TreeWrappedItemHelper
{
public:
    NewChartItemHelper(const core::ObjectWrapperConstPtr& wrapper);

public:
    //! Tworzy wizualizator newChart
    virtual VisualizerPtr createVisualizer();
    //! zwraca utworzone serie danych
    //! \param visualizer wizualizator, ktory bedzie tworzyl serie
    //! \param path sciezka dla timelin
    //! \param series tutaj trafia stworzone serie 
    virtual void createSeries(const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
    //! \return typ ScalarChannelReaderInterface
    std::vector<core::TypeInfo> getTypeInfos() const;

};
typedef core::shared_ptr<NewChartItemHelper> NewChartItemHelperPtr;
typedef core::shared_ptr<const NewChartItemHelper> NewChartItemHelperConstPtr;

//! klasa pomocnicza przy tworzeniu wykresow z wektora 3-elementowego
class NewVector3ItemHelper : public TreeWrappedItemHelper
{
public:
    //! Konstruktor pobiera obiekt z wrapperem wektora 3D
    //! \param wrapper 
    NewVector3ItemHelper(const core::ObjectWrapperConstPtr& wrapper);

public:
    //! zwraca utworzone serie danych
    //! \param visualizer wizualizator, ktory bedzie tworzyl serie
    //! \param path sciezka dla timelin
    //! \param series tutaj trafia stworzone serie
    virtual void createSeries(const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
    //! \return wizualizator 2D
    virtual VisualizerPtr createVisualizer();
    //! \return typ ScalarChannelReaderInterface
    std::vector<core::TypeInfo> getTypeInfos() const;
};
typedef core::shared_ptr<NewVector3ItemHelper> NewVector3ItemHelperPtr;
typedef core::shared_ptr<const NewVector3ItemHelper> NewVector3ItemHelperConstPtr;

//! pomaga tworzyc wizualizator 2D z wieloma seriami danych
class NewMultiserieHelper : public TreeItemHelper
{
public:
    //! Krotka : obiekt; zwiazane z nim eventy; proba, z ktorej pochodzi
    struct ChartWithDescription 
    {
        core::ObjectWrapperConstPtr wrapper;
        EventsCollectionConstPtr events;
        PluginSubject::MotionConstPtr motion;

        ChartWithDescription(core::ObjectWrapperConstPtr w, EventsCollectionConstPtr e, PluginSubject::MotionConstPtr m) :
            wrapper(w),
            events(e),
            motion(m)
        {}
    };
    typedef std::vector<ChartWithDescription> ChartWithDescriptionCollection;

public:
    //! Konstruktor
    //! \param charts kolekcja wrapperow, wraz z opisem (eventy + motion)
    NewMultiserieHelper(const ChartWithDescriptionCollection& charts);
    //! Koknstruktor
    //! \param charts kolekcja wrapperow, ich opis wypelniany pustymi obiektami
    NewMultiserieHelper(const std::vector<core::ObjectWrapperConstPtr>& charts);
    
public:
    //! Ustawia strategie kolorow (np. random)
    //! \param strategy 
    void setColorStrategy(IMultiserieColorStrategyConstPtr strategy) { colorStrategy = strategy; }
    //! zwraca utworzone serie danych
    //! \param visualizer wizualizator, ktory bedzie tworzyl serie
    //! \param path sciezka dla timelin
    //! \param series tutaj trafia stworzone serie 
    virtual void createSeries(const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
    //! \return wizualizator 2D
    virtual VisualizerPtr createVisualizer();
    //! \return tytul wizualizatora 2D
    const QString& getTitle() const { return title; }
    //! ustawia tytul
    //! \param val 
    void setTitle(const QString& val) { title = val; }
    //! \return typ ScalarChannelReaderInterface
    virtual std::vector<core::TypeInfo> getTypeInfos() const;
    //! \return ustawiona kolekcja wrapperow wraz z opisami
    const ChartWithDescriptionCollection& getChartWithDescriptions() const { return wrappers; }
private:
    //! ustawiona kolekcja wrapperow wraz z opisami
    ChartWithDescriptionCollection wrappers;
    //! tytul w wizualizatorze 2D
    QString title;
    //! wykorzystana strategia do kolorowania serii
    IMultiserieColorStrategyConstPtr colorStrategy;
};
typedef core::shared_ptr<NewMultiserieHelper> NewMultiserieHelperPtr;
typedef core::shared_ptr<const NewMultiserieHelper> NewMultiserieHelperConstPtr;

//! klasa pomocnicza przy tworzeniu wizualizatora jointow (leniwe parsowanie)
class JointsItemHelper : public TreeItemHelper
{
public:
    //! Konstruktor
    //! \param motion motion, z ktorego beda wyciagane elementy do stworzenia JointAnglesCollection
    JointsItemHelper(const PluginSubject::MotionConstPtr & motion);

public:
    //! zwraca utworzone serie danych
    //! \param visualizer wizualizator, ktory bedzie tworzyl serie
    //! \param path sciezka dla timelin
    //! \param series tutaj trafia stworzone serie 
    virtual void createSeries(const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
    //! \return wizualizator 3D
    virtual VisualizerPtr createVisualizer();
    //! \return typ JointAnglesCollection
    std::vector<core::TypeInfo> getTypeInfos() const;

private:
    //! motion, z ktorego beda wyciagane elementy do stworzenia JointAnglesCollection
    PluginSubject::MotionConstPtr motion;
};
typedef core::shared_ptr<JointsItemHelper> JointsItemHelperPtr;
typedef core::shared_ptr<const JointsItemHelper> JointsItemHelperConstPtr;

//! klasa pomocnicza przy tworzeniu zbiorczego widoku 3d (markery + jointy + plyty GRF)
class Multiserie3D : public TreeItemHelper
{
public:
    //! Konstruktor
    //! \param motion proba pomiarowa, z ktorej beda wyciagane elementy 3D
    Multiserie3D(const PluginSubject::MotionConstPtr & motion);

public:
    //! zwraca utworzone serie danych
    //! \param visualizer wizualizator, ktory bedzie tworzyl serie
    //! \param path sciezka dla timelin
    //! \param series tutaj trafia stworzone serie 
    virtual void createSeries(const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
    //! \return wizualizator 3D
    virtual VisualizerPtr createVisualizer();
    //! \return kolekcja ze wspieranymi przez wizualizator 3D typami
    std::vector<core::TypeInfo> getTypeInfos() const;

private:
    //! proba pomiarowa, z ktorej beda wyciagane elementy 3D
    PluginSubject::MotionConstPtr motion;
};
typedef core::shared_ptr<Multiserie3D> Multiserie3DPtr;
typedef core::shared_ptr<const Multiserie3D> Multiserie3DConstPtr;

#endif
