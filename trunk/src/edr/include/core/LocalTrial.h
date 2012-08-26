/**
@author Marek Daniluk
@brief Klasa LocalTrial reprezentuje probe pomiarowa lokalnie na dysku uzytkownika.
*/

#ifndef _LOCALTRIAL_H_
#define _LOCALTRIAL_H_

class LocalTrial
{
public:
    LocalTrial()
    {
    };

    void setC3dPath(const std::string& path)
    {
        c3dPath = path;
    };

    void setSkeletonPath(const std::string& path)
    {
        skeletonPath = path;
    };

    void setAnimationsPaths(const std::vector<std::string>& paths)
    {
        animationsPaths = paths;
    };

    void setVideosPaths(const std::vector<std::string>& paths)
    {
        videosPaths= paths;
    };

    void setName(const std::string& name)
    {
        this->name = name;
    };

    void setTrialPath(const std::string& path)
    {
        trialPath = path;
    };

    const std::string& getC3dPath() const
    {
        return c3dPath;
    };

    const std::string& getSkeletonPath() const
    {
        return skeletonPath;
    };

    const std::vector<std::string>& getAnimationsPaths() const
    {
        return animationsPaths;
    };

    const std::vector<std::string>& getVideosPaths() const
    {
        return videosPaths;
    };

    const std::string& getName() const
    {
        return name;
    };

    const std::string& getTrialPath() const
    {
        return trialPath;
    };

    bool isC3d() const
    {
        return !c3dPath.empty();
    };

    bool isSkeleton() const
    {
        return !skeletonPath.empty();
    };

    bool isAnimations() const
    {
        return !animationsPaths.empty();
    };

    bool isVideos() const
    {
        return !videosPaths.empty();
    };

private:
    std::string c3dPath;
    //std::string animationPath;
    std::string skeletonPath;
    std::vector<std::string> animationsPaths;
    std::vector<std::string> videosPaths;
    std::string name;
    std::string trialPath;
};
#endif
