#include "CoreUiPCH.h"
#include <coreui/CoreSplitableDockWidget.h>
#include <coreui/CoreAction.h>

using namespace coreUI;

CoreSplitableDockWidget::CoreSplitableDockWidget(const QString &title, QWidget *parent, Qt::WindowFlags flags)
	: CoreDockWidget(title, parent, flags)
{
	init();
}

CoreSplitableDockWidget::CoreSplitableDockWidget(QWidget *parent, Qt::WindowFlags flags)
	: CoreDockWidget(parent, flags)
{
	init();
}

void CoreSplitableDockWidget::init()
{
	splitVerticalyAction = new CoreAction(tr("Utils"), QIcon(QString::fromUtf8(":/resources/icons/vertical.png")), tr("Split verticaly"), this, CoreTitleBar::Right);

	connect(splitVerticalyAction, SIGNAL(triggered()), this, SLOT(splitVerticaly()));

	splitHorizontalyAction = new CoreAction(tr("Utils"), QIcon(QString::fromUtf8(":/resources/icons/horizontal.png")), tr("Split horizontaly"), this, CoreTitleBar::Right);

	connect(splitHorizontalyAction, SIGNAL(triggered()), this, SLOT(splitHorizontaly()));
}

CoreSplitableDockWidget::~CoreSplitableDockWidget()
{
	delete splitVerticalyAction;
	delete splitHorizontalyAction;
}

void CoreSplitableDockWidget::splitVerticaly()
{
	split(Qt::Vertical);
}

void CoreSplitableDockWidget::splitHorizontaly()
{
	split(Qt::Horizontal);
}