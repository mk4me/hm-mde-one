#include <DicomPCH.h>
#include "AnnotationStatusFilter.h"

using namespace dicom;

AnnotationStatusFilter::AnnotationStatusFilter(const std::string & name,
	const bool accept, const bool missingDefault,
	const Identifiers & identifiers)
	: name_(name), accept(accept), missingDefault(missingDefault),
	identifiers(identifiers)
{
}

AnnotationStatusFilter::~AnnotationStatusFilter()
{
}

std::string AnnotationStatusFilter::name() const
{
	return name_;
}

bool AnnotationStatusFilter::filterMotion(const hmdbServices::MotionShallowCopy::Trial* motion) const
{
	return (identifiers.find(motion->trialID) != identifiers.end()) ? accept : missingDefault;
}

void AnnotationStatusFilter::setIdentifiers(const Identifiers & identifiers)
{
	this->identifiers = identifiers;
}