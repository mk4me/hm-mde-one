/********************************************************************
    created:  2011/04/28
    created:  28:4:2011   16:34
    filename: CoreDockWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__COREDOCKWIDGET_H__
#define HEADER_GUARD_CORE__COREDOCKWIDGET_H__

#include <utils/Export.h>
#include <QtGui/QDockWidget>

namespace coreUI {

//! Klasa rozszerza klasycznego QDockWidget o możliwosć przełanczania trybu oddokowywania na przeciwny,
//! ustawaiania okna jako "niesmiertelnego" przy zamykaniu go oraz modyfikacji jego flag przy oddokowywaniu
class UTILS_EXPORT CoreDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit CoreDockWidget(const QString &title, QWidget *parent = 0, Qt::WindowFlags flags = 0);
    explicit CoreDockWidget(QWidget *parent = nullptr, Qt::WindowFlags flags = 0);

    ~CoreDockWidget();

	static const bool isDockWidgetPermanent(const QDockWidget * dockWidget);
	static void setDockWidgetPermament(QDockWidget * dockWidget, bool permanent);

public Q_SLOTS:

    //! Zmienia stan dokowania.
    void toggleFloating();

public:

    bool isPermanent() const;
    void setPermanent(bool permanent);
};

}

#endif  //  HEADER_GUARD_CORE__COREDOCKWIDGET_H__

