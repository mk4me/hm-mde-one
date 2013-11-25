#include "DicomPCH.h"
#include <corelib/IPlugin.h>
#include <plugins/dicom/Dicom.h>
#include <plugins/dicom/ILayeredImage.h>
#include "DicomParser.h"
#include "PngParser.h"
#include "DicomVisualizer.h"
#include "DicomSource.h"
#include "LayeredImageVisualizer.h"
#include "LayeredImage.h"

using namespace dicom;

CORE_PLUGIN_BEGIN("dicom", core::UID::GenerateUniqueID("{09E8994A-99B4-42D6-9E72-C695ABFEAB1E}"))
    CORE_PLUGIN_ADD_SOURCE(DicomSource);
	CORE_PLUGIN_ADD_PARSER(DicomParser);
    CORE_PLUGIN_ADD_PARSER(PngParser);
    CORE_PLUGIN_ADD_VISUALIZER(DicomVisualizer);
    CORE_PLUGIN_ADD_VISUALIZER(LayeredImageVisualizer);
    CORE_PLUGIN_ADD_OBJECT_WRAPPER(DcmDataset);
    CORE_PLUGIN_ADD_OBJECT_WRAPPER(DicomImage);
    CORE_PLUGIN_ADD_OBJECT_WRAPPER(ILayeredImage);
    CORE_PLUGIN_ADD_OBJECT_WRAPPER(LayeredImage);
CORE_PLUGIN_END
