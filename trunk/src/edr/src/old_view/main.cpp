#include "CorePCH.h"
#include "ToolboxMain.h"
#include "AppInitializer.h"

int main(int argc, char *argv[])
{
	AppInitializer::start<ToolboxMain>(argc, argv);
}

