#include "DicomPCH.h"
#include <corelib/IPlugin.h>
#include "IDicomService.h"
#include <plugins/dicom/Dicom.h>
#include <plugins/dicom/ILayeredImage.h>
#include "PngParser.h"
#include "DicomVisualizer.h"
#include "DicomSource.h"
#include "LayeredImageVisualizer.h"
#include "LayeredImage.h"
#include "DicomPerspective.h"
#include <plugins/hmdbCommunication/IHMDBSource.h>
#include <plugins/hmdbCommunication/IHMDBSourceViewManager.h>
#include <corelib/ISourceManager.h>
#include <corelib/Filesystem.h>
#include "LayersXmlParser.h"
#include "InternalXmlParser.h"
//#include "AnnotationStatusFilter.h"
#include <hmdbserviceslib/IBasicQueriesWS.h>

using namespace dicom;

//void szybkieSprawdzenieDuplikatow()
//{
//	typedef core::Filesystem fs;
//
//	std::set<fs::Path> goodImages;
//	std::set<fs::Path> notSetImages;
//	std::set<fs::Path> corruptImages;
//	std::set<fs::Path> identicalImages;
//	fs::Path dir = "C:/Users/Wojciech/Desktop/data_do_34_zrzut.15.5.2014/";
//	auto files = fs::listFiles(dir, true);
//
//	for (auto it = files.begin(); it != files.end(); ++it) {
//		try {
//			auto layers = DicomLoader::loadLayers(*it);
//			bool wasQual = false;
//			for (auto l = layers->begin(); l != layers->end(); ++l) {
//				auto qual = utils::dynamic_pointer_cast<ImageQualityLayer>(*l);
//				if (qual) {
//						wasQual = true;
//					if (qual->value() == adnotations::different) {
//						goodImages.insert(it->stem().stem());
//					}
//					else if (qual->value() == adnotations::incorrect) {
//						corruptImages.insert(it->stem().stem());
//					}
//					else {
//						identicalImages.insert(it->stem().stem());
//					}
//				}
//			}
//			if (!wasQual) {
//				//notSetImages.insert(it->stem().stem());
//				goodImages.insert(it->stem().stem());
//			}
//		} catch (...) {}
//	}
//
//	std::ofstream file;
//	file.open("C:/Users/Wojciech/Desktop/data_do_34_zrzut.15.5.2014/unique.txt", std::ios::out);
//	//file << "--- Images with 'image quality' property set to 'different' ---" << std::endl;
//	for (auto it = goodImages.begin(); it != goodImages.end(); ++it) {
//		if (corruptImages.find(*it) == corruptImages.end() && identicalImages.find(*it) == identicalImages.end()) {
//			file << (*it) << std::endl;
//		}
//	}
//
//	/*file << "--- Images with unknown quality ---" << std::endl;
//	for (auto it = notSetImages.begin(); it != notSetImages.end(); ++it) {
//		if (goodImages.find(*it) == goodImages.end()) {
//			file << (*it) << std::endl;
//		}
//	}*/
//
//	/*file << "--- ident ---" << std::endl;
//	for (auto it = identicalImages.begin(); it != identicalImages.end(); ++it) {
//	file << (*it) << std::endl;
//	}
//
//	file << "--- cprrupt quality ---" << std::endl;
//	for (auto it = corruptImages.begin(); it != corruptImages.end(); ++it) {
//	file << (*it) << std::endl;
//	}*/
//	file.close();
//
//}



class DicomTempService : public IDicomService
{																		
UNIQUE_ID("{2B0AE786-F194-46DE-A161-CCCFF317E44B}")						
CLASS_DESCRIPTION("DicomTempService", "DicomTempService");							

public:
	DicomTempService() : sourceManager(nullptr), memoryDataManager(nullptr), perspective(new DicomPerspective)
	{

	}

	virtual ~DicomTempService()
	{

	}

	virtual void init(core::ISourceManager * sourceManager, core::IVisualizerManager * visualizerManager,
		core::IMemoryDataManager * memoryDataManager, core::IStreamDataManager * streamDataManager, core::IFileDataManager * fileDataManager)
	{
		this->sourceManager = sourceManager;
		this->memoryDataManager = memoryDataManager;
	}

    virtual const bool lateInit()  
    {
		auto comm = core::querySource<hmdbCommunication::IHMDBSource>(plugin::getSourceManager());
		if (comm != nullptr){
			comm->viewManager()->addHierarchyPerspective(perspective);
			return true;
		}

		return false;
	}

	virtual void finalize()
	{
		
	}

	virtual void update(double deltaTime) {}
	virtual QWidget* getWidget() { return nullptr; }
	virtual QWidgetList getPropertiesWidgets() { return QWidgetList(); }

    virtual void updateItemIcon( const std::string& filename, const QIcon& icon )
    {
        auto item = perspective->tryGetHierarchyItem(filename);
        if (item) {
            item->setIcon(icon);
            auto transaction = memoryDataManager->transaction();
            auto hierarchyTransaction = memoryDataManager->hierarchyTransaction();
            core::IHierarchyItemConstPtr root = item;
            while (root->getParent() && root->getParent()->getParent()) {
                root = root->getParent();
            }
            if (root) {
                hierarchyTransaction->updateRoot(root);        
            }
        }
    }

private:
	core::ISourceManager *sourceManager;
	core::IMemoryDataManager* memoryDataManager;
	DicomPerspectivePtr perspective;
};

CORE_PLUGIN_BEGIN("dicom", core::UID::GenerateUniqueID("{09E8994A-99B4-42D6-9E72-C695ABFEAB1E}"))
    //CORE_PLUGIN_ADD_SOURCE(DicomSource);
	CORE_PLUGIN_ADD_PARSER(PngParser);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(IDicomInternalStruct);
    CORE_PLUGIN_ADD_OBJECT_WRAPPER(DicomInternalStruct);
    CORE_PLUGIN_ADD_OBJECT_WRAPPER(LayersVector);
    CORE_PLUGIN_ADD_PARSER(InternalXmlParser);
    CORE_PLUGIN_ADD_PARSER(LayersXmlParser);
    CORE_PLUGIN_ADD_VISUALIZER(LayeredImageVisualizer);
    CORE_PLUGIN_ADD_OBJECT_WRAPPER(ILayeredImage);
    CORE_PLUGIN_ADD_OBJECT_WRAPPER(LayeredImage);
    CORE_PLUGIN_ADD_SERVICE(DicomTempService);
CORE_PLUGIN_END