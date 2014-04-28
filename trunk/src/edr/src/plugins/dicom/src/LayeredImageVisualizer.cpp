#include "DicomPCH.h"
#include "LayeredImageVisualizer.h"
#include <plugins/dicom/ILayeredImage.h>
#include <QtGui/QPixmap>
#include <QtGui/QLabel>
#include <QtGui/QHBoxLayout>
#include <QtGui/QSlider>
#include <QtGui/QPushButton>
#include "LayeredImageVisualizerView.h"
#include <QtGui/QApplication>
#include "BackgroundLayer.h"
#include <boost/serialization/shared_ptr.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include "LayeredImage.h"
#include "PointsLayer.h"
#include <corelib/PluginCommon.h>
#include <plugins/newCommunication/ICommunicationDataSource.h>
#include "corelib/ISourceManager.h"
#include "plugins/newCommunication/IDownloadRequest.h"
#include "coreui/CorePopup.h"
#include <QtGui/QMessageBox>
#include "IDicomService.h"
#include <corelib/IServiceManager.h>

using namespace dicom;

LayeredImageVisualizer::LayeredImageVisualizer() :  currentTrialID(-1)
{
	mainWidget = new LayeredImageVisualizerView(this);
	currentSerie = -1;
}

LayeredImageVisualizer::~LayeredImageVisualizer()
{

}

const bool LayeredImageVisualizer::userIsReviewer() const
{
    communication::ICommunicationDataSourcePtr comm = core::querySource<communication::ICommunicationDataSource>(plugin::getSourceManager());
    if (comm) {
        return comm->userIsReviewer();
    }

    return false;
}

plugin::IVisualizer* LayeredImageVisualizer::create() const
{
    return new LayeredImageVisualizer();
}

void LayeredImageVisualizer::setStatus(const webservices::xmlWsdl::AnnotationStatus::Type type,
	const QString & comment)
{
	auto ds = core::queryService<IDicomService>(plugin::getServiceManager());
	ds->setAnnotationStatus(currentLayerUser_, currentTrialID, type, comment.toStdString());
}

const std::string LayeredImageVisualizer::getCurrentLayerUserName() const
{
	return currentLayerUser_;
}

const int LayeredImageVisualizer::currnetTrialID() const
{
	return currentTrialID;
}

const bool LayeredImageVisualizer::trySave()
{
	if(mainWidget->isWindowModified() == false){
		return true;
	}

	if(QMessageBox::question(mainWidget, tr("Data changed"), tr("Some data was changed. Would You like to save changes or discard them?"),
		QMessageBox::Save, QMessageBox::Discard) == QMessageBox::Save){		

		try{
			saveSerie();
		}catch(...){
			return false;
		}
	}else{
		return true;
	}

	return true;
}

bool LayeredImageVisualizer::eventFilter(QObject * watched, QEvent * event)
{
	if(event->type() == QEvent::Close){
		//auto widget = qobject_cast<QWidget *>(watched);
		
		if(trySave() == true){
			event->accept();
		}else{
			event->ignore();
		}

		return true;
	}

	return QObject::eventFilter(watched, event);
}

plugin::IVisualizer::ISerie * LayeredImageVisualizer::createSerie( const utils::TypeInfo & requestedType, const core::ObjectWrapperConstPtr& data )
{
    auto serie = new LayeredSerie(this, mainWidget);
    serie->setData(requestedType, data);

    series.push_back(serie);
    
    if (series.size() == 1) {
        setActiveSerie(series[0]);
    }

    Q_EMIT serieChanged();

    return serie; 
}

plugin::IVisualizer::ISerie * LayeredImageVisualizer::createSerie( const plugin::IVisualizer::ISerie * serie )
{
    return nullptr;
}

plugin::IVisualizer::ISerie* dicom::LayeredImageVisualizer::createSerie( const ISerie* serie, const core::TypeInfo & requestedType, const core::ObjectWrapperConstPtr & data )
{
    return nullptr;
}


void LayeredImageVisualizer::removeSerie( plugin::IVisualizer::ISerie *serie )
{
    auto active = getActiveSerie();
    auto it = std::find(series.begin(), series.end(), serie);
    if (it != series.end()) {
        series.erase(it);
    }

    if (active == serie) {
        currentSerie = series.empty() ? 0 : 1;
    }

    Q_EMIT serieChanged();
}

void LayeredImageVisualizer::setActiveSerie( plugin::IVisualizer::ISerie * serie )
{
    auto prevSerie = getActiveSerie();
    int count = static_cast<int>(series.size());
    for (int i = 0; i < count; ++i) {
        if (series[i] == serie) {
            currentSerie = i;
            if (prevSerie != series[i]) {
                mainWidget->setWindowTitle(QString::fromStdString(series[i]->getName())+"[*]");
                Q_EMIT serieChanged();
            }
            return;
        }
    }   
}

const plugin::IVisualizer::ISerie * LayeredImageVisualizer::getActiveSerie() const
{
    return correctIndex(currentSerie) ? series[currentSerie] : nullptr;
}

