#include "ConfigurationService.h"
using namespace std;

int main() {
	ConfigurationService* pConfig = new ConfigurationService();
	
	pConfig->loadConfiguration("test.txt");
	delete pConfig;
	return 0;
}
