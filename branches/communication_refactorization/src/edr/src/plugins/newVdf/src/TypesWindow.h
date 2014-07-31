/********************************************************************
	created:	2012/12/17
	created:	17:12:2012   8:56
	filename: 	TypesWindow.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__TYPESWINDOW_H__
#define HEADER_GUARD_NEWVDF__TYPESWINDOW_H__


#include <QtWidgets/QWidget>
#include <utils/ObserverPattern.h>
#include <utils/CommandStack.h>
#include <plugins/newVdf/IDataProcessorManager.h>
#include <plugins/newVdf/IDataSinkManager.h>
#include <plugins/newVdf/IDataSourceManager.h>

#include "Command.h"
#include "CanvasStyleEditor.h"
#include "SceneModel.h"

namespace Ui { class TypesWindow; } 

namespace vdf {

class NewVdfWidget;


//! Widget z elementami do wstawienia na scenê
class TypesWindow : public QWidget, 
    public utils::Observer<vdf::IDataProcessorManager>, public utils::Observer<vdf::IDataSinkManager>, public utils::Observer<vdf::IDataSourceManager>
{
    Q_OBJECT;
public:
    //! Konstruktor
    //! \param stack Stos komend (umo¿liwia m.in. undo)
    //! \param canvas Model ze stylami elementów
    //! \param sceneModel Scena, na któr¹ trafi¹ elementy 
    //! \param parent 
    //! \param f 
    explicit TypesWindow(utils::ICommandStackPtr stack, CanvasStyleEditorPtr canvas, SceneModelPtr sceneModel, QWidget* parent = 0, Qt::WindowFlags f = 0);
	virtual ~TypesWindow() {}

public:
    // TODO : zmienic idedtyfikacje z name na uid

    //! Tworzy element na podstawie nazwy i umieszcza go na scenie, komenda trafia na stos wywo³añ.
    //! \param name nazwa elementu, który zostanie utworzony
    //! \param scenePos pozycja na scenie, gdzie trafi obiekt
    void insert(const QString& name, const QPointF& scenePos);

    //! Tworzy element, który jest gotowy aby go wstawiæ na scenê
    //! \param entry nazwa/identyfikator elementu
    SceneBuilder::VisualNodeWithPins createItemByEntry(const QString& entry);

    core::UniqueID getId(const QString& name) const;
        
protected:
    //! Wywo³ywane, gdy pojawi¹ siê nowe elementy przetwarzaj¹ce
    //! \param pm 
    void update(const IDataProcessorManager* pm);
    //! Wywo³ywane, gdy pojawi¹ siê nowe Ÿród³a
    //! \param sm 
    void update(const IDataSourceManager* sm);
    //! Wywo³ywane, gdy pojawi¹ siê nowe sinki
    //! \param sm 
    void update(const IDataSinkManager* sm);

private:
    //! Dodanie nowego elementu do odpwiedniego widgeta
    //! \param entry nazwa/identyfikator elementu
    //! \param icon ikona elementu
    //! \param type typ elementu, warunkuje, do którego widgeta trafi wpis
    void addEntry(const QString& entry, const QIcon& icon, IVisualItem::Type type);

private:
    //! Model ze stylami elmentów
    CanvasStyleEditorPtr canvas;
    //! Stos komend (umo¿liwia m.in. undo)
	utils::ICommandStackPtr commmandStack;
    //! Scena, na któr¹ trafi¹ elementy
    SceneModelPtr sceneModel;
    //! nazwa/identyfikator elementu -> prototyp elementu
    std::map<QString, IWorkflowItemBasePtr> name2node;
    //! ui tworzone w designerze
    Ui::TypesWindow* ui;
};

}

#endif
