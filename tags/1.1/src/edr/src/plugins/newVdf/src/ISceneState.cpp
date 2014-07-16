#include "NewVdfPCH.h"
#include "ISceneState.h"
#include "SimpleItem.h"
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsView>
#include <QtGui/QHBoxLayout>
#include "SceneStateMachine.h"
#include "VdfScene.h"

using namespace vdf;

void changeCursor(SceneStateMachine* stateMachine, Qt::CursorShape cursor) 
{
	auto views = stateMachine->getScene()->views();
	for (auto it = views.begin(); it != views.end(); ++it) {
		(*it)->setCursor(cursor);
	}
}

