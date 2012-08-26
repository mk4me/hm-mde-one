#include "hmmPCH.h"
#include "FilterCommand.h"
#include "HmmMainWindow.h"
#include "TreeBuilder.h"


using namespace core;

SimpleFilterCommand::SimpleFilterCommand( PluginSubject::DataFilterPtr dataFilter ) :
    dataFilter(dataFilter)
{
}
