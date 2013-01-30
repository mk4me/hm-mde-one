/********************************************************************
    created:  2013/01/30
    created:  30:1:2013   23:35
    filename: CoreSplitableDockWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___CORESPLITABLEDOCKWIDGET_H__
#define HEADER_GUARD___CORESPLITABLEDOCKWIDGET_H__

#include <coreui/CoreDockWidget.h>
#include <QtGui/QMainWindow>
#include <boost/type_traits.hpp>
#include <utils/Debug.h>

namespace coreUI {

	class CoreAction;

class UTILS_EXPORT CoreSplitableDockWidget : public CoreDockWidget
{
	Q_OBJECT

private:
	CoreAction * splitVerticalyAction;
	CoreAction * splitHorizontalyAction;

private:

	void init();

public:
	explicit CoreSplitableDockWidget(const QString &title, QWidget *parent = 0, Qt::WindowFlags flags = 0);
	explicit CoreSplitableDockWidget(QWidget *parent = nullptr, Qt::WindowFlags flags = 0);

	virtual ~CoreSplitableDockWidget();

public slots:
	void splitVerticaly();
	void splitHorizontaly();

	virtual void split(Qt::Orientation orientation) = 0;

signals:

	void splited(CoreSplitableDockWidget * newDockWidget, Qt::Orientation orientation);

};

template<class WidgetClonePolicy>
class CoreSplitableDockWidgetT : public CoreSplitableDockWidget
{
public:
	explicit CoreSplitableDockWidgetT(const QString &title, QWidget *parent = 0, Qt::WindowFlags flags = 0)
		: CoreSplitableDockWidget(title, parent, flags)
	{

	}

	explicit CoreSplitableDockWidgetT(QWidget *parent = nullptr, Qt::WindowFlags flags = 0)
		: CoreSplitableDockWidget(parent, flags)
	{

	}

	virtual ~CoreSplitableDockWidgetT()
	{

	}

public slots:

	virtual void split(Qt::Orientation orientation)
	{
		CoreSplitableDockWidgetT * newDockWidget = new CoreSplitableDockWidgetT(windowTitle(), nullptr, windowFlags());

		QMainWindow* mainWindow = qobject_cast<QMainWindow*>(parent());
		UTILS_ASSERT(mainWindow);

		QWidget * currentWidget = widget();
		if(currentWidget != nullptr){
			QWidget * newWidget = WidgetClonePolicy::cloneWidget(currentWidget);
			UTILS_ASSERT(newWidget);
			newDockWidget->setWidget(newWidget);
		}

		mainWindow->splitDockWidget(this, newDockWidget, orientation);

		emit splited(newDockWidget, orientation);
	}
};

template<class Widget>
class WidgetClonePolicyCopyConstructor {

	UTILS_STATIC_ASSERT((boost::is_base_of<QWidget, Widget>::value), "Widget class should inherit from QWidget");

	static QWidget * cloneWidget(QWidget * widget)
	{
		Widget * w = qobject_cast<Widget*>(widget);
		if(w == nullptr){
			return nullptr;
		}

		return new Widget(*w);
	}
};

}

#endif	//	HEADER_GUARD___CORESPLITABLEDOCKWIDGET_H__
