#include "MedusaExporterPCH.h"
#include "ExporterModel.h"
#include "IExporter.h"

using namespace medusaExporter;

void medusaExporter::ExporterModel::addExporter(const IExporterConstPtr& exporter)
{
	exporters.push_back(exporter);
}
