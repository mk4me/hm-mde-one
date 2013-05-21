/********************************************************************
	created:	2013/05/07
	created:	7:5:2013   13:19
	filename: 	HierarchyHelper.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__HIERARCHYHELPER_H__
#define HEADER_GUARD_CORE__HIERARCHYHELPER_H__

#include <QtCore/QString>
#include <corelib/Visualizer.h>

namespace core {

//! klasa podpinana pod elementy drzewa, pozwala tworzyć i zasilac wizualizatory
class CORELIB_EXPORT HierarchyHelper
{
public:
    HierarchyHelper();
	virtual ~HierarchyHelper() {}

protected:
    //! Tworzy serie danych
    //! \param visualizer wizualizator, który będzie tworzył serie
    //! \param path ścieżka dla timeline
    //! \param series tutaj trafia stworzone serie
    virtual void createSeries(const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::VisualizerSerie*>& series) = 0;

public:
    //! \return utworzony obiekt wizualizatora
    virtual core::VisualizerPtr createVisualizer(core::IVisualizerManager* manager) = 0;
    //! zwraca utworzone serie danych
    //! \param visualizer wizualizator, który będzie tworzył serie
    //! \param path ścieżka dla timeline
    //! \param series tutaj trafia stworzone serie
    void getSeries(const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::VisualizerSerie*>& series);
    //! \return tekst do GUI (np. summary window)
    QString getText() const { return text; }
    //! ustawia tekst GUI (np. summary window)
    //! \param val 
    void setText(QString val) { text = val; }
    //! \return typy wspierane przez helper
    virtual std::vector<core::TypeInfo> getTypeInfos() const = 0;
private:
    //! utworzony przez helper wizualizator
    core::VisualizerPtr createdVisualizer;
    //! tekst GUI (np. summary window)
    QString text;
};
DEFINE_SMART_POINTERS(HierarchyHelper);


//! Najbardziej podstawowy helper dla drzewa danych
//! Tworzy domyślny wizualizator dla wrappera, bez dodatkowych operacji
class CORELIB_EXPORT WrappedItemHelper : public HierarchyHelper
{
public:
    //! Konstruktor wymaga podania wrappera z danymi
    //! \param wrapper 
    WrappedItemHelper(const core::ObjectWrapperConstPtr & wrapper);
    virtual ~WrappedItemHelper() {}

public:
    //! zwraca utworzone serie danych
    //! \param visualizer wizualizator, który będzie tworzył serie
    //! \param path ścieżka dla timeline
    //! \param series tutaj trafia stworzone serie 
    virtual void createSeries(const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::VisualizerSerie*>& series);
    //! \return tworzy wizualizator o typie zgodnym z wrapperem (gdy w systemie jest zarejstrowanych wiecej wizualizatorów danego typu to tworzony jest przpadkowy)
    virtual core::VisualizerPtr createVisualizer(core::IVisualizerManager* manager);
    //! \return typ wspierany przez helper (typ wpisany do wrappera)
    std::vector<core::TypeInfo> getTypeInfos() const;
    //! \return przypisany do helpera wrapper
    const core::ObjectWrapperConstPtr getWrapper() const { return wrapper; }
protected:
    //! przypisany do helpera wrapper
    core::ObjectWrapperConstPtr wrapper;
};
DEFINE_SMART_POINTERS(WrappedItemHelper);

}

#endif
