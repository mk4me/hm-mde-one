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

//! widget przechowujacy grupe filtrow
class DataFilterWidget : public QWidget, private Ui::FilterWidget
{
    Q_OBJECT;
public:
    //! Konstruktor
    //! \param name nazwa GUI
    //! \param pixmap ikona
    //! \param hmmWindow widok HMM
    DataFilterWidget(const QString& name, const QPixmap& pixmap, HmmMainWindow* hmmWindow);
	virtual ~DataFilterWidget() {}

public:
    //! 
    //! \param bigLabelText 
    //! \param dataFilter 
    //! \param icon 
    void addFilter(const QString& bigLabelText, PluginSubject::DataFilterPtr dataFilter, const QPixmap* icon = nullptr);
    //! 
    //! \param bigLabelText 
    //! \param command 
    //! \param icon 
    void addFilter(const QString& bigLabelText, IFilterCommandPtr command, const QPixmap* icon = nullptr);
    //! 
    //! \param entry 
    void addFilter(FilterEntryWidget* entry);
    //! 
    void closeFilters();
    //! 
    bool getActive() const { return active; }
    //! 
    //! \param val 
    void setActive(bool val);
    //! 
    const QPixmap& getPixmap() const { return *this->pictureLabel->pixmap(); }
    //! 
    QString getName() const { return this->label->text(); }
    //! 
    //! \param color 
    void setColor(const QColor& color);
    //! 
    int getNumEntries() const { return entries.size(); }
    //! 
    //! \param index 
    const FilterEntryWidget* getEntry(int index) const;

protected:
    //! 
    //! \param object 
    //! \param event 
    bool eventFilter(QObject *object, QEvent *event);

signals:
    //! 
    //! \param  
    void activated(bool);
    //! 
    void clicked();

public:
    //! 
    //! \param e 
    virtual void mousePressEvent(QMouseEvent *e);
    //! 
    //! \param toSkip 
    void uncheckEntries(FilterEntryWidget* toSkip = nullptr);

public slots:
    //! 
    void resetFilters();

private slots:
    //! 
    void onClick();

private:
    //! 
    std::vector<FilterEntryWidget*> entries;
    //! 
    HmmMainWindow* hmmWindow;
    //! 
    bool active;
    //! 
    bool filtersClosed;
};
typedef boost::shared_ptr<DataFilterWidget> DataFilterWidgetPtr;
typedef boost::shared_ptr<const DataFilterWidget> DataFilterWidgetConstPtr;


#endif
