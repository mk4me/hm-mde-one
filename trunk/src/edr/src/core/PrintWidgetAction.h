/********************************************************************
	created:	2012/01/10
	created:	10:1:2012   11:03
	filename: 	PrintWidgetAction.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#include <QtGui/QAction>
#include <QtGui/QClipboard>

#ifndef HEADER_GUARD_CORE__PRINTWIDGETACTION_H__
#define HEADER_GUARD_CORE__PRINTWIDGETACTION_H__

class PrintWidgetAction : public QAction
{
    Q_OBJECT;
public:
    PrintWidgetAction(QWidget* widgetToPrint, const QString& text, QWidget* parent) : 
      QAction(text, parent), 
      widgetToPrint(widgetToPrint) 
    {
        connect(this, SIGNAL(triggered()), this, SLOT(onPrint()));
    }

	virtual ~PrintWidgetAction() {}

signals:
    void printTriggered(const QPixmap& pixmap);

private slots:
    void onPrint()
    {
        QPixmap pixmap;
        QGLWidget* gl = dynamic_cast<QGLWidget*>(widgetToPrint);
        if (gl) {
            pixmap = QPixmap::grabWindow(QApplication::desktop()->winId());
            QRect widgetRect = gl->geometry();
            widgetRect.moveTopLeft(gl->parentWidget()->mapToGlobal(widgetRect.topLeft()));
        
            pixmap = pixmap.copy(widgetRect);
            //pixmap = gl->renderPixmap();
        } else {
            pixmap = QPixmap::grabWidget(widgetToPrint);
        }
        
        /*static int c = 0;
        QString name = QString("C:\\Users\\Wojtek\\Desktop\\tmp\\Screen%1.png").arg(c++);
        pixmap.save(name);*/
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setImage(pixmap.toImage());
        emit printTriggered(pixmap);
    }

private:
    QWidget* widgetToPrint;
};
typedef core::shared_ptr<PrintWidgetAction> PrintWidgetActionPtr;
typedef core::shared_ptr<const PrintWidgetAction> PrintWidgetActionConstPtr;


#endif
