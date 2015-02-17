#include "PCH.h"
#include "SkeletonStateStreamSerie.h"
#include <osgutils/OsgSchemeDrawer.h>
#include "TrajectoriesDrawer.h"
#include "SkeletalVisualizationSchemeHelper.h"
#include <threadingUtils/StreamData.h>



class SkeletonStreamStateSerieUpdater : public threadingUtils::IStreamStatusObserver
{
public:
	SkeletonStreamStateSerieUpdater(SkeletonStateStreamSerie * serie)
		: serie(serie)
	{
	}

	virtual ~SkeletonStreamStateSerieUpdater() {}

	//! Metoda wołana kiedy faktycznie stan strumienia się zmieni
	virtual void update()
	{
		serie->update();
	}

private:

	SkeletonStateStreamSerie * serie;
};

SkeletonStateStreamSerie::SkeletonStateStreamSerie(KinematicVisualizer * visualizer,	const utils::TypeInfo & requestedType,const core::VariantConstPtr & data) :
	visualizer(visualizer),	data(data), requestedType(requestedType),
	AbstractSkeletonStreamSerie(visualizer, requestedType, data),
	name("SkeletonData")
{
	UTILS_ASSERT(data->data()->getTypeInfo() == typeid(SkeletonWithStreamData));
	data->getMetadata("core/name", name);
	skeletalData = data->get();
	auto skeletonState = utils::make_shared<kinematic::SkeletonState>(kinematic::SkeletonState::create(*skeletalData->skeleton));

	auto ratio = 1.0;
	int pointsCount = skeletalData->nodesMapping.size();
	AbstractSkeletonSerie::init(ratio, pointsCount, skeletonState, skeletalData->nodesMapping);

	updater.reset(new SkeletonStreamStateSerieUpdater(this));
	skeletalData->states->attachObserver(updater);
}

SkeletonStateStreamSerie::~SkeletonStateStreamSerie()
{
	if (skeletalData != nullptr){
		skeletalData->states->detachObserver(updater);
	}
}


void SkeletonStateStreamSerie::setName(const std::string & name)
{
	this->name = name;
}

const std::string SkeletonStateStreamSerie::getName() const
{
	return name;
}

const core::VariantConstPtr & SkeletonStateStreamSerie::getData() const
{
	return data;
}

void SkeletonStateStreamSerie::update()
{
	AbstractSkeletonSerie::update();
}

const utils::TypeInfo & SkeletonStateStreamSerie::getRequestedDataType() const
{
	return requestedType;
}

const osg::Vec3 SkeletonStateStreamSerie::pivotPoint() const
{
	return rootPosition; // * lToW;
}

const kinematic::SkeletonState::NonRigidCompleteStateChange& SkeletonStateStreamSerie::getStateChange()
{
	skeletalData->states->data(currentStateChange);
	return currentStateChange;
}