plugin::IVisualizer::ISerie * dicom::LayeredImageVisualizer::getActiveSerie()
{
    return correctIndex(currentSerie) ? series[currentSerie] : nullptr;
}


QWidget* LayeredImageVisualizer::createWidget()
{
    return mainWidget;
}

QIcon* LayeredImageVisualizer::createIcon()
{
    return new QIcon();
}

int LayeredImageVisualizer::getMaxDataSeries() const
{
    return -1;
}

void LayeredImageVisualizer::getSupportedTypes( core::TypeInfoList & supportedTypes ) const
{
    supportedTypes.push_back(typeid(ILayeredImage));
}

void LayeredImageVisualizer::update( double deltaTime )
{
    if (correctIndex(currentSerie)) {
        auto serie = series[currentSerie];
        // nie jest to najszczesliwszy mechanizm, ale inicjalizacja jest mozliwa dopiero, gdy zna sie geometrie widgetow
        if (!serie->wasInitialized()) {
            serie->init();
        }
    }
}

QPixmap LayeredImageVisualizer::takeScreenshot() const
{
    if (correctIndex(currentSerie)) {
        return series[currentSerie]->getPixmap();
    }
    return QPixmap();
}

void dicom::LayeredImageVisualizer::setPrevSerie()
{
    if (correctIndex(currentSerie - 1)) {
        setActiveSerie(series[currentSerie - 1]);
    } 
}

void dicom::LayeredImageVisualizer::setNextSerie()
{
    if (correctIndex(currentSerie + 1)) {
        setActiveSerie(series[currentSerie + 1]);
    } 
}
//
void dicom::LayeredImageVisualizer::trySetSerie( int val )
{
    if (correctIndex(val)) {
        setActiveSerie(series[val]);
    }
}

bool dicom::LayeredImageVisualizer::correctIndex( int i ) const
{
    return i >= 0 && i < series.size();
}

int dicom::LayeredImageVisualizer::getNumSeries() const
{
    return series.size();
}

int dicom::LayeredImageVisualizer::getCurrentSerieNo() const
{
    return currentSerie;
}

void dicom::LayeredImageVisualizer::saveSerie()
{
    if (correctIndex(currentSerie)) {
        
        series[currentSerie]->save();
        
        auto service = core::queryService<IDicomService>(plugin::getServiceManager());
        QIcon icon(":/dicom/file_done.png");
        service->updateItemIcon(series[currentSerie]->getImageFilename(), icon);
        
		mainWidget->setWindowModified(false);
    }
}

void dicom::LayeredImageVisualizer::editSerie(int tagIdx, int idx)
{
    if (correctIndex(currentSerie)) {
        series[currentSerie]->editLayer(tagIdx,idx);
    }
}

void dicom::LayeredImageVisualizer::removeLayer(int tagIdx, int idx)
{
    if (correctIndex(currentSerie)) {
        series[currentSerie]->removeLayer(tagIdx,idx);
    }
}

void dicom::LayeredImageVisualizer::selectLayer(int tagIdx, int idx )
{
    LayeredSerie* serie = dynamic_cast<LayeredSerie*>(getActiveSerie());
    if (serie) {
        auto selected = selectedLayer();
        ILayeredImagePtr img = serie->getImage();
        std::string tag = img->getTag(tagIdx);
		currentLayerUser_ = tag;
		currentTrialID = img->getTrialID();
        if (/*selected.second != -1 && */(tag != selected.first || idx != selected.second)) {
			
			if(idx > -1){
				auto li = img->getLayerItem(tag, idx);			

				auto pl = utils::dynamic_pointer_cast<PointsLayer>(li);

				if(pl == nullptr){
					mainWidget->setDeletionButtonEnabled(false);
				}else{
					mainWidget->setDeletionButtonEnabled(true);
				}
			}
			
			serie->getGraphicsScene()->blockSignals(true);
            BOOST_FOREACH(std::string t, img->getTags()) {
                int count = img->getNumLayerItems(t);
                for (int i = 0; i < count; ++i) {

                    auto li = img->getLayerItem(t, i);
					
					auto gli = utils::dynamic_pointer_cast<ILayerGraphicItem>(li);

					if(gli != nullptr){
						gli->setSelected(tag == t && i == idx);
					}
                }
            }
            serie->getGraphicsScene()->blockSignals(false);
            serie->getLayersModel()->refreshSelected();
        }
    }
}

std::pair<std::string, int> dicom::LayeredImageVisualizer::selectedLayer() const
{
    int idx = -1;
    std::string selectedTag;
    const LayeredSerie* serie = dynamic_cast<const LayeredSerie*>(getActiveSerie());
    if (serie) {
        ILayeredImageConstPtr img = serie->getImage();
        BOOST_FOREACH(std::string tag, img->getTags()) {
            int count = img->getNumGraphicLayerItems(tag);
            for (int i = 0; i < count; ++i) {
                if (img->getLayerGraphicItem(tag, i)->getSelected()) {
                    if (idx == -1) {
                        idx = i;
                        selectedTag = tag;
                    } else {
                        // zaznaczono wiecej niz jedna serie danych !?
                        UTILS_ASSERT(false);
                        return std::make_pair(std::string(), -1);
                    }
                }
            }
        }
    }

    return std::make_pair(selectedTag, idx);
}

