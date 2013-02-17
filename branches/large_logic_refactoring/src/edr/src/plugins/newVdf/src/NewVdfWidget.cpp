#include "NewVdfPCH.h"
#include "NewVdfWidget.h"
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsView>
#include <QtGui/QVBoxLayout>
#include "SimpleItem.h"
#include "VdfScene.h"
#include "Command.h"
#include "VdfView.h"


NewVdfWidget::NewVdfWidget(CommandStackPtr stack, SceneModelPtr sceneModel) :
    sceneModel(sceneModel),
	commandStack(stack)
{
    QVBoxLayout* layout = new QVBoxLayout();
	this->setLayout(layout);

	stateMachine = SceneStateMachinePtr(new SceneStateMachine(this));

	scene = new VdfScene(stateMachine, sceneModel);
	view = new VdfView(scene);

	QToolBar* toolbar = new QToolBar("States");
	QPushButton* nrm = new QPushButton("Normal");
	QPushButton* cnt = new QPushButton("Connect");
	QPushButton* grp = new QPushButton("Group");
	QPushButton* und = new QPushButton("Undo");
	QPushButton* red = new QPushButton("Redo");
	QPushButton* mrg = new QPushButton("Merge");

	connect(nrm, SIGNAL(clicked()), scene, SLOT(normalPressed()));
	connect(cnt, SIGNAL(clicked()), scene, SLOT(connectPressed()));
	connect(grp, SIGNAL(clicked()), scene, SLOT(groupPressed()));
	connect(und, SIGNAL(clicked()), this, SLOT(undo()));
	connect(red, SIGNAL(clicked()), this, SLOT(redo()));
	connect(mrg, SIGNAL(clicked()), this, SLOT(merge()));

	toolbar->addWidget(nrm);
	toolbar->addWidget(cnt);
	toolbar->addWidget(grp);
	toolbar->addWidget(und);
	toolbar->addWidget(red);
	toolbar->addWidget(mrg);
	layout->addWidget(toolbar);
	
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
//void NewVdfWidget::addItem( IVisualItem* item )
//{
//    UTILS_ASSERT(item);
//    static int x = -100; 
//    static int y = 0;
//
//    x += 100;
//    if (x > 500) {
//        x = 0;
//        y += 100;
//    }
//
//    addItem(item, QPointF(x, y) );
//}
//
//void NewVdfWidget::addItem( IVisualItem* item, const QPointF scenePos )
//{
//    scene->addItem(item->visualItem());
//    item->visualItem()->setPos(scenePos);
//}
