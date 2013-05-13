#include "CoreUiPCH.h"
#include <stdexcept>
#include <QtGui/QMenu>
#include <QtGui/QPainter>
#include <corelib/ILog.h>
#include <corelib/PluginCommon.h>
#include <coreui/CoreTitleBar.h>
#include "ui_CoreTitleBar.h"
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsView>
#include <QtGui/QGraphicsProxyWidget>
#include <QtGui/QToolBar>
#include <QtGui/QAction>

using namespace coreUI;

CoreTitleBar::CoreTitleBar(bool floating, QWidget* parent) : QWidget(parent), ui(new Ui::CoreTitleBar),
	floating_(floating), leftToolbar(new QToolBar), rightToolbar(new QToolBar),
	toogleViewAction_(new QAction(nullptr)), verticalOrientation_(false)
{
    ui->setupUi(this);
	ui->titleLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);
	ui->iconPlaceholder->setVisible(false);
	ui->actionDock_Undock->setChecked(!floating);
	ui->buttonFloat->setDefaultAction(ui->actionDock_Undock);
	ui->buttonClose->setDefaultAction(ui->actionClose);
	leftToolbar->setParent(ui->leftToolbarPlaceholder);
	leftToolbar->setVisible(false);
	rightToolbar->setParent(ui->rightToolbarPlaceholder);
	rightToolbar->setVisible(false);
	ui->leftToolbarPlaceholder->layout()->addWidget(leftToolbar);
	ui->rightToolbarPlaceholder->layout()->addWidget(rightToolbar);

	connect(leftToolbar, SIGNAL(actionTriggered(QAction*)), this, SLOT(onActionTriggeredLeft(QAction*)));
	connect(rightToolbar, SIGNAL(actionTriggered(QAction*)), this, SLOT(onActionTriggeredRight(QAction*)));

	toogleViewAction_->setParent(this);
	toogleViewAction_->setCheckable(true);
	connect(toogleViewAction_, SIGNAL(triggered(bool)), this, SLOT(onToogleView(bool)));
	setIconSize(QSize(16,16));
}

CoreTitleBar::~CoreTitleBar()
{

}

void CoreTitleBar::setIcon(const QPixmap & icon)
{
	ui->iconPlaceholder->setPixmap(icon);

	if(icon.isNull() == true){
		ui->iconPlaceholder->setVisible(false);
	}else{
		ui->iconPlaceholder->setVisible(true);
	}
}

const QPixmap & CoreTitleBar::getIcon() const
{
	return *(ui->iconPlaceholder->pixmap());
}

void CoreTitleBar::onTopLevelChanged(bool floating)
{
	ui->buttonFloat->blockSignals(true);
	ui->buttonFloat->setChecked(floating);
	ui->buttonFloat->blockSignals(false);
}

void CoreTitleBar::toggleFloating(bool floating)
{
	if(parentWidget() != nullptr){
		QDockWidget *dockWidget = qobject_cast<QDockWidget*>(parentWidget());
		if(dockWidget != nullptr) {
			dockWidget->setFloating(floating);
		}
	}
}

void CoreTitleBar::closeWindow()
{
	if(parentWidget() != nullptr){
		QDockWidget *dockWidget = qobject_cast<QDockWidget*>(parentWidget());
		if(dockWidget != nullptr) {
			dockWidget->close();
			return;
		}
	}

	this->close();
}

bool CoreTitleBar::eventFilter(QObject * watched, QEvent * event)
{
	if(event->type() == QEvent::WindowTitleChange){
		auto w = qobject_cast<QWidget *>(watched);
		toogleViewAction_->setText(w->windowTitle());
		updateTitleOrientation();
	}

	return QWidget::eventFilter(watched, event);
}

void CoreTitleBar::changeEvent(QEvent * event)
{
	if(event->type() == QEvent::WindowTitleChange){
		toogleViewAction_->setText(windowTitle());
		updateTitleOrientation();	
	}

	QWidget::changeEvent(event);
}

bool CoreTitleBar::event(QEvent * event)
{
	switch (event->type()) {
	case QEvent::Hide:
		if (!isHidden())
			break;
		// fallthrough intended
	case QEvent::Show:
		toogleViewAction_->setChecked(event->type() == QEvent::Show);
		emit visibilityChanged(event->type() == QEvent::Show);
		break;
	}

	return QWidget::event(event);
}

void CoreTitleBar::updateTitleOrientation()
{
	if(verticalOrientation_ == true){
		
		QString newTitle(windowTitle());
		for(auto i = newTitle.size() - 1; i > 0; --i){
			newTitle.insert(i, "<BR>");
		}

		ui->titleLabel->setText("<html>" + newTitle + "</html>");
	}else{
		ui->titleLabel->setText(windowTitle());
	}
}

void CoreTitleBar::onActionTriggeredLeft(QAction * action)
{
	emit actionTriggered(action, Left);
}

void CoreTitleBar::onActionTriggeredRight(QAction * action)
{
	emit actionTriggered(action, Right);
}

QAction * CoreTitleBar::actionAt(const QPoint & p) const
{
	auto ret = leftToolbar->actionAt(p);
	if(ret == nullptr){
		ret = rightToolbar->actionAt(p);
	}

	return ret;
}

