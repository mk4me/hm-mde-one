#include "DicomPCH.h"
#include <corelib/IPlugin.h>
#include <plugins/dicom/Dicom.h>
#include <plugins/dicom/ILayeredImage.h>
#include "PngParser.h"
#include "DicomVisualizer.h"
#include "DicomSource.h"
#include "LayeredImageVisualizer.h"
#include "LayeredImage.h"
#include "DicomPerspective.h"
#include <plugins/newCommunication/ICommunicationDataSource.h>
#include <corelib/ISourceManager.h>
#include "LayersXmlParser.h"
#include "InternalXmlParser.h"

using namespace dicom;

class DicomTempService : public plugin::IService
{																		
UNIQUE_ID("{2B0AE786-F194-46DE-A161-CCCFF317E44B}")						
CLASS_DESCRIPTION("DicomTempService", "DicomTempService");							
															
public:			
    DicomTempService() : sourceManager(nullptr) {}
    virtual void init(core::ISourceManager * sourceManager,core::IVisualizerManager * visualizerManager,						
        core::IMemoryDataManager * memoryDataManager, core::IStreamDataManager * streamDataManager, core::IFileDataManager * fileDataManager ) 
    {
        this->sourceManager = sourceManager;
    }						
    virtual const bool lateInit()  
    { 
        communication::ICommunicationDataSourcePtr comm = core::querySource<communication::ICommunicationDataSource>(sourceManager);
		if(comm != nullptr){
			comm->addHierarchyPerspective(utils::make_shared<DicomPerspective>());
			return true; 
		}

		return false;
    }

    virtual void finalize() {} 											
    virtual void update( double deltaTime ) {} 								 
    virtual QWidget* getWidget() { return nullptr; } 						 
    virtual QWidgetList getPropertiesWidgets() { return QWidgetList(); }     
private:
    core::ISourceManager *sourceManager;
};																			 

CORE_PLUGIN_BEGIN("dicom", core::UID::GenerateUniqueID("{09E8994A-99B4-42D6-9E72-C695ABFEAB1E}"))
    //CORE_PLUGIN_ADD_SOURCE(DicomSource);
    CORE_PLUGIN_ADD_PARSER(PngParser);
    CORE_PLUGIN_ADD_OBJECT_WRAPPER(DicomInternalStruct);
    CORE_PLUGIN_ADD_OBJECT_WRAPPER(LayersVector);
    CORE_PLUGIN_ADD_PARSER(InternalXmlParser);
    CORE_PLUGIN_ADD_PARSER(LayersXmlParser);
    CORE_PLUGIN_ADD_VISUALIZER(LayeredImageVisualizer);
    CORE_PLUGIN_ADD_OBJECT_WRAPPER(ILayeredImage);
    CORE_PLUGIN_ADD_OBJECT_WRAPPER(LayeredImage);
    CORE_PLUGIN_ADD_SERVICE(DicomTempService);
CORE_PLUGIN_END
