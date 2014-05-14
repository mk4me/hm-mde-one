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

    private slots:
        void onExport();

    private:
		MedusaExporterService* medusaService;
		Ui::MedusaExporter* ui;
    };
}

#endif
