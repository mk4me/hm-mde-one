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
#include "FilterCommand.h"
#include "ui_filterEntry.h"

class HmmMainWindow;

class FilterEntryWidget : public QWidget, private Ui::FilterEntry
{
    Q_OBJECT;
public:
    FilterEntryWidget(QWidget* parent, HmmMainWindow* hmm, const QString& bigLabelText, const QString& smallLabelText, IFilterCommandPtr filterCommand) :
      QWidget(parent),
      filterCommand(filterCommand),
      hmm(hmm)
    {
        setupUi(this);
        this->pushButton->setText(bigLabelText);
        this->smallLabel->setText(smallLabelText);
        connect(pushButton, SIGNAL(clicked()), this, SLOT(onButton()));
        connect(configurationButton, SIGNAL(clicked()), this, SLOT(onConfigurationButton()));
    }
	virtual ~FilterEntryWidget() {}
public:
    IFilterCommandPtr getFilterCommand() const { return filterCommand; }
    QString getName() const { return pushButton->text(); }

private slots:
    void onButton();
    void onConfigurationButton();

private:
    IFilterCommandPtr filterCommand;
    HmmMainWindow* hmm;
};


typedef boost::shared_ptr<FilterEntryWidget> FilterEntryWidgetPtr;
typedef boost::shared_ptr<const FilterEntryWidget> FilterEntryWidgetConstPtr;


#endif
