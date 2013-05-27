#include "HmmLibPCH.h"
#include "HmmService.h"
#include <corelib/FilterBundle.h>
#include <plugins/subject/SubjectDataFilters.h>
#include <plugins/c3d/C3DCollections.h>
#include "FilterCommand.h"

core::IFilterProvider::FilterBundles HmmService::getFilterBundles() const
{
    core::IFilterProvider::FilterBundles bundles;
    core::IFilterBundlePtr fb(new core::FilterBundle());
    SubjectHierarchyFilterPtr filter(new SubjectHierarchyTypeFilter(typeid(GRFCollection)));
    core::IFilterCommandPtr command(new SimpleFilterCommand(filter));
    fb->addFilterCommand(command);
    bundles.push_back(fb);
    return bundles;
}
