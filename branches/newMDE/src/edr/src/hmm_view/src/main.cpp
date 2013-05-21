#include "hmmPCH.h"
#include <core/AppInitializer.h>
#include "HmmMainWindow.h"

int main(int argc, char* argv[])
{
	return core::AppInitializer::runApp<HmmMainWindow>(argc, argv);
}
