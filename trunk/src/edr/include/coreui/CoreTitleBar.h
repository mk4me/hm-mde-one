/********************************************************************
    created:  2011/04/28
    created:  28:4:2011   14:35
    filename: CoreTitleBar.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__CORETITLEBAR_H__
#define HEADER_GUARD_CORE__CORETITLEBAR_H__

#include <coreui/Export.h>
#include <QtWidgets/QWidget>
#include <QtWidgets/QDockWidget>
#include <map>

class QToolBar;
class QAction;
class QDockWidget;
class QMouseEvent;

namespace Ui {
	class CoreTitleBar;
}

namespace coreUI {


//! Obiekt UI reprezentuj�cy tytu� dla okna. Pozwala wy�wietla� jego nazw�, zamykanie,
//! prze�anczanie pomi�dzy trybami dokowania/oddokowania. Ponadto oferuje mo�liwo��
//! przechowywania akcji po lewej i po prawej stronie zorganizowanych w 2 toolbarach.
//! Mo�e te� pracowa� w wersji pionowej - experymentalna opcja:)
class COREUI_EXPORT CoreTitleBar : public QWidget
{
    Q_OBJECT

public:
	//! Typ wyliczeniowy opisuj�cy domy�lny mechanizm dwukliku na tytule okna
	enum ToggleType { Dock, FullScreen };

	//! Typ opisuj�cy stron� do kt�rej ma trafi� dana akcja
	enum SideType {Left, Right, Both, Any};
	//! Interfejs akcji chc�cych potencjalnie dzia�a� z corowym titlebarem
	class ICoreTitleBarAction
	{
	public:
		//! \return Strona po kt�rej ma si� pojawi� akcja
		virtual SideType side() const = 0;
	};

public:
	//! Destruktor wirtualny
    virtual ~CoreTitleBar();

public:

	static CoreTitleBar * supplyWithCoreTitleBar(QDockWidget * dockWidget, const bool observe = true);
	static void supplyCoreTitleBarWithActions(CoreTitleBar * titleBar, const QList<QAction *> & actions);
	static void supplyCoreTitleBarWithActions(CoreTitleBar * titleBar, QWidget * widget);

	virtual bool eventFilter(QObject * watched, QEvent * event);

	QAction * actionAt(const QPoint & p) const;
	QAction * actionAt(int x, int y) const;
	void addAction(QAction * action);
	void addAction(QAction * action, SideType side);
	SideType actionSide(QAction * action) const;
	QAction * addAction(const QString & text, SideType side = Left);
	QAction * addAction(const QIcon & icon, const QString & text, SideType side = Left);
	QAction * addAction(const QString & text, const QObject * receiver, const char * member, SideType side = Left);
	QAction * addAction(const QIcon & icon, const QString & text, const QObject * receiver, const char * member, SideType side = Left);
	QAction * addSeparator(SideType side = Left);
	QAction * addWidget(QWidget * widget, SideType side = Left);	
	void clearActions(SideType side);
	void setActionsEnabled(SideType side, bool enable);
	void setActionsVisible(SideType side, bool visible);
	QSize iconSize () const;
	QAction * insertSeparator(QAction * before);
	QAction * insertWidget(QAction * before, QWidget * widget);
	QAction * toggleVisibilityAction();
	Qt::ToolButtonStyle titlebarButtonStyle() const;
	QWidget * widgetForAction(QAction * action);

    bool isTitleVisible() const;

	void setToggleType(ToggleType toggleType);
	ToggleType toggleType() const;
    
public slots:

    void setTitleVisible(bool visible);
	void setToggleDockButtonVisible(bool visible);
	void setToggleDockButtonEnabled(bool enable);
	void setToggleFullScreenButtonVisible(bool visible);
	void setToggleFullScreenButtonEnabled(bool enable);
	void setCloseButtonVisible(bool visible);
	void setCloseButtonEnabled(bool enable);

	//! Qt API obslugujace kssztalt i rozmiar dla akcji
	void setIconSize(const QSize & iconSize);
	void setTitlebarButtonStyle(Qt::ToolButtonStyle titlebarButtonStyle);

private slots:

	void toggleFloating(bool floating);
	void toggleFullScreen(bool fullScreen);
	void toggleVisibility(bool visible);

	void refreshFeatures(QDockWidget::DockWidgetFeatures features);

	void onActionTriggeredLeft(QAction * action);
	void onActionTriggeredRight(QAction * action);
	

	void onTopLevelChanged(bool floating);

	void closeWindow();

signals:
	void actionTriggered(QAction * action, SideType side);
	void iconSizeChanged(const QSize & iconSize);
	void titlebarButtonStyleChanged(Qt::ToolButtonStyle titlebarButtonStyle);
	void visibilityChanged(bool visible);

protected:

	virtual void mouseDoubleClickEvent(QMouseEvent * event);

    //virtual void paintEvent(QPaintEvent *paintEvent);
	virtual void changeEvent(QEvent * event);
	virtual bool event(QEvent * event);
	virtual void paintEvent(QPaintEvent * event);

private:
	CoreTitleBar(bool floating, QWidget * parent = nullptr, ToggleType toggleType = Dock);
	void updateTitleOrientation();

private:	
	Ui::CoreTitleBar * ui;    
	QToolBar * leftToolbar;
	QToolBar * rightToolbar;
	bool verticalOrientation_;
	bool floating_;
	bool wasFloating_;
	bool updateFullScreen;
	ToggleType toggleType_;
};

}

#endif  //  HEADER_GUARD_CORE__CORETITLEBAR_H__
