#include "CorePCH.h"
#include <stdexcept>
#include <QtGui/QMenu>
#include <QtGui/QPainter>
#include <core/ILog.h>
#include <core/PluginCommon.h>
#include "CoreTitleBar.h"
#include "CoreDockWidget.h"

using namespace coreUI;

CoreTitleBar::CoreTitleBar(bool floating, QWidget* parent) : QWidget(parent), titleLabel(new QLabel()),
	titleScene(new QGraphicsScene()), titleView(new QGraphicsView()), floating_(floating),
	leftToolbar(new QToolBar), rightToolbar(new QToolBar), toogleViewAction_(new QAction(nullptr)),
	verticalOrientation_(false), titleVerticalMode_(Rotated)
{
    setupUi(this);
	buttonFloat->setChecked(floating);
	leftToolbar->setParent(leftToolbarPlaceholder);
	rightToolbar->setParent(rightToolbarPlaceholder);
	leftToolbarPlaceholder->layout()->addWidget(leftToolbar);
	rightToolbarPlaceholder->layout()->addWidget(rightToolbar);

	connect(leftToolbar, SIGNAL(actionTriggered(QAction*)), this, SLOT(onActionTriggeredLeft(QAction*)));
	connect(rightToolbar, SIGNAL(actionTriggered(QAction*)), this, SLOT(onActionTriggeredRight(QAction*)));

	toogleViewAction_->setParent(this);
	toogleViewAction_->setCheckable(true);
	connect(toogleViewAction_, SIGNAL(triggered(bool)), this, SLOT(onToogleView(bool)));

	//obsługa tytułu
	//ustawiam to aby można było wrzucać tagi html - dla pionowych napisów
	titleLabel->setTextFormat(Qt::RichText);
	titleLabel->setParent(titleView);
	titleProxy = titleScene->addWidget(titleLabel);
	titleScene->setParent(titleView);
	titleView->setScene(titleScene);
	titleView->setParent(titlePlaceholder);
	titlePlaceholder->layout()->addWidget(titleView);
}

