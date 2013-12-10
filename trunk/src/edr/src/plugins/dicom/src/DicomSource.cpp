#include "DicomSource.h"
#include <corelib/Filesystem.h>
#include <corelib/HierarchyItem.h>
#include <corelib/HierarchyDataItem.h>
#include "DicomSourceWidget.h"
#include <corelib/HierarchyHelper.h>
#include <plugins/dicom/Dicom.h>
#include <plugins/dicom/ILayeredImage.h>
#include "LayeredImage.h"
#include "corelib/IVisualizerManager.h"
#include "LayeredImageVisualizer.h"
#include <boost/archive/xml_iarchive.hpp>
#include "BackgroundLayer.h"
#include "DicomInternalStruct.h"
#include "PointsLayer.h"

using namespace dicom;



template <class HelperPtr>
class MultiLayerHelper : public HelperPtr::element_type
{
public:
    MultiLayerHelper(const std::vector<HelperPtr>& helpers ) :
      helpers(helpers)
      {

      }

      virtual void createSeries( const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::VisualizerSerie*>& series )
      {
          for (auto it = helpers.begin(); it != helpers.end(); ++it) {
              (*it)->createSeries(visualizer, path, series);
          }
      }

private:
    const std::vector<HelperPtr> helpers;

};
typedef MultiLayerHelper<LayerHelperPtr> MultiHelper;
DEFINE_SMART_POINTERS(MultiHelper);

DicomSource::DicomSource() :
    fileDM(nullptr),
    memoryDM(nullptr)
{

}


void DicomSource::init( core::IMemoryDataManager * memoryDM, core::IStreamDataManager * streamDM, core::IFileDataManager * fileDM )
{
    this->fileDM = fileDM;
    this->memoryDM = memoryDM;
    this->streamDM = streamDM;
}

bool DicomSource::lateInit()
{
    return true;
}

void DicomSource::finalize()
{
    
}

void DicomSource::update( double deltaTime )
{
    
}

QWidget* DicomSource::getWidget()
{
    return new DicomSourceWidget(this);
}

QWidget* DicomSource::getControlWidget()
{
    return nullptr;
}

QWidget* DicomSource::getSettingsWidget()
{
    return nullptr;
}

void DicomSource::getOfferedTypes( core::TypeInfoList & offeredTypes ) const
{
    
}

void DicomSource::addFile( const core::Filesystem::Path& path )
{
    auto root = transactionPart<core::WrappedItemHelper>(path);
    auto hierarchyTransaction = memoryDM->hierarchyTransaction();
    hierarchyTransaction->addRoot(root);
}



template <class Helper>
core::IHierarchyItemPtr DicomSource::transactionPart( const core::Filesystem::Path &path )
{
    auto transaction = fileDM->transaction();
    transaction->addFile(path);
    core::ConstObjectsList cList;
    transaction->getObjects(path, cList);

    core::ObjectsList oList;
    std::transform(cList.begin(), cList.end(), std::back_inserter(oList), 
        [&](utils::ObjectWrapperConstPtr w) 
        { 
            return utils::const_pointer_cast<utils::ObjectWrapper>(w); 
        });
    QString filename = QString::fromStdString(path.stem().string());
    QString desc = QString::fromStdString(path.string());

    if (oList.size() == 1) {
        utils::ObjectWrapperPtr wrapper = *oList.begin();
        core::Filesystem::Path p = path;
        p.replace_extension("xml");
        (*wrapper)[std::string("DICOM_XML")] = std::string(p.string());
        QIcon icon;
        if (core::Filesystem::pathExists(p)) {
            std::ifstream ifs(p.string());
            boost::archive::xml_iarchive xmlIn(ifs);

            xmlIn.register_type<BackgroundLayer>();
            //xmlIn.register_type<CircleLayer>();
            xmlIn.register_type<PointsLayer>();
            std::vector<ILayerItemPtr> layers;
            xmlIn >> BOOST_SERIALIZATION_NVP(layers);
            ifs.close();

            ILayeredImagePtr img = wrapper->get();
            for (auto it = layers.begin(); it != layers.end(); ++it) {
                img->addLayer(*it);
            }
            icon = QIcon(":/dicom/file_done.png");
        } else {
            icon = QIcon(":/dicom/file.png");
        }

        return utils::make_shared<core::HierarchyDataItem>(*oList.begin(), icon, filename, desc, utils::make_shared<Helper>(*oList.begin()));
    }

    UTILS_ASSERT(false, "Obsolete code");
    core::HierarchyItemPtr root = utils::make_shared<core::HierarchyItem>(filename, desc, QIcon());
    for (auto it = oList.begin(); it != oList.end(); ++it) {
        core::HierarchyDataItemPtr item = utils::make_shared<core::HierarchyDataItem>(*it, QIcon(), QString((*it)->getClassName().c_str()), desc,  utils::make_shared<Helper>(*it));
        root->appendChild(item);
    }

    return root;
}

