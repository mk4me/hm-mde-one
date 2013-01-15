/********************************************************************
    created:  2011/05/27
    created:  27:5:2011   11:17
    filename: LocalDataSourceWidget.h
    author:   Mateusz Janiak

    purpose:
*********************************************************************/
#ifndef HEADER_GUARD___LOCALDATASOURCEWIDGET_H__
#define HEADER_GUARD___LOCALDATASOURCEWIDGET_H__

#include <QtGui/QWidget>
#include <QtGui/QCheckBox>
#include "ui_LocalDataSourceWidget.h"
#include <core/TypeInfo.h>
#include "LocalDataSource.h"

namespace core {

class LocalDataSourceWidget : public QWidget, public Ui::LocalDataSourceWidget
{
    Q_OBJECT
public:
    LocalDataSourceWidget(LocalDataSource * dataSource, QWidget * parent = nullptr);
    ~LocalDataSourceWidget();


public slots:

    void onOpen();
    void onEdit(const QString & text);

private slots:
    void onCheckChange(int state);

private:
    std::map<QCheckBox*, TypeInfo> checkBoxToTypeMapping;
    LocalDataSource * localDataSource;
    LocalDataSource::Data data;

};

}

#endif  //  HEADER_GUARD___LOCALDATASOURCEWIDGET_H__
