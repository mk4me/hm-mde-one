#ifndef __HEADER_GUARD_KINEMATIC__KINEMATICPARSER_H__
#define __HEADER_GUARD_KINEMATIC__KINEMATICPARSER_H__

#include <core/IParser.h>

class KinematicParser : public core::IParser
{
    UNIQUE_ID('KINE','PARS');
private:
    //! Obiekt wewnêtrzny.
    core::ObjectWrapperPtr model;
public:
    KinematicParser();
    virtual ~KinematicParser();
// IParser
public:
    virtual void parseFile(const boost::filesystem::path& path);
    virtual core::IParser* create();
    virtual std::string getSupportedExtensions() const;
    virtual void getObjects(std::vector<core::ObjectWrapperPtr>& objects);
};
#endif  // __HEADER_GUARD_KINEMATIC__KINEMATICPARSER_H__