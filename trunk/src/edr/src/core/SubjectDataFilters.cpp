#include "CorePCH.h"
#include "SubjectDataFilters.h"

using namespace PluginSubject;

TypeFilter::TypeFilter(const core::TypeInfo& type)
{
    types.push_back(type);
}

TypeFilter::TypeFilter(const std::vector<core::TypeInfo>& types) : types(types)
{

}

SessionPtr TypeFilter::doDataFiltering(const SessionConstPtr & session) const
{
    Motions motions;
    session->getMotions(motions);

    //Motions validMotions;
    std::map<MotionConstPtr, std::vector<core::ObjectWrapperConstPtr> > validMotions;

    for(auto it = motions.begin(); it != motions.end(); it++){
        bool valid = false;
        for(auto typeIT = types.begin(); typeIT != types.end(); typeIT++){
            if((*it)->hasObjectOfType(*typeIT) == true){
                std::vector<core::ObjectWrapperConstPtr> wrappers;
                (*it)->getWrappers(wrappers, *typeIT);

                validMotions[*it].insert(validMotions[*it].end(), wrappers.begin(), wrappers.end());
            }
        }
    }

    std::vector<MotionPtr> filteredMotions;
    
    for(auto it = validMotions.begin(); it != validMotions.end(); it++){
        std::set<core::ObjectWrapperConstPtr> swrappers(it->second.begin(), it->second.end());
        std::vector<core::ObjectWrapperConstPtr> wrappers(swrappers.begin(), swrappers.end());
        filteredMotions.push_back(createFilteredMotion(it->first, wrappers));
    }
    
    std::vector<core::ObjectWrapperConstPtr> sesWraps;
    session->getWrappers(sesWraps);

    return createFilteredSession(session, filteredMotions, sesWraps);
}

CustomFilter::CustomFilter(const std::vector<DataFilterPtr>& filters) : filters(filters)
{

}

SessionPtr CustomFilter::doDataFiltering(const SessionConstPtr & session) const
{
    SessionPtr temp;

    if(filters.empty() == false){
        auto it = filters.begin();

        temp = (*it)->filterData(session);

        while(++it != filters.end()){
            temp = (*it)->filterData(temp);
        }
    }

    return temp;
}