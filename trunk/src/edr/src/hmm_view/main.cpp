#include "AppInitializer.h"
#include "HmmMainWindow.h"
//#include "vld.h"
int main(int argc, char* argv[])
{
	return core::AppInitializer::start<HmmMainWindow>(argc, argv);
}
