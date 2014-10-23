/********************************************************************
	created:	2014/04/29
	created:	29:4:2014   11:16
	filename: 	MedusaExporterServiceWidget.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MEDUSA_EXPORTER__MEDUSAEXPORTERSERVICEWIDGET_H__
#define HEADER_GUARD_MEDUSA_EXPORTER__MEDUSAEXPORTERSERVICEWIDGET_H__


#include <QtWidgets/QWidget>
#include <corelib/IFileDataManager.h>
#include "ExporterModel.h"
#include <QtCore/QThread>
#include <QtWidgets/QAbstractButton>
#include <boost/bind.hpp>

class QPushButton;
class QLayout;

namespace core
{
    class IMemoryDataManager;
}

namespace Ui {
    class MedusaExporter;
}

namespace medusaExporter {

    class MedusaExporterService;



    class ExporterThread : public QThread
    {
        Q_OBJECT;
    public:
        ExporterThread() : 
            cc(boost::bind(&ExporterThread::callback, this, ::_1, ::_2)) {}
        void run() {
            cc.run();
        }

        void addOperation(const CallbackCollector::Operation& op, double weight, const QString& desc)
        {
            cc.addOperation(op, weight, desc);
        }
    Q_SIGNALS:
        void progressChanged(double ratio, const QString& description);

    private:
        void callback(double ratio, const QString& description)
        {
            emit progressChanged(ratio, description);
        }

    private:
        CallbackCollector cc;
    };


    class WidgetDisabler
    {
    public:
        WidgetDisabler() {}
        WidgetDisabler(QWidget* w) { addWidget(w); }
        WidgetDisabler(QWidget* w1, QWidget* w2) { addWidget(w1); addWidget(w2); }
        virtual ~WidgetDisabler()
        {
            for (auto it = widgets.begin(); it != widgets.end(); ++it) {
                (*it)->setEnabled(true);
            }

            for (auto it = checkedButtons.begin(); it != checkedButtons.end(); ++it) {
                (*it)->setChecked(false);
            }
        }

        void addWidget(QWidget* w) {
            w->setEnabled(false);
            widgets.push_back(w);
        }

        void checkButton(QAbstractButton* button) {
            button->setChecked(true);
            checkedButtons.push_back(button);
        }

    private:
        QWidgetList widgets;
        QList<QAbstractButton*> checkedButtons;
    };
    DEFINE_SMART_POINTERS(WidgetDisabler);

	class MedusaExporterServiceWidget : public QWidget
    {
    private:
        Q_OBJECT;
    public:
		MedusaExporterServiceWidget(MedusaExporterService* service);

	public:
		void setExporter(ExporterModelPtr exporterModel);
        void setExportDir(const QString& dir);
        QString getExportDir() const;

	private Q_SLOTS:
		void onExport();
		void onExtract();
		void onExtractDirDialog();
		void onExportFrom();
        void onPackChecks();
        void onClearMedusaExport();
        void onOpenMedusaExport();
	    void callback(double ratio, const QString& description);
        void afterExtract();
        void afterExport();
        void afterClear();

	private:
        void setExportFrom(const QString& dir, const QStringList& users);
	

    private:
		MedusaExporterService* medusaService;
		Ui::MedusaExporter* ui;
		ExporterModelPtr exporterModel;
		ExporterModel::CallbackFunction callbackFunction;
        mutable QStringList tempUsers;
        mutable QString innerExportDirPath;
        WidgetDisablerPtr disabler;
    };
}

#endif
