/********************************************************************
	created:	2014/01/17
	created:	17:1:2014   0:41
	filename: 	CorePopupPrivate.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_COREUI__COREPOPUPPRIVATE_H__
#define HEADER_GUARD_COREUI__COREPOPUPPRIVATE_H__

#include <QtCore/QTimer>
#include <QtGui/QLabel>

namespace coreUI {

    class CorePopupPrivate : public QLabel
    {
        Q_OBJECT
    public:
        CorePopupPrivate(const QString& message, int ms, const QRect& rect, QWidget* parent = 0, Qt::WindowFlags f = 0);
        virtual ~CorePopupPrivate() {}

        void start();
    
    Q_SIGNALS:
        void done(CorePopupPrivate*);

    private Q_SLOTS:
        void closePopup();

    private:
        QTimer timer;
        QRect rect;
    };

    class CorePopupDeleter : public QObject
    {
        Q_OBJECT;
    public Q_SLOTS:
        void deletePopup(CorePopupPrivate*);
    };
}
#endif