void dicom::LayeredImageVisualizer::uploadSerie()
{
    saveSerie();
    if (correctIndex(currentSerie)) {
        communication::ICommunicationDataSourcePtr comm = core::querySource<communication::ICommunicationDataSource>(plugin::getSourceManager());
        core::Filesystem::Path p(series[currentSerie]->getXmlOutputFilename());
        /// TODO : pobrac dane z OW
        if (!comm->offlineMode()) {
            comm->uploadMotionFile(p, "");
        } else {
            coreUI::CorePopup::showMessage(tr("Unable to upload file"), tr("Application is in offline mode"));
        }

		//TODO
		//zapis statusu adnotacji
    }
}

std::string dicom::LayeredImageVisualizer::getUserName() const
{
    communication::ICommunicationDataSourcePtr comm = core::querySource<communication::ICommunicationDataSource>(plugin::getSourceManager());
    return comm->currentUser()->name();
}

void dicom::LayeredImageVisualizer::removeSelectedLayers()
{
    LayeredSerie* serie = dynamic_cast<LayeredSerie*>(getActiveSerie());
    if (serie) {
        auto image = serie->getImage();
        int tagsNo = image->getNumTags();
        for (int iTag = 0; iTag < tagsNo; ++iTag) {
            auto tag = image->getTag(iTag);
            for (int itm = image->getNumGraphicLayerItems(tag) - 1; itm >= 0; --itm) {

				auto li = image->getLayerGraphicItem(tag, itm);

				if (li->getSelected() == true) {

					auto pl = utils::dynamic_pointer_cast<PointsLayer>(li);

					if(pl != nullptr){
						serie->removeLayer(iTag, itm);
					}
                }
            }
        }
    }
}

const bool dicom::LayeredImageVisualizer::verifyCompletness() const
{
	bool ret = true;

	const LayeredSerie* serie = dynamic_cast<const LayeredSerie*>(getActiveSerie());

	auto image = serie->getImage();

	int minimumLayers = 2;

	if(image->isPowerDoppler() == true){
		minimumLayers = 3;
	}

	if(image->getNumLayerItems(currentLayerUser_) < minimumLayers)
	{
		ret = false;
	}

	return ret;
}

const bool dicom::LayeredImageVisualizer::verifyImflamatoryLevel() const
{
	const LayeredSerie* serie = dynamic_cast<const LayeredSerie*>(getActiveSerie());

	auto image = serie->getImage();

	auto s = image->getNumLayerItems(currentLayerUser_);

	for(int i = 0; i < s; ++i){
		auto l = image->getLayerItem(currentLayerUser_, i);
		if(l->getAdnotationIdx() == dicom::adnotations::inflammatoryLevel){
			auto il = utils::dynamic_pointer_cast<const InflammatoryLevelLayer>(l);
			return il->value() != dicom::adnotations::unknownInflammatoryLevel;
		}
	}

	return false;
}

const bool dicom::LayeredImageVisualizer::verifyBloodLevel() const
{
	const LayeredSerie* serie = dynamic_cast<const LayeredSerie*>(getActiveSerie());

	auto image = serie->getImage();

	if(image->isPowerDoppler() == false){
		return true;
	}

	auto s = image->getNumLayerItems(currentLayerUser_);

	for(int i = 0; i < s; ++i){
		auto l = image->getLayerItem(currentLayerUser_, i);
		if(l->getAdnotationIdx() == dicom::adnotations::bloodLevel){
			auto bl = utils::dynamic_pointer_cast<const BloodLevelLayer>(l);
			return bl->value() != dicom::adnotations::unknownBloodLevel;
		}
	}

	return false;
}

const bool dicom::LayeredImageVisualizer::verifyJointType() const
{
	const LayeredSerie* serie = dynamic_cast<const LayeredSerie*>(getActiveSerie());

	auto image = serie->getImage();

	auto s = image->getNumLayerItems(currentLayerUser_);

	for(int i = 0; i < s; ++i){
		auto l = image->getLayerItem(currentLayerUser_, i);
		if(l->getAdnotationIdx() == dicom::adnotations::jointType){
			auto jt = utils::dynamic_pointer_cast<const JointTypeLayer>(l);
			return jt->value() != dicom::adnotations::unknownJoint;
		}
	}

	return false;
}

const bool dicom::LayeredImageVisualizer::verifyFingerType() const
{
	const LayeredSerie* serie = dynamic_cast<const LayeredSerie*>(getActiveSerie());

	auto image = serie->getImage();

	auto s = image->getNumLayerItems(currentLayerUser_);

	for(int i = 0; i < s; ++i){
		auto l = image->getLayerItem(currentLayerUser_, i);
		if(l->getAdnotationIdx() == dicom::adnotations::fingerType){
			auto ft = utils::dynamic_pointer_cast<const FingerTypeLayer>(l);
			return ft->value() != dicom::adnotations::unknownFinger;
		}
	}

	return false;
}