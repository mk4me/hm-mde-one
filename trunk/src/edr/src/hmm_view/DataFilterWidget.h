/********************************************************************
	created:	2011/10/04
	created:	4:10:2011   10:20
	filename: 	DataFilterWidget.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__DATAFILTERWIDGET_H__
#define HEADER_GUARD_HMM__DATAFILTERWIDGET_H__

#include <QtCore/QString>
#include <QtGui/QWidget>
#include <boost/date_time.hpp>
#include <boost/function.hpp>
#include <osg/Vec3>
#include "FilterEntryWidget.h"
#include "HmmMainWindow.h"
#include "ui_filter.h"


class DataFilterWidget : public QWidget, private Ui::FilterWidget
{
    Q_OBJECT;
public:
    DataFilterWidget(const QString& name, const QPixmap& pixmap, HmmMainWindow* hmmWindow);
	virtual ~DataFilterWidget() {}

public:
    void addFilter(const QString& bigLabelText, const QString& smallLabelText, DataFilterPtr dataFilter, const QPixmap* icon = nullptr);
    void addFilter(const QString& bigLabelText, const QString& smallLabelText, IFilterCommandPtr command, const QPixmap* icon = nullptr);
    void addFilter(FilterEntryWidget* entry);
    void closeFilters();
    bool getActive() const { return active; }
    void setActive(bool val);
    const QPixmap& getPixmap() const { return *this->pictureLabel->pixmap(); }
    QString getName() const { return this->label->text(); }

protected:
    bool eventFilter(QObject *object, QEvent *event);

signals:
    void activated(bool);

public:
    virtual void mousePressEvent(QMouseEvent *e);

private slots:
    void onClick();

private:
    std::vector<FilterEntryWidget*> entries;
    HmmMainWindow* hmmWindow;
    bool active;
    bool filtersClosed;
};
typedef boost::shared_ptr<DataFilterWidget> DataFilterWidgetPtr;
typedef boost::shared_ptr<const DataFilterWidget> DataFilterWidgetConstPtr;


#endif
