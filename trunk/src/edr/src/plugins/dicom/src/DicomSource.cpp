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
core::IHierarchyItemPtr DicomSource::transactionPart( const core::Filesystem::Path &path, const QString& desc,
	const bool hasPowerDoppler)
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
    QString desc2 = QString::fromStdString(path.string()) + QString("\n") + desc;

    if (oList.size() == 1) {
        utils::ObjectWrapperPtr wrapper = *oList.begin();
        core::Filesystem::Path p = path;
        p.replace_extension("xml");
        wrapper->setMetadata("DICOM_XML", std::string(p.string()));
        QIcon icon;
        if (core::Filesystem::pathExists(p)) {
            auto layers = DicomLoader::loadLayers(p);
            ILayeredImagePtr img = wrapper->get();
            for (auto it = layers->begin(); it != layers->end(); ++it) {
                img->addLayer(*it, "DicomSource");
            }
            icon = QIcon(":/dicom/file_done.png");
        } else {
            icon = QIcon(":/dicom/file.png");
        }

        return utils::make_shared<core::HierarchyDataItem>(*oList.begin(), icon, filename, desc2, utils::make_shared<Helper>(*oList.begin()));
    }

    UTILS_ASSERT(false, "Obsolete code");
    core::HierarchyItemPtr root = utils::make_shared<core::HierarchyItem>(filename, desc2, QIcon());
    for (auto it = oList.begin(); it != oList.end(); ++it) {
        core::HierarchyDataItemPtr item = utils::make_shared<core::HierarchyDataItem>(*it, QIcon(), QString((*it)->getClassName().c_str()), desc2,  utils::make_shared<Helper>(*it));
        root->appendChild(item);
    }

    return root;
}

dicom::LayersVectorPtr DicomLoader::loadLayers(const core::Filesystem::Path &p )
{
    std::ifstream ifs(p.string());
    boost::archive::xml_iarchive xmlIn(ifs);

    xmlIn.register_type<BackgroundLayer>();
    //xmlIn.register_type<CircleLayer>();
    xmlIn.register_type<PointsLayer>();
	xmlIn.register_type<BloodLevelLayer>();
	xmlIn.register_type<InflammatoryLevelLayer>();
	xmlIn.register_type<FingerTypeLayer>();
	xmlIn.register_type<JointTypeLayer>();
    LayersVectorPtr layers = utils::make_shared<LayersVector>();
    //xmlIn >> BOOST_SERIALIZATION_NVP(layers);
    xmlIn >> boost::serialization::make_nvp("layers", *layers);
    ifs.close();

	return layers;
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
    DicomInternalStructPtr inter = DicomLoader::load(path);
    int count = inter->getNumPatients();
    for (int i = 0; i < count; ++i) {
        auto patient = inter->getPatient(i);
        QString patientDesc = createDesc(*patient);
        for (auto itSession = patient->sessions.begin(); itSession != patient->sessions.end(); ++itSession) {
            QString name((*itSession)->getOutputDirectory().c_str());
            QString sessionDesc = createDesc(*(*itSession)) + QString("\n") + patientDesc;
            core::HierarchyDataItemPtr sessionItem = utils::make_shared<core::HierarchyDataItem>(QIcon(), name, sessionDesc);
            std::vector<LayerHelperPtr> helpers;

            for (auto itSerie = (*itSession)->series.begin(); itSerie !=  (*itSession)->series.end(); ++itSerie) {
                QString serieDesc = createDesc(*(*itSerie)) + QString("\n") + sessionDesc;
                for (auto itImage = (*itSerie)->images.begin(); itImage != (*itSerie)->images.end(); ++itImage) {
                    QString imageDesc = createDesc(*(*itImage)) + QString("\n") + serieDesc;
                    auto dataItem = transactionPart<LayerHelper>(path.parent_path() / (*itImage)->imageFile,
						imageDesc, (*itImage)->isPowerDoppler);
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

QString dicom::DicomSource::createDesc( const internalData::Patient& patient )
{
    QString desc;
    desc += QString("patientID : %1\n").arg(patient.patientID.c_str());
    desc += QString("patientName : %1\n").arg(patient.patientName.c_str());
    desc += QString("patientBirthdate : %1\n").arg(patient.patientBirthdate.c_str());
    desc += QString("patientSex : %1\n").arg(patient.patientSex.c_str());
    return desc;
}

QString dicom::DicomSource::createDesc( const internalData::Study& study )
{
    QString desc;
    desc += QString("studyId: %1\n").arg(study.studyId.c_str());
    desc += QString("studyDesc: %1\n").arg(study.studyDesc.c_str());
    desc += QString("studyDate: %1\n").arg(study.studyDate.c_str());
    desc += QString("studyTime: %1\n").arg(study.studyTime.c_str());
    desc += QString("physicianName: %1\n").arg(study.physicianName.c_str());
    desc += QString("accesionNumber: %1\n").arg(study.accesionNumber.c_str());
    desc += QString("studyNumber: %1\n").arg(study.studyNumber);
    return desc;
}

QString dicom::DicomSource::createDesc( const internalData::Serie& serie )
{
    QString desc;
    desc += QString("serieId : %1\n").arg(serie.serieId.c_str());
    desc += QString("modality : %1\n").arg(serie.modality.c_str());
    desc += QString("serieDate : %1\n").arg(serie.serieDate.c_str());
    desc += QString("serieTime : %1\n").arg(serie.serieTime.c_str());
    desc += QString("serieDesc : %1\n").arg(serie.serieDesc.c_str());
    desc += QString("serieNumber : %1\n").arg(serie.serieNumber.c_str());
    desc += QString("physicianName : %1\n").arg(serie.physicianName.c_str());
    return desc;
}

QString dicom::DicomSource::createDesc( const internalData::Image& image )
{
    QString desc;
    desc += QString("imageFile : %1\n").arg(image.imageFile.c_str());
    //desc += QString("adnotationsFile : %1\n").arg(image.adnotationsFile.c_str());
    desc += QString("originFilePath : %1\n").arg(image.originFilePath.c_str());
    desc += QString("isPowerDoppler : %1\n").arg(image.isPowerDoppler == true ? QObject::tr("yes") : QObject::tr("no"));
    return desc;
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