QAction * CoreTitleBar::actionAt(int x, int y) const
{
	auto ret = leftToolbar->actionAt(x,y);
	if(ret == nullptr){
		ret = rightToolbar->actionAt(x,y);
	}

	return ret;
}

void CoreTitleBar::addAction(QAction * action)
{
	SideType side = Left;
	auto a = dynamic_cast<ICoreTitleBarAction*>(action);
	if(a != nullptr){
		side = a->side();
	}

	addAction(action, side);
}

void CoreTitleBar::addAction(QAction * action, SideType side)
{
	switch(side){
	case Left:
		leftToolbar->addAction(action);
		leftToolbar->setVisible(true);
		break;

	case Right:
		rightToolbar->addAction(action);
		rightToolbar->setVisible(true);
		break;
	}
}

QAction * CoreTitleBar::addAction(const QString & text, SideType side)
{
	QAction * ret = nullptr;
	switch(side){
	case Left:
		ret = leftToolbar->addAction(text);
		leftToolbar->setVisible(true);
		break;

	case Right:
		ret = rightToolbar->addAction(text);
		rightToolbar->setVisible(true);
		break;
	}

	return ret;
}

QAction * CoreTitleBar::addAction(const QIcon & icon, const QString & text, SideType side)
{
	QAction * ret = nullptr;
	switch(side){
	case Left:
		ret = leftToolbar->addAction(icon, text);
		leftToolbar->setVisible(true);
		break;

	case Right:
		ret = rightToolbar->addAction(icon, text);
		rightToolbar->setVisible(true);
		break;
	}

	return ret;
}

QAction * CoreTitleBar::addAction(const QString & text, const QObject * receiver, const char * member, SideType side)
{
	QAction * ret = nullptr;
	switch(side){
	case Left:
		ret = leftToolbar->addAction(text, receiver, member);
		leftToolbar->setVisible(true);
		break;

	case Right:
		ret = rightToolbar->addAction(text, receiver, member);
		rightToolbar->setVisible(true);
		break;
	}

	return ret;
}

QAction * CoreTitleBar::addAction(const QIcon & icon, const QString & text, const QObject * receiver, const char * member, SideType side)
{
	QAction * ret = nullptr;
	switch(side){
	case Left:
		ret = leftToolbar->addAction(icon, text, receiver, member);
		leftToolbar->setVisible(true);
		break;

	case Right:
		ret = rightToolbar->addAction(icon, text, receiver, member);
		rightToolbar->setVisible(true);
		break;
	}

	return ret;
}

QAction * CoreTitleBar::addSeparator(SideType side)
{
	QAction * ret = nullptr;
	switch(side){
	case Left:
		ret = leftToolbar->addSeparator();
		leftToolbar->setVisible(true);
		break;

	case Right:
		ret = rightToolbar->addSeparator();
		rightToolbar->setVisible(true);
		break;
	}

	return ret;
}

QAction * CoreTitleBar::addWidget(QWidget * widget, SideType side)
{
	QAction * ret = nullptr;
	switch(side){
	case Left:
		ret = leftToolbar->addWidget(widget);
		leftToolbar->setVisible(true);
		break;

	case Right:
		ret = rightToolbar->addWidget(widget);
		rightToolbar->setVisible(true);
		break;
	}

	return ret;
}

void CoreTitleBar::clearAll()
{
	clearSide(Left);
	clearSide(Right);
}

void CoreTitleBar::clearSide(SideType side)
{
	switch(side){
	case Left:
		leftToolbar->clear();
		leftToolbar->setVisible(false);
		break;

	case Right:
		rightToolbar->clear();
		rightToolbar->setVisible(false);
		break;
	}
}

QSize CoreTitleBar::iconSize () const
{
	return leftToolbar->iconSize();
}

QAction * CoreTitleBar::insertSeparator(QAction * before)
{
	QAction * ret = nullptr;
	auto it = objectsSides_.find(before);
	if(it != objectsSides_.end()){
		switch(it->second){
		case Left:
			ret = leftToolbar->insertSeparator(before);
			leftToolbar->setVisible(true);
			break;

		case Right:
			ret = rightToolbar->insertSeparator(before);
			rightToolbar->setVisible(true);
			break;
		}
	}

	return ret;
}

QAction * CoreTitleBar::insertWidget(QAction * before, QWidget * widget)
{
	QAction * ret = nullptr;
	auto it = objectsSides_.find(before);
	if(it != objectsSides_.end()){
		switch(it->second){
		case Left:
			ret = leftToolbar->insertWidget(before, widget);
			leftToolbar->setVisible(true);
			break;

		case Right:
			ret = rightToolbar->insertWidget(before, widget);
			rightToolbar->setVisible(true);
			break;
		}
	}

	return ret;
}

void CoreTitleBar::onToogleView(bool b)
{
	if (b == isHidden()) {
		if (b)
			setVisible(true);
		else
			setVisible(false);
	}
}

QAction * CoreTitleBar::toggleViewAction() const
{
	return toogleViewAction_;
}

