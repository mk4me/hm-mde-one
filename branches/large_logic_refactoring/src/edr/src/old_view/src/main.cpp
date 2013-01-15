#include "CorePCH.h"
#include "ToolboxMain.h"
#include <core/src/AppInitializer.h>

int main(int argc, char *argv[])
{
	return core::AppInitializer::start<ToolboxMain>(argc, argv);
}

