#include "hmmPCH.h"
#include "AppInitializer.h"
#include "HmmMainWindow.h"
//#include "vld.h"
int main(int argc, char* argv[])
{
	return AppInitializer::start<HmmMainWindow>(argc, argv);
}
