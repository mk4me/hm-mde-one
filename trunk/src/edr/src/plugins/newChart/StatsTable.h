/********************************************************************
	created:	2011/12/06
	created:	6:12:2011   10:34
	filename: 	StatsTable.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__STATSTABLE_H__
#define HEADER_GUARD_NEW_CHART__STATSTABLE_H__

#include "ui_statsTable.h"

class StatsTable : public QWidget, private Ui::StatsTable
{
    Q_OBJECT;
public:
    StatsTable(QWidget* parent = nullptr, Qt::WindowFlags f = 0);
	virtual ~StatsTable() {}

public slots:
    void setExpand(bool expand);
    void addEntry(const QString& name, ScalarChannelStatsConstPtr stats);

private slots:
    void onButton();

private:
    bool expanded;
};
typedef core::shared_ptr<StatsTable> StatsTablePtr;
typedef core::shared_ptr<const StatsTable> StatsTableConstPtr;


#endif
