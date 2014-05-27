#include "MedusaExporterPCH.h"
#include "CSVExporter.h"
#include "AnnotationData.h"
#include <plugins/dicom/Annotations.h>
#include <plugins/dicom/ValueLayer.h>
#include <plugins/dicom/ILayer.h>

using namespace medusaExporter;

bool isIdentical(const  std::vector<dicom::ILayerItemConstPtr>& layers) 
{
	for (auto it = layers.begin(); it != layers.end(); ++it) {
		auto val = utils::dynamic_pointer_cast<const dicom::IValueLayer>(*it);
		if (val) {
			if (val->valueAsString() == dicom::annotations::annotationValueAsString(dicom::annotations::identical).toStdString()){
				return true;
			}
		}
	}

	return false;
}

void medusaExporter::CSVExporter::exportAnnotations(const core::Filesystem::Path& path, const AnnotationData& data, const ExportConfig& config) const
{
	using namespace std;
	
	ofstream file;
	file.open(path.c_str(), ios::out);
	auto names = dicom::annotations::instance()->left;
	const auto& layers = data.getAnnotations();

	for (auto itLayer = layers.begin(); itLayer != layers.end(); ++itLayer) {
		if (config.skipIdentical && isIdentical(itLayer->second)) {
			continue;
		}
		file << itLayer->first.imageName << ", " << "numberOfLayers, " << (itLayer->second.size() + 1 ) << std::endl;
		file << "isPowerDoppler, " << itLayer->first.isPowerDoppler << std::endl;
		for (auto it = itLayer->second.begin(); it != itLayer->second.end(); ++it) {
			dicom::ILayerItemConstPtr itm = *it;

			file << names.at((dicom::annotations::annotationsIdx)itm->getAdnotationIdx()).toStdString(); // << ", " << itm->getName().toStdString();
			dicom::IValueLayerConstPtr val = utils::dynamic_pointer_cast<const dicom::IValueLayer>(itm);
			if (val) {
				file << ", " << val->valueAsString();
			}

			dicom::ILayerGraphicItemConstPtr graphic = utils::dynamic_pointer_cast<const dicom::ILayerGraphicItem>(itm);
			if (graphic) {
				auto points = graphic->getPointsCloud(config.pointsDensity);
				file << ", " << points.size();
				for (auto it = points.begin(); it != points.end(); ++it) {
					file << ", " << it->x() << ", " << it->y();
				}
			}
			file << std::endl;
		}
	}

	file.close();
}
