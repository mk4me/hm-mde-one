#include "SubjectPCH.h"
#include <plugins/subject/Subject.h>
#include <plugins/subject/Motion.h>
#include <core/PluginCommon.h>
#include <plugins/kinematic/Wrappers.h>

void Motion::createJointCollection( kinematic::SkeletalModelConstPtr model, kinematic::SkeletalDataConstPtr data )
{
	kinematic::JointAnglesCollectionPtr j(new kinematic::JointAnglesCollection());
	j->setSkeletal(model, data);
	this->joints = j;
	core::ObjectWrapperPtr wrapper = core::ObjectWrapper::create<kinematic::JointAnglesCollection>();
	wrapper->set(j);
	wrapper->setName("Skeleton");
	// hack
	wrapper->setSource("C:\\source_set_in_wrong_way.hack");
	core::getDataManager()->addExternalData(wrapper);
}