dicom::DicomInternalStructPtr DicomLoader::load( const core::Filesystem::Path& from )
{
    DicomInternalStructPtr inter = utils::make_shared<DicomInternalStruct>();
    std::ifstream ifs(from.c_str());
    if (ifs.good()) {
        boost::archive::xml_iarchive xmlIn(ifs);
        xmlIn >> BOOST_SERIALIZATION_NVP(inter);
        ifs.close();
    }

    return inter;
}

void dicom::DicomSource::openInternalDataMainFile( core::Filesystem::Path path )
{
    DicomLoader loader;
    auto inter = loader.load(path);
    int count = inter->getNumPatients();
    for (int i = 0; i < count; ++i) {
        auto patient = inter->getPatient(i);
        for (auto itSession = patient->sessions.begin(); itSession != patient->sessions.end(); ++itSession) {
            QString name((*itSession)->getOutputDirectory().c_str());

            core::HierarchyDataItemPtr sessionItem = utils::make_shared<core::HierarchyDataItem>(QIcon(), name, QString());
            std::vector<LayerHelperPtr> helpers;

            for (auto itSerie = (*itSession)->series.begin(); itSerie !=  (*itSession)->series.end(); ++itSerie) {
                for (auto itImage = (*itSerie)->images.begin(); itImage != (*itSerie)->images.end(); ++itImage) {
                    auto dataItem = transactionPart<LayerHelper>(path.parent_path() / (*itImage)->imageFile);
                    core::HierarchyDataItemPtr di = utils::dynamic_pointer_cast<core::HierarchyDataItem>(dataItem); 
                    if (di) {
                        auto itemHelpers = di->getHelpers();
                        for (auto itHelper = itemHelpers.begin(); itHelper != itemHelpers.end(); ++itHelper) {
                            LayerHelperPtr helper = utils::dynamic_pointer_cast<LayerHelper>(*itHelper);
                            if (helper) {
                                helpers.push_back(helper);
                            }
                        }
                    }
                    sessionItem->appendChild(dataItem);

                }
            }
            
            int childrenCount = sessionItem->getNumChildren();
            if (childrenCount > 0) {  
                MultiHelperPtr multi = utils::make_shared<MultiHelper>(helpers);
                sessionItem->addHelper(multi);
            }

            if (sessionItem->getNumChildren()) {
                auto hierarchyTransaction = memoryDM->hierarchyTransaction();
                hierarchyTransaction->addRoot(sessionItem);
            }
        }
    }
}

std::vector<core::TypeInfo> dicom::LayerHelper::getTypeInfos() const
{
    std::vector<core::TypeInfo> ret;
    ret.push_back(typeid(ILayeredImage));
    return ret;
}

core::VisualizerPtr dicom::LayerHelper::createVisualizer( core::IVisualizerManager* manager )
{
    auto vis = manager->getVisualizerPrototype(LayeredImageVisualizer::getClassID());
    UTILS_ASSERT(vis);
    return core::VisualizerPtr(vis->create());
}

void dicom::LayerHelper::createSeries( const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::VisualizerSerie*>& series )
{
    auto serie = visualizer->createSerie(imageWrapper->getTypeInfo(), imageWrapper);
    serie->serie()->setName(path.toStdString());
    series.push_back(serie);
}

dicom::LayerHelper::LayerHelper( const core::ObjectWrapperConstPtr& imgWrapper ) :
imageWrapper(imgWrapper)
{

}
