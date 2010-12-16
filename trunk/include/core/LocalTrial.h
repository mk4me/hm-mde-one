/**
@author Marek Daniluk
@brief Klasa LocalTrial reprezentuje probe pomiarowa lokalnie na dysku uzytkownika.
*/

#ifndef _LOCALTRIAL_H_
#define _LOCALTRIAL_H_

struct LocalTrial
{
	LocalTrial()
	{
		c3dPath = "";
		animationPath = "";
		skeletonPath = "";
		videosPaths.clear();
		name = "";
		trialPath = "";
	};
	std::string c3dPath;
	std::string animationPath;
	std::string skeletonPath;
	std::vector<std::string> videosPaths;
	std::string name;
	std::string trialPath;
};
#endif
