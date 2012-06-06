/********************************************************************
	created:	2011/10/05
	created:	5:10:2011   10:22
	filename: 	FilterEntryWidget.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__FILTERENTRYWIDGET_H__
#define HEADER_GUARD_HMM__FILTERENTRYWIDGET_H__

#include <boost/function.hpp>
#include <QtGui/QWidget>
#include <osg/Vec3>
#include "FilterCommand.h"
#include "ui_filterEntry.h"

class HmmMainWindow;

//! widget z widokiem dla filtru
class FilterEntryWidget : public QWidget, private Ui::FilterEntry
{
    Q_OBJECT;
public:
    FilterEntryWidget(QWidget* parent, HmmMainWindow* hmm, const QString& bigLabelText, IFilterCommandPtr filterCommand, const QPixmap* icon = nullptr);
	virtual ~FilterEntryWidget();

public:
    IFilterCommandPtr getFilterCommand() const { return filterCommand; }
    QTreeWidgetItem* createTreeEntry(const std::vector<PluginSubject::SessionConstPtr>& sessions);
    QWidget* getConfigurator() const { return filterCommand->getConfigurationWidget(); }
    QString getName() const { return pushButton->text(); }
    bool isChecked() const { return pushButton->isChecked(); }
    void setChecked(bool val) { pushButton->setChecked(val); }

signals:
    void onFilterClicked(FilterEntryWidget* filter);

private slots:
    void onButton();

private:
    IFilterCommandPtr filterCommand;
    HmmMainWindow* hmm;
    QWidget* configurator;
};


typedef boost::shared_ptr<FilterEntryWidget> FilterEntryWidgetPtr;
typedef boost::shared_ptr<const FilterEntryWidget> FilterEntryWidgetConstPtr;


#endif
