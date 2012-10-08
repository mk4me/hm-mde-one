#include "hmmPCH.h"
#include "AppInitializer.h"
#include "HmmMainWindow.h"
//#include "vld.h"
int main(int argc, char* argv[])
{
	//Q_INIT_RESOURCE(CoreIcons);
	return core::AppInitializer::start<HmmMainWindow>(argc, argv);
}
