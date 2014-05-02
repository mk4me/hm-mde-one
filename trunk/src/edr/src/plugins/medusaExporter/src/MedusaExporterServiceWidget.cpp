#include "MedusaExporterPCH.h"
#include "MedusaExporterServiceWidget.h"
#include "ui_MedusaExporter.h"
#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QLayout>
#include <QtGui/QPushButton>
#include <corelib/PluginCommon.h>
#include <corelib/IFileDataManager.h>
#include <corelib/Filesystem.h>
#include "MedusaExporterService.h"

using namespace medusaExporter;

MedusaExporterServiceWidget::MedusaExporterServiceWidget(MedusaExporterService* service) :
    medusaService(service),
    ui(new Ui::MedusaExporter())
{
    ui->setupUi(this);
    connect(ui->exportButton, SIGNAL(clicked()), this, SLOT(onExport()));
}

void medusaExporter::MedusaExporterServiceWidget::onExport()
{

}