Qt::ToolButtonStyle CoreTitleBar::titlebarButtonStyle() const
{
	return leftToolbar->toolButtonStyle();
}

QWidget * CoreTitleBar::widgetForAction(QAction * action) const
{
	QWidget * ret = nullptr;
	auto it = objectsSides_.find(action);
	if(it != objectsSides_.end()){
		switch(it->second){
		case Left:
			ret = leftToolbar->widgetForAction(action);
			break;

		case Right:
			ret = rightToolbar->widgetForAction(action);
			break;
		}
	}

	return ret;
}

void CoreTitleBar::setIconSize(const QSize & iconSize)
{
	leftToolbar->setIconSize(iconSize);
	rightToolbar->setIconSize(iconSize);
}

void CoreTitleBar::setTitlebarButtonStyle(Qt::ToolButtonStyle titlebarButtonStyle)
{
	leftToolbar->setToolButtonStyle(titlebarButtonStyle);
	rightToolbar->setToolButtonStyle(titlebarButtonStyle);
}

//void CoreTitleBar::paintEvent( QPaintEvent *paintEvent )
//{
//    QStyleOption opt;
//    opt.init(this);
//    QPainter p(this);
//    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this); // PE_Widget
//}

bool CoreTitleBar::isTitleVisible() const
{
    return ui->titleLabel->isVisible();
}


void CoreTitleBar::setTitleVisible(bool visible)
{
    ui->titleLabel->setVisible(visible);
}


void CoreTitleBar::refreshFeatures(QDockWidget::DockWidgetFeatures features)
{
    if(features & QDockWidget::DockWidgetClosable){
        ui->buttonClose->setVisible(true);
    }else{
        ui->buttonClose->setVisible(false);
    }

    if(features & QDockWidget::DockWidgetFloatable){
        ui->buttonFloat->setVisible(true);
    }else{
        ui->buttonFloat->setVisible(false);
    }

    if(features & QDockWidget::DockWidgetMovable){
        setEnabled(true);
		//blockSignals(true);
    }else{
        setEnabled(false);
		//blockSignals(false);
    }

	bool vertical = (features & QDockWidget::DockWidgetVerticalTitleBar);
	if(vertical != verticalOrientation_) {
		verticalOrientation_ = vertical;

		// będziemy je dodawać do nowego layoutu, stary usuniemy
		QBoxLayout * newLayout = nullptr;
		if(verticalOrientation_ == true){
			// I need to be vertical
			newLayout = new QVBoxLayout();
			ui->titleLabel->setAlignment(Qt::AlignCenter);
			setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
		} else {
			// I need to be horizontal
			newLayout = new QHBoxLayout();
			ui->titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
			setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		}

		// pobieramy wszystkie widgety - ikonę, tytuł, lewy toolbar, prawy toolbar i button		
		while(layout()->isEmpty() == false){
			auto item = layout()->takeAt(layout()->count()-1);
			if(item->widget() != nullptr){
				newLayout->addWidget(item->widget());
			}else if(item->spacerItem()){
				newLayout->addStretch();
			}else {
				newLayout->addLayout(item->layout());
			}

			delete item;
		}

		delete layout();

		//albo własna implementacja z pionowym textem
		updateTitleOrientation();

		leftToolbar->setOrientation(verticalOrientation_ == true ? Qt::Vertical : Qt::Horizontal);
		rightToolbar->setOrientation(verticalOrientation_ == true ? Qt::Vertical : Qt::Horizontal);

		setLayout(newLayout);
	}
}

CoreTitleBar * CoreTitleBar::supplyWithCoreTitleBar(QDockWidget * dockWidget)
{
    if(dockWidget->titleBarWidget() != nullptr){
        return nullptr;
    }

    auto titleBar = new CoreTitleBar(dockWidget->isFloating(), dockWidget);
    dockWidget->setTitleBarWidget(titleBar);
	titleBar->setWindowTitle(dockWidget->windowTitle());
	titleBar->refreshFeatures(dockWidget->features());


	// obserwujemy zmiany tytulu okna
	dockWidget->installEventFilter(titleBar);
	//obserwujemy zmiany cech okna
    QObject::connect(dockWidget, SIGNAL(featuresChanged(QDockWidget::DockWidgetFeatures)), titleBar, SLOT(refreshFeatures(QDockWidget::DockWidgetFeatures)));
	//zmiana dokowania po stronie dockWidgeta z pominięciem titlebara
	QObject::connect(dockWidget, SIGNAL(topLevelChanged(bool)), titleBar, SLOT(onTopLevelChanged(bool)));

    return titleBar;
}

void CoreTitleBar::supplyCoreTitleBarWithActions(CoreTitleBar * titleBar, QWidget * widget)
{
	supplyCoreTitleBarWithActions(titleBar, widget->actions());
}

void CoreTitleBar::supplyCoreTitleBarWithActions(CoreTitleBar * titleBar, const QList<QAction *> & actions)
{
	for(auto it = actions.begin(); it != actions.end(); ++it){
		titleBar->addAction(*it);
	}
}

void CoreTitleBar::paintEvent(QPaintEvent * event)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}