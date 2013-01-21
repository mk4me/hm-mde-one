/********************************************************************
    created:  2011/04/28
    created:  28:4:2011   14:35
    filename: CoreTitleBar.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__CORETITLEBAR_H__
#define HEADER_GUARD_CORE__CORETITLEBAR_H__

#include <core/ui_CoreTitleBar.h>
#include <QtGui/QWidget>
#include <QtGui/QDockWidget>
#include <map>

class QToolBar;
class QAction;
class QAction;
class QDockWidget;

namespace coreUI {


//! Obiekt UI reprezentuj¹cy tytu³ dla okna. Pozwala wyœwietlaæ jego nazwê, zamykanie,
//! prze³anczanie pomiêdzy trybami dokowania/oddokowania. Ponadto oferuje mo¿liwoœæ
//! przechowywania akcji po lewej i po prawej stronie zorganizowanych w 2 toolbarach.
//! Mo¿e te¿ pracowaæ w wersji pionowej - experymentalna opcja:)
class CoreTitleBar : public QWidget, public Ui::CoreTitleBar
{
    Q_OBJECT

public:
	//! Typ opisuj¹cy stronê do której ma trafiæ dana akcja
	enum SideType {Left, Right};
	//! Typ opisuj¹cy sposób realizacji obrotu tytu³u okna
	enum TitleTextVerticalMode{
		Rotated,	//! Obrócony tytu³ o -45 stopnii - trzeba go czytaæ bokiem
		Vertical	//! Tytu³ napisany w pionie
	};

private:

	//! Typ œledz¹cy rozmieszczenie akcji wg stron
	typedef std::map<QObject *, SideType> ObjectsSides;

public:
    virtual ~CoreTitleBar();

public:

	static CoreTitleBar * supplyWithCoreTitleBar(QDockWidget * dockWidget);
	static void supplyCoreTitleBarWithActions(CoreTitleBar * titleBar, QWidget * widget);

	virtual bool eventFilter(QObject * watched, QEvent * event);

	void setTitleVerticalMode(TitleTextVerticalMode titleVerticalMode);
	TitleTextVerticalMode titleVerticalMode() const;
	QAction * actionAt(const QPoint & p) const;
	QAction * actionAt(int x, int y) const;
	void addAction(QAction * action, SideType side);
	QAction * addAction(const QString & text, SideType side);
	QAction * addAction(const QIcon & icon, const QString & text, SideType side);
	QAction * addAction(const QString & text, const QObject * receiver, const char * member, SideType side);
	QAction * addAction(const QIcon & icon, const QString & text, const QObject * receiver, const char * member, SideType side);
	QAction * addSeparator(SideType side);
	QAction * addWidget(QWidget * widget, SideType side);
	void clearAll();
	void clearSide(SideType side);
	QSize iconSize () const;
	QAction * insertSeparator(QAction * before);
	QAction * insertWidget(QAction * before, QWidget * widget);
	QAction * toggleViewAction() const;
	Qt::ToolButtonStyle titlebarButtonStyle() const;
	QWidget * widgetForAction(QAction * action) const;

    bool isTitleVisible() const;
    
public slots:

    void setTitleVisible(bool visible);

	//! Qt API
	void setIconSize(const QSize & iconSize);
	void setTitlebarButtonStyle(Qt::ToolButtonStyle titlebarButtonStyle);

private slots:

	void toggleFloating(bool floating);

	void refreshFeatures(QDockWidget::DockWidgetFeatures features);

	void onActionTriggeredLeft(QAction * action);
	void onActionTriggeredRight(QAction * action);
	void onToogleView(bool b);

signals:
	void actionTriggered(QAction * action, SideType side);
	void iconSizeChanged(const QSize & iconSize);
	void titlebarButtonStyleChanged(Qt::ToolButtonStyle titlebarButtonStyle);
	void visibilityChanged(bool visible);

protected:
    //virtual void paintEvent(QPaintEvent *paintEvent);
	virtual void changeEvent(QEvent * event);
	virtual bool event(QEvent * event);
	//virtual void resizeEvent(QResizeEvent * event);

private:
	CoreTitleBar(bool floating = false, QWidget * parent = nullptr);
	void updateTitleOrientation();

private:
	QLabel * titleLabel;
	QGraphicsScene * titleScene;
	QGraphicsProxyWidget * titleProxy;
	QGraphicsView * titleView;
    ObjectsSides objectsSides_;
	QToolBar * leftToolbar;
	QToolBar * rightToolbar;
	bool verticalOrientation_;
	TitleTextVerticalMode titleVerticalMode_;
	QAction * toogleViewAction_;
	bool floating_;
};

}

#endif  //  HEADER_GUARD_CORE__CORETITLEBAR_H__
