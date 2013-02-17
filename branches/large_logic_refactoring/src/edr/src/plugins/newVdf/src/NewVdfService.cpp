#include "NewVdfPCH.h"
#include <core/IServiceManager.h>
#include <plugins/newVdf/NewVdfService.h>
#include "NewVdfWidget.h"
#include "TypesWindow.h"
#include "CanvasStyleEditorWidget.h"
#include "SceneModel.h"
#include "VdfScene.h"
#include "Command.h"
#include "CommandStackDebug.h"
#include <QtCore/QObject>

NewVdfService::NewVdfService() :
	name("Data flow service"),
	commandStack(new CommandStack())
{
    CanvasStyleEditorPtr canvas(new CanvasStyleEditor());
    SceneModelPtr scene(new SceneModel(canvas));
    newVdfWidget = new NewVdfWidget(commandStack, scene);
	commandStackDebug = new CommandStackDebug(commandStack);
	QObject::connect(commandStack.get(), SIGNAL(changed()), commandStackDebug, SLOT(refresh()));
	commandStack->addCommand(ICommandPtr(new NullCommand()));
    typesWindow = new TypesWindow(commandStack, canvas, newVdfWidget);
    canvasStyleEditorWidget = new CanvasStyleEditorWidget(canvas);
	QObject::connect(canvasStyleEditorWidget, SIGNAL(backgroundAccepted(IBackgroundStrategyPtr)), newVdfWidget->getScene() , SLOT(setBackgroundStrategy(IBackgroundStrategyPtr)));
}

NewVdfService::~NewVdfService()
{
}


void NewVdfService::init( core::IManagersAccessor * managersAccessor )
{

}

const std::string& NewVdfService::getName() const
{
    return name;
}

QWidget* NewVdfService::getWidget( core::IActionsGroupManager * actionsManager )
{
	QWidget* widget = new QWidget();
	QLayout* layout = new QHBoxLayout(widget);
	layout->addWidget(commandStackDebug);
	commandStackDebug->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
	layout->addWidget(newVdfWidget);
	widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    return widget;
}

QWidget* NewVdfService::getSettingsWidget( core::IActionsGroupManager * actionsManager )
{
    return canvasStyleEditorWidget;
}

QWidget* NewVdfService::getControlWidget( core::IActionsGroupManager * actionsManager )
{
    return typesWindow;
}

