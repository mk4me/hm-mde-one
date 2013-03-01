#include "NewVdfPCH.h"
#include "NewVdfWidget.h"
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsView>
#include <QtGui/QVBoxLayout>
#include <coreui/CoreAction.h>
#include <coreui/CoreWidgetAction.h>
#include "SimpleItem.h"
#include "VdfScene.h"
#include "Command.h"
#include "VdfView.h"


using namespace vdf;

NewVdfWidget::NewVdfWidget(CommandStackPtr stack, SceneModelPtr sceneModel) :
    sceneModel(sceneModel),
	commandStack(stack)
{
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);
	this->setLayout(layout);

	stateMachine = SceneStateMachinePtr(new SceneStateMachine(this));
    connect(stateMachine.get(), SIGNAL(singleNodeSelected(IVisualNodePtr)), this, SIGNAL(singleNodeSelected(IVisualNodePtr)));
	scene = new VdfScene(stateMachine, sceneModel);
	view = new VdfView(scene);

	QToolBar* toolbar = new QToolBar("States");
    coreUI::CoreAction*  und = new coreUI::CoreAction(tr("Edit")  , QIcon(":/resources/icons/textedit/editundo.png"), tr("Undo"), this, coreUI::CoreTitleBar::Left);
    coreUI::CoreAction*  red = new coreUI::CoreAction(tr("Edit")  , QIcon(":/resources/icons/textedit/editredo.png"), tr("Redo"), this, coreUI::CoreTitleBar::Left);
    coreUI::CoreAction*  mrg = new coreUI::CoreAction(tr("Misc") , QIcon(":/resources/icons/skeletal_trace.png"), tr("Merge"), this, coreUI::CoreTitleBar::Left);
    coreUI::CoreAction*  clr = new coreUI::CoreAction(tr("Clear") , QIcon(":/resources/icons/manipulator-rotate.png"), tr("Scene"), this, coreUI::CoreTitleBar::Left);
    coreUI::CoreAction*  del = new coreUI::CoreAction(tr("Edit"), QIcon(":/resources/icons/x.png"), tr("Delete"), this, coreUI::CoreTitleBar::Left);

	mrg->setEnabled(false);
	
	connect(und, SIGNAL(triggered()), this, SLOT(undo()));
	connect(red, SIGNAL(triggered()), this, SLOT(redo()));
	connect(mrg, SIGNAL(triggered()), this, SLOT(merge()));
	connect(clr, SIGNAL(triggered()), this, SLOT(clearScene()));
	connect(del, SIGNAL(triggered()), this, SLOT(deleteSelected()));
       
    this->addAction(und);
    this->addAction(red);
    this->addAction(mrg);
    this->addAction(del);
    this->addAction(clr);
	
    layout->addWidget(view);
    setMinimumSize(500, 400);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}
    
NewVdfWidget::~NewVdfWidget()
{

}

void NewVdfWidget::undo()
{
	commandStack->undo();
}

void NewVdfWidget::redo()
{
	commandStack->redo();
}

void NewVdfWidget::merge()
{
	sceneModel->merge(scene->selectedItems());
}

void vdf::NewVdfWidget::deleteSelected()
{
    auto selected = stateMachine->getScene()->selectedItems();
    auto command = ICommandPtr(new RemoveSelectedCommand(stateMachine->getSceneModel(), selected));
    stateMachine->getCommandStack()->addCommand(command);
}

void vdf::NewVdfWidget::clearScene()
{
    if(QMessageBox::question( nullptr, tr("Confirm operation"), tr("Are You sure You want to clear scene?"),
        QMessageBox::Yes | QMessageBox::Abort, QMessageBox::Abort ) == QMessageBox::Yes) {
    	sceneModel->clearScene();
        stateMachine->getCommandStack()->clear();
    }
}
