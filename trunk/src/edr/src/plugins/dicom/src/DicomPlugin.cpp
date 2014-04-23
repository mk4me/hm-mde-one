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
#include <plugins/newCommunication/ICommunicationDataSource.h>
#include <corelib/ISourceManager.h>
#include "LayersXmlParser.h"
#include "InternalXmlParser.h"
#include "AnnotationStatusFilter.h"

using namespace dicom;

class DicomTempService : public IDicomService, public utils::Observer<communication::ICommunicationDataSource>
{																		
UNIQUE_ID("{2B0AE786-F194-46DE-A161-CCCFF317E44B}")						
CLASS_DESCRIPTION("DicomTempService", "DicomTempService");							
		
private:

	typedef std::map<int, std::map<int, AnnotationStatus>> AnnotationsMap;

private:

	static const AnnotationsMap convert(const webservices::xmlWsdl::AnnotationsList & annotations)
	{
		AnnotationsMap ret;

		for(auto it = annotations.begin(); it != annotations.end(); ++it){

			const webservices::xmlWsdl::Annotation & a = *it;

			AnnotationStatus as;
			as.status = a.status;
			as.comment = a.comment;
			as.note = a.note;

			ret[a.userID][a.trialID] = as;
		}

		return ret;
	}

public:			
    DicomTempService() : sourceManager(nullptr), memoryDataManager(nullptr), inEditionFilterIDX(-1), perspective(new DicomPerspective),
	inVerificationFilterIDX(-1), verifiedFilterIDX(-1), usersToRefresh(true) {}

    virtual void init(core::ISourceManager * sourceManager,core::IVisualizerManager * visualizerManager,						
        core::IMemoryDataManager * memoryDataManager, core::IStreamDataManager * streamDataManager, core::IFileDataManager * fileDataManager ) 
    {
        this->sourceManager = sourceManager;
        this->memoryDataManager = memoryDataManager;
    }		

    virtual const bool lateInit()  
    { 
        comm = core::querySource<communication::ICommunicationDataSource>(sourceManager);
		if(comm != nullptr){

			comm->attach(this);

			auto as = QObject::tr("Annotation status");

			//dodajemy filtry dla adnotacji
			inEditionFilterIDX = comm->addDataFilter(inEditionFilter = new AnnotationStatusFilter(std::string((as + ": " + QObject::tr("in edition")).toStdString()), true, false));
			inVerificationFilterIDX = comm->addDataFilter(inVerificationFilter = new AnnotationStatusFilter(std::string((as + ": " + QObject::tr("in verification")).toStdString()), true, false));
			verifiedFilterIDX = comm->addDataFilter(verifiedFilter = new AnnotationStatusFilter(std::string((as + ": " + QObject::tr("verified")).toStdString()), true, false));
			comm->addHierarchyPerspective(perspective);
			return true; 
		}

		return false;
    }

	virtual void finalize()
	{
		comm->detach(this);
		comm.reset();
	}

	virtual void update(const communication::ICommunicationDataSource * subject)
	{
		if(subject->isLogged() == true){

			if(subject->userIsReviewer() == true && usersToRefresh == true){
				std::map<std::string, int>().swap(usersMapping);
				auto users = subject->listUsers();
				for(auto it = users.begin(); it != users.end(); ++it){
					usersMapping[(*it).login] = (*it).id;
				}
				usersToRefresh = false;
			}
			
			auto lu = subject->lastUpdateTime();

			if(lastUpdate < lu){

				auto wa = subject->getAllAnnotationStatus();

				annotations = convert(wa);

				lastUpdate = lu;

				updateFilters(subject);
			}
		}else{
			usersToRefresh = true;
		}
	}

	const int getInEditionFilterIDX() const
	{
		return inEditionFilterIDX;
	}

	const int getInVerificationFilterIDX() const
	{
		return inVerificationFilterIDX;
	}

	const int getVerifiedFilterIDX() const
	{
		return verifiedFilterIDX;
	}

	virtual const AnnotationStatus annotationStatus(const std::string & user, const int trialID) const
	{
		AnnotationStatus ret;

		ret.status = webservices::xmlWsdl::AnnotationStatus::UnderConstruction;

		auto id = userID(user);

		auto it = annotations.find(id);

		if(it != annotations.end()){

			auto IT = it->second.find(trialID);
			if(IT != it->second.end()){

				ret = IT->second;
			}
		}

		return ret;
	}