CoreTitleBar::~CoreTitleBar()
{

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

bool CoreTitleBar::eventFilter(QObject * watched, QEvent * event)
{
	if(event->type() == QEvent::WindowTitleChange){
		auto w = qobject_cast<QWidget *>(watched);
		toogleViewAction_->setText(w->windowTitle());
		titleLabel->setText(w->windowTitle());
	}

	return QWidget::eventFilter(watched, event);
}

void CoreTitleBar::changeEvent(QEvent * event)
{
	if(event->type() == QEvent::WindowTitleChange){
		toogleViewAction_->setText(windowTitle());
		titleLabel->setText(windowTitle());		
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

//void CoreTitleBar::resizeEvent(QResizeEvent * event)
//{
//	if(parentWidget() != nullptr){
//		QDockWidget *dockWidget = qobject_cast<QDockWidget*>(parentWidget());
//		if(dockWidget != nullptr) {
//			bool vertical = (dockWidget->features() & QDockWidget::DockWidgetVerticalTitleBar);
//			if(vertical != verticalOrientation_) {
//				verticalOrientation_ = vertical;
//				// pobieramy wszystkie widgety - ikonę, tytuł, lewy toolbar, prawy toolbar i buttony
//				auto widgets = layout()->children();
//				// będziemy je dodawać do nowego layoutu, stary usuniemy
//				QLayout * newLayout = nullptr;
//				if(verticalOrientation_ == true){
//					// I need to be vertical
//					newLayout = new QVBoxLayout(this);
//				} else {
//					// I need to be horizontal
//					newLayout = new QHBoxLayout(this);
//				}
//
//				for(int i = widgets.size(); i > 0; --i){
//					newLayout->addItem(layout()->takeAt(i));
//				}
//
//				delete layout();
//
//				//TODO
//				//kierunek tytułu okna
//				//QLabel* label = QLabel("Y axis"); 
//				//QGraphicsScene scene;
//				//QGraphicsProxyWidget  * proxy = scene.addWidget(label);
//				//proxy->rotate(-45);
//				//QGraphicsView view(&scene);
//				
//				//lub na bazie qxt
//				
//				//albo własna implementacja z pionowym textem
//				updateTitleOrientation();
//
//
//				leftToolbar->setOrientation(verticalOrientation_ == true ? Qt::Vertical : Qt::Horizontal);
//				rightToolbar->setOrientation(verticalOrientation_ == true ? Qt::Vertical : Qt::Horizontal);
//
//				setLayout(newLayout);
//			}
//		}
//	}
//
//	QWidget::resizeEvent(event);
//}

void CoreTitleBar::setTitleVerticalMode(TitleTextVerticalMode titleVerticalMode)
{
	if(titleVerticalMode != titleVerticalMode_)
	{
		titleVerticalMode_ = titleVerticalMode;
		if(verticalOrientation_ == true){
			updateTitleOrientation();
		}
	}
}

CoreTitleBar::TitleTextVerticalMode CoreTitleBar::titleVerticalMode() const
{
	return titleVerticalMode_;
}

void CoreTitleBar::updateTitleOrientation()
{
	if(verticalOrientation_ == true){
		switch(titleVerticalMode_){
		case Rotated:
			titleLabel->setText(windowTitle());
			titleProxy->rotate(-45);
			break;

		case Vertical:
			{
				QString newTitle(windowTitle());
				for(auto i = newTitle.size() - 2; i > 0; --i){
					newTitle.insert(i, "<BR>");
				}

				titleLabel->setText("<html>" + newTitle + "</html>");
			}
			break;
		}
	}else{
		titleProxy->rotate(0);
		titleLabel->setText(windowTitle());
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

void CoreTitleBar::addAction(QAction * action, SideType side)
{
	switch(side){
	case Left:
		leftToolbar->addAction(action);
		break;

	case Right:
		rightToolbar->addAction(action);
		break;
	}
}

QAction * CoreTitleBar::addAction(const QString & text, SideType side)
{
	QAction * ret = nullptr;
	switch(side){
	case Left:
		ret = leftToolbar->addAction(text);
		break;

	case Right:
		ret = rightToolbar->addAction(text);
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
		break;

	case Right:
		ret = rightToolbar->addAction(icon, text);
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
		break;

	case Right:
		ret = rightToolbar->addAction(text, receiver, member);
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
		break;

	case Right:
		ret = rightToolbar->addAction(icon, text, receiver, member);
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
		break;

	case Right:
		ret = rightToolbar->addSeparator();
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
		break;

	case Right:
		ret = rightToolbar->addWidget(widget);
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
		break;

	case Right:
		rightToolbar->clear();
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
			break;

		case Right:
			ret = rightToolbar->insertSeparator(before);
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
			break;

		case Right:
			ret = rightToolbar->insertWidget(before, widget);
			break;
		}
	}

	return ret;
}

void CoreTitleBar::onToogleView(bool b)
{
	if (b == isHidden()) {
		if (b)
			show();
		else
			close();
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
    return titleLabel->isVisible();
}


void CoreTitleBar::setTitleVisible(bool visible)
{
    titleLabel->setVisible(visible);
}


void CoreTitleBar::refreshFeatures(QDockWidget::DockWidgetFeatures features)
{
    if(features & QDockWidget::DockWidgetClosable){
        buttonClose->setVisible(true);
    }else{
        buttonClose->setVisible(false);
    }

    if(features & QDockWidget::DockWidgetFloatable){
        buttonFloat->setVisible(true);
    }else{
        buttonFloat->setVisible(false);
    }

    if(features & QDockWidget::DockWidgetMovable){
        setEnabled(true);
    }else{
        setEnabled(false);
    }

	bool vertical = features & QDockWidget::DockWidgetVerticalTitleBar;
	if(vertical != verticalOrientation_) {
		verticalOrientation_ = vertical;
		// pobieramy wszystkie widgety - ikonę, tytuł, lewy toolbar, prawy toolbar i buttony
		auto widgets = layout()->children();
		// będziemy je dodawać do nowego layoutu, stary usuniemy
		QLayout * newLayout = nullptr;
		if(verticalOrientation_ == true){
			// I need to be vertical
			newLayout = new QVBoxLayout(this);
		} else {
			// I need to be horizontal
			newLayout = new QHBoxLayout(this);
		}

		for(int i = widgets.size(); i > 0; --i){
			newLayout->addItem(layout()->takeAt(i));
		}

		delete layout();

		//TODO
		//kierunek tytułu okna
		//QLabel* label = QLabel("Y axis"); 
		//QGraphicsScene scene;
		//QGraphicsProxyWidget  * proxy = scene.addWidget(label);
		//proxy->rotate(-45);
		//QGraphicsView view(&scene);

		//lub na bazie qxt

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
        CORE_LOG_WARNING("DockWidget already has a titlebar!!");
        return nullptr;
    }

    auto titleBar = new CoreTitleBar(dockWidget->isFloating(), dockWidget);
    dockWidget->setTitleBarWidget(titleBar);
	titleBar->setWindowTitle(dockWidget->windowTitle());

	// obserwujemy zmiany tytulu okna
	dockWidget->installEventFilter(titleBar);
	//obserwujemy zmiany cech okna
    QObject::connect(dockWidget, SIGNAL(featuresChanged(QDockWidget::DockWidgetFeatures)), titleBar, SLOT(refreshFeatures(QDockWidget::DockWidgetFeatures)));
	//akcje na zamkniecie okna
    QObject::connect(titleBar->buttonClose, SIGNAL(triggered()), dockWidget, SLOT(close()));

    return titleBar;
}

void CoreTitleBar::supplyCoreTitleBarWithActions(CoreTitleBar * titleBar, QWidget * widget)
{

}
