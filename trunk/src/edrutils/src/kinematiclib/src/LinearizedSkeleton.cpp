#include <kinematiclib/LinearizedSkeleton.h>

using namespace kinematic;

struct NameKeyExtractor
{
	std::string operator()(const Skeleton::JointData & jointData) const
	{
		return jointData.name();
	}
};


LinearizedSkeleton::Order LinearizedSkeleton::createCompleteOrder(const Skeleton & skeleton)
{
	return Visitor::createCompleteOrderKey(skeleton, NameKeyExtractor());
}

LinearizedSkeleton::Order LinearizedSkeleton::createNonLeafOrder(const Skeleton & skeleton)
{
	return Visitor::createNonLeafOrderKey(skeleton, NameKeyExtractor());
}

LinearizedSkeleton::Mapping LinearizedSkeleton::createCompleteMapping(const Skeleton & skeleton)
{
	return Visitor::createCompleteMappingKey(skeleton, NameKeyExtractor());
}

LinearizedSkeleton::Mapping LinearizedSkeleton::createNonLeafMapping(const Skeleton & skeleton)
{
	return Visitor::createNonLeafMappingKey(skeleton, NameKeyExtractor());
}