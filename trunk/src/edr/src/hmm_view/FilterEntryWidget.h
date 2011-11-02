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

class FilterEntryWidget : public QWidget, private Ui::FilterEntry
{
    Q_OBJECT;
public:
    FilterEntryWidget(QWidget* parent, HmmMainWindow* hmm, const QString& bigLabelText, const QString& smallLabelText, IFilterCommandPtr filterCommand, const QPixmap* icon = nullptr) :
      QWidget(parent),
      filterCommand(filterCommand),
      hmm(hmm),
      dialog(nullptr)
    {
        setupUi(this);
        this->pushButton->setText(bigLabelText);

        this->pushButton->setAttribute(Qt::WA_NoSystemBackground, true);
        this->smallLabel->setText(smallLabelText);
        if (icon) {
            this->pushButton->setIcon(*icon);
        }
        this->configurationButton->setVisible(false);
        this->setMouseTracking(true);

        connect(pushButton, SIGNAL(clicked()), this, SLOT(onButton()));
        connect(configurationButton, SIGNAL(clicked()), this, SLOT(onConfigurationButton()));
        QIcon icon2(core::getResourceString("images/przod.png"));
        configurationButton->setIcon(icon2);
    }
	virtual ~FilterEntryWidget() 
    {
        if (dialog) {
            delete dialog;
        }
    }

public:
    IFilterCommandPtr getFilterCommand() const { return filterCommand; }
    QString getName() const { return pushButton->text(); }

private slots:
    void onButton();
    void onConfigurationButton();

protected:
    virtual void enterEvent( QEvent * );
    virtual void leaveEvent( QEvent * );

private:
    IFilterCommandPtr filterCommand;
    HmmMainWindow* hmm;
    QDialog* dialog;
};


typedef boost::shared_ptr<FilterEntryWidget> FilterEntryWidgetPtr;
typedef boost::shared_ptr<const FilterEntryWidget> FilterEntryWidgetConstPtr;


#endif
