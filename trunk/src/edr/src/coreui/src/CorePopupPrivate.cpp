#include "CoreUiPCH.h"
#include <coreui/CorePopup.h>
#include "CorePopupPrivate.h"
#include <coreui/CoreCursorChanger.h>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>

using namespace coreUI;

coreUI::CorePopupDeleter deleter;

void coreUI::CorePopup::showMessage( const QString& message, int ms, const QRect& rect )
{
    CorePopupPrivate* popup = new CorePopupPrivate(message, ms, rect, nullptr, Qt::Dialog | Qt::FramelessWindowHint);
    QObject::connect(popup, SIGNAL(done(CorePopupPrivate*)), &deleter, SLOT(deletePopup(CorePopupPrivate*)));
    popup->start();
}

void coreUI::CorePopup::showMessage( const QString& message, int ms ) 
{
    QDesktopWidget widget;
    QRect mainScreenSize = widget.availableGeometry(widget.primaryScreen());
    QRect popupRect(mainScreenSize.right() - 200, mainScreenSize.bottom() - 50, 200, 50);
    showMessage(message, ms, popupRect);
}

coreUI::CorePopupPrivate::CorePopupPrivate( const QString& message, int ms, const QRect& rect, QWidget* parent /*= 0*/, Qt::WindowFlags f /*= 0*/ ) :
    QLabel(message, parent, f),
    rect(rect)
{
    timer.setInterval(ms);
    connect(&timer, SIGNAL(timeout()), this, SLOT(closePopup()));
}

void coreUI::CorePopupPrivate::closePopup()
{
    hide();
    Q_EMIT done(this);
}

void coreUI::CorePopupPrivate::start()
{
    timer.start();
    setGeometry(rect);
    show();
}

void coreUI::CorePopupDeleter::deletePopup( CorePopupPrivate* cp)
{
    delete cp;
}
