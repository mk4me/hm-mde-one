#include "SubjectPCH.h"
#include <plugins/subject/Subject.h>
#include <plugins/subject/Motion.h>
#include <core/PluginCommon.h>
#include <plugins/kinematic/Wrappers.h>

Motion::Motion()
{
	fillDefaultTypesList();
}

Motion::Motion( const std::vector<core::TypeInfo>& supportedTypes )
{
	types = supportedTypes;
}

//void Motion::createJointCollection( kinematic::SkeletalModelConstPtr model, kinematic::SkeletalDataConstPtr data )
//{
//	kinematic::JointAnglesCollectionPtr j(new kinematic::JointAnglesCollection());
//	j->setSkeletal(model, data);
//	this->joints = j;
//	core::ObjectWrapperPtr wrapper = core::ObjectWrapper::create<kinematic::JointAnglesCollection>();
//	wrapper->set(j);
//	wrapper->setName("Skeleton");
//	// hack
//	wrapper->setSource("C:\\source_set_in_wrong_way.hack");
//	core::getDataManager()->addExternalData(wrapper);
//}

void Motion::fillDefaultTypesList()
{
	types.clear();
	types.push_back(typeid(GRFCollection));
    types.push_back(typeid(GRFChannel));
	types.push_back(typeid(EMGCollection));
    types.push_back(typeid(EMGChannel));
	types.push_back(typeid(kinematic::JointAnglesCollection));
	types.push_back(typeid(kinematic::SkeletalData));
	types.push_back(typeid(kinematic::SkeletalModel));
	types.push_back(typeid(MarkerCollection));
	types.push_back(typeid(ForceCollection));
	types.push_back(typeid(AngleCollection));
	types.push_back(typeid(PowerCollection));
	types.push_back(typeid(MomentCollection));
    types.push_back(typeid(VectorChannel));
	types.push_back(typeid(C3DEventsCollection));
    types.push_back(typeid(VideoChannel));
}

void Motion::addWrapper( const core::ObjectWrapperPtr& wrapper )
{
    if (!isSupported(wrapper->getTypeInfo())) {
        throw std::runtime_error("type is not supported");
    }
    wrappers.push_back(wrapper);
}

bool Motion::isSupported( const core::TypeInfo& typeToCheck )
{
    for (auto it = types.begin(); it != types.end(); it++) {
        if (*it == typeToCheck) {
            return true;
        }
    }
    return false;
}

//bool Motion::hasObjectOfType( const core::TypeInfo& type, bool exact /*= false*/ )
//{
//	core::IDataManager* manager = core::getDataManager();
//	for (auto it = wrappers.begin(); it != wrappers.end(); it++) {
//		if ((*it)->isSupported(type)) {
//			return true;
//		}
//	}
//
//	return false;
//}