	virtual void setAnnotationStatus(const std::string & user, const int trialID,
		const webservices::xmlWsdl::AnnotationStatus::Type status,
		const std::string & comment)
	{
		auto id = userID(user);

		auto lu = comm->lastUpdateTime();

		if(lastUpdate >= lu){
			lastUpdate = webservices::DateTime::now();
		}

		comm->setAnnotationStatus(id, trialID, status, comment);
		AnnotationStatus as;
		as.status = status;

		if(comm->userIsReviewer() == true){
			as.note = comment;
		}else{
			as.comment = comment;
		}
		
		annotations[id][trialID] = as;

		updateFilters(comm.get());
	}
									
    virtual void update( double deltaTime ) {} 								 
    virtual QWidget* getWidget() { return nullptr; } 						 
    virtual QWidgetList getPropertiesWidgets() { return QWidgetList(); }

private:

	const int userID(const std::string & user) const
	{
		int ret = -1;

		auto it = usersMapping.find(user);
		if(it != usersMapping.end()){
			ret = it->second;
		}else if(user == comm->currentUser()->name()){
			ret = comm->currentUser()->id();
		}

		return ret;
	}

	void updateFilters(const communication::ICommunicationDataSource * subject)
	{
		AnnotationStatusFilter::Identifiers inEdition;
		AnnotationStatusFilter::Identifiers inVerification;
		AnnotationStatusFilter::Identifiers verified;

		auto allTrialsID = subject->trialsIDs();

		if(subject->userIsReviewer() == false){
			//student
			if(annotations.empty() == false){
				for(auto it = annotations.begin()->second.begin();
					it != annotations.begin()->second.end(); ++it){

					switch(it->second.status){

					case webservices::xmlWsdl::AnnotationStatus::Approved:
						verified.insert(it->first);
						break;

					case webservices::xmlWsdl::AnnotationStatus::ReadyForReview:
						inVerification.insert(it->first);
						break;

					default:
						inEdition.insert(it->first);
						break;

					}
				}
			}
		}else{
			//lekarz
			//muszê przebudowaæ indeks na trialID -> userID + status
			AnnotationsMap byTrialAnnotations;

			for(auto it = annotations.begin();
				it != annotations.end(); ++it){

				for(auto IT = it->second.begin(); IT != it->second.end(); ++IT){

					byTrialAnnotations[IT->first][it->first] = IT->second;

				}
			}

			for(auto it = byTrialAnnotations.begin();
				it != byTrialAnnotations.end(); ++it){

				int verifiedC = 0;
				int inEditionC = 0;
				int waitingC = 0;

				for(auto IT = it->second.begin(); IT != it->second.end(); ++IT){
					
					switch(IT->second.status){

						case webservices::xmlWsdl::AnnotationStatus::Approved:
							++verifiedC;
							break;

						case webservices::xmlWsdl::AnnotationStatus::ReadyForReview:
							++waitingC;
							break;

						default:
							++inEditionC;
							break;
					}
				}

				if(inEditionC > 0){

					inEdition.insert(it->first);

				}else if(waitingC > 0){

					inVerification.insert(it->first);

				}else if(verifiedC > 0){

					verified.insert(it->first);

				}
			}
		}

		std::set<int> noEditingTrials(inVerification);
		noEditingTrials.insert(verified.begin(), verified.end());

		std::vector<int> result(allTrialsID.size());
		auto retIT = std::set_difference(allTrialsID.begin(), allTrialsID.end(), noEditingTrials.begin(), noEditingTrials.end(), result.begin());

		inEdition.insert(result.begin(), retIT);

		//aktualizuje filtry
		inEditionFilter->setIdentifiers(inEdition);
		inVerificationFilter->setIdentifiers(inVerification);
		verifiedFilter->setIdentifiers(verified);

		//odœwie¿am jeœli trzeba
		auto fID = subject->currentFilter();

		if(fID == inEditionFilterIDX || fID == inVerificationFilterIDX
			|| fID == verifiedFilterIDX){

				comm->refreshCurrentFilter();
		}
	}
    
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
	communication::ICommunicationDataSourcePtr comm;
	AnnotationStatusFilter * inEditionFilter;
	AnnotationStatusFilter * inVerificationFilter;
	AnnotationStatusFilter * verifiedFilter;
	std::map<std::string, int> usersMapping;
	bool usersToRefresh;

	int inEditionFilterIDX;
	int inVerificationFilterIDX;
	int verifiedFilterIDX;

    DicomPerspectivePtr perspective;
	webservices::DateTime lastUpdate;
	AnnotationsMap annotations;
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
