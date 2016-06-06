#include "MedusaExporterPCH.h"
#include "AnnotationData.h"

void medusaExporter::AnnotationData::addAnnotation(const ImageInfo& info, dicom::ILayerItemConstPtr item)
{
	layers[info].push_back(item);
}

const medusaExporter::AnnotationData::Layers& medusaExporter::AnnotationData::getAnnotations() const
{
	return layers;
}

dicom::ILayerItemConstPtr medusaExporter::tryGetAnnotation(const std::vector<dicom::ILayerItemConstPtr>& layers, dicom::annotations::annotationsIdx annotationsIdx)
{
	for (auto it = layers.begin(); it != layers.end(); ++it) {
		if ((*it)->getAdnotationIdx() == (int)annotationsIdx) {
			return *it;
		}
	}
	return dicom::ILayerItemConstPtr();
}
