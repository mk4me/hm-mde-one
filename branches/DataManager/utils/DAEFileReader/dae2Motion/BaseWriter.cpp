#include "stdafx.h"
#include "BaseWriter.h"
#include "DAE2MotionWriter.h"


//------------------------------
CBaseWriter::CBaseWriter( CDAE2MotionWriter* dea2CubeITMWriter )
: m_DEA2MotionWriter(dea2CubeITMWriter)
{
}

//------------------------------
CBaseWriter::~CBaseWriter()
{
}

//------------------------------
//void CDEA2CubeITMBaseWriter::reportError( const String& method, const String& message, OgreWriter::Severity severity )
//{
//    OgreWriter::reportError(method, message, severity);
//}

//------------------------------
const CDAE2MotionWriter::GeometryUniqueIdInstanceGeometryInfoMap& CBaseWriter::getGeometryUniqueIdInstanceGeometryInfoMap()
{
    return m_DEA2MotionWriter->getGeometryUniqueIdInstanceGeometryInfoMap();
}

const CDAE2MotionWriter::LightUniqueIdInstanceLightInfoMap& CBaseWriter::getLightUniqueIdInstanceGeometryInfoMap()
{
    return m_DEA2MotionWriter->getLightUniqueIdInstanceLightInfoMap();
}

const CDAE2MotionWriter::CameraUniqueIdInstanceCameraInfoMap& CBaseWriter::getCameraUniqueIdInstanceGeometryInfoMap()
{
    return m_DEA2MotionWriter->getCameraUniqueIdInstanceCameraInfoMap();
}

//------------------------------
void CBaseWriter::addGeometryUniqueIdInstanceGeometryInfoPair( const COLLADAFW::UniqueId& geoId, const CDAE2MotionWriter::InstanceGeometryInfo& instanceGeoInfo )
{
    CDAE2MotionWriter::GeometryUniqueIdInstanceGeometryInfoMap& map = m_DEA2MotionWriter->getGeometryUniqueIdInstanceGeometryInfoMap();
    map.insert(std::make_pair(geoId, instanceGeoInfo));
}

void CBaseWriter::addLightUniqueIdInstanceLightInfoPair(const COLLADAFW::UniqueId& geoId, const CDAE2MotionWriter::InstanceLightInfo& instanceLightInfo)
{
    CDAE2MotionWriter::LightUniqueIdInstanceLightInfoMap& map = m_DEA2MotionWriter->getLightUniqueIdInstanceLightInfoMap();
    map.insert(std::make_pair(geoId, instanceLightInfo));
}

void CBaseWriter::addCameraUniqueIdInstanceCameraInfoPair(const COLLADAFW::UniqueId& geoId, const CDAE2MotionWriter::InstanceCameraInfo& instanceCameraInfo)
{
    CDAE2MotionWriter::CameraUniqueIdInstanceCameraInfoMap& map = m_DEA2MotionWriter->getCameraUniqueIdInstanceCameraInfoMap();
    map.insert(std::make_pair(geoId, instanceCameraInfo));
}


