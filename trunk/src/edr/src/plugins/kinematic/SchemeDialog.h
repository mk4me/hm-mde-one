/********************************************************************
	created:	2011/09/06
	created:	6:9:2011   12:49
	filename: 	SchemeDialog.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_SCHEME_DIALOG__SchemeDialog_H__
#define HEADER_GUARD_SCHEME_DIALOG__SchemeDialog_H__


#include <core/IVisualizer.h>
#include <osg/Geode>
#include <boost/tuple/tuple.hpp>
#include "TrajectoriesDrawer.h"
#include "GlLineSchemeDrawer.h"
#include "GlPointSchemeDrawer.h"
#include "ui_TrajectoriesDialog.h"


class SchemeDialog : public QDialog, private Ui::TrajectoriesDialog
{
	Q_OBJECT;
public:
	SchemeDialog(QWidget* parent);

public:
	void setDrawer(OsgSchemeDrawerPtr drawer, const QString& rootName);

private slots:
	//void colorClicked();
	void visibilityChanged(bool visible);
    void rootVisibilityChanged(bool visible);
    void globalColorClicked();
	//void widthChanged(double width);
	//void startTimeChanged(double time);
	//void endTimeChanged(double time);
	//void treeItemChanged ( QTreeWidgetItem * current, QTreeWidgetItem * previous );
	//void startSliderChanged(int value);
	//void endSliderChanged(int value);
  

private:
	QColor transformColor(const osg::Vec4& color) const;
	osg::Vec4 transformColor(const QColor& color) const;
	QTreeWidgetItem* getItemWhichContains(QObject* object) const;
	void setButtonColor(QPushButton* button, const QColor& color);
	void blockAllSignals(bool val);

private:
    std::map<QTreeWidgetItem*, boost::tuple<GlPointSchemeDrawerPtr, GlLineSchemeDrawerPtr, int>> item2Drawer;
    std::map<QTreeWidgetItem*, OsgSchemeDrawerPtr> item2Root;
};


#endif
