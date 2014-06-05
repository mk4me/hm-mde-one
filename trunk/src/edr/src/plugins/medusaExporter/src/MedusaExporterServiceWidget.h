/********************************************************************
	created:	2014/04/29
	created:	29:4:2014   11:16
	filename: 	MedusaExporterServiceWidget.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MEDUSA_EXPORTER__MEDUSAEXPORTERSERVICEWIDGET_H__
#define HEADER_GUARD_MEDUSA_EXPORTER__MEDUSAEXPORTERSERVICEWIDGET_H__


#include <QtGui/QWidget>
#include <corelib/IFileDataManager.h>
#include "ExporterModel.h"

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
		void onDownload();
		void onExtract();
		void onExtractDirDialog();
		void onExportFrom();
        void onPackChecks();
        void onClearMedusaExport();
        void onOpenMedusaExport();

	private:
        void setExportFrom(const QString& dir, const ExporterModel::CallbackFunction& fun = ExporterModel::CallbackFunction());
		void callback(float ratio, const QString& description);

    private:
		MedusaExporterService* medusaService;
		Ui::MedusaExporter* ui;
		ExporterModelPtr exporterModel;
		ExporterModel::CallbackFunction callbackFunction;
    };
}

#endif
