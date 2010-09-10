#ifndef __DEA2CubeITMBaseWriter_H__
#define __DEA2CubeITMBaseWriter_H__

#include "DAE2MotionWriter.h"

class CBaseWriter 	
{
private:
    CDAE2MotionWriter* m_DEA2MotionWriter;

public:
    // Konstruktor i destruktor
    CBaseWriter( CDAE2MotionWriter* dea2CubeITMWriter);
    virtual ~CBaseWriter();

    const CDAE2MotionWriter::GeometryUniqueIdInstanceGeometryInfoMap& getGeometryUniqueIdInstanceGeometryInfoMap();

    const CDAE2MotionWriter::LightUniqueIdInstanceLightInfoMap& getLightUniqueIdInstanceGeometryInfoMap();

    const CDAE2MotionWriter::CameraUniqueIdInstanceCameraInfoMap& getCameraUniqueIdInstanceGeometryInfoMap();

    void addGeometryUniqueIdInstanceGeometryInfoPair(const COLLADAFW::UniqueId& geoId, const CDAE2MotionWriter::InstanceGeometryInfo& instanceGeoInfo);

    void addLightUniqueIdInstanceLightInfoPair(const COLLADAFW::UniqueId& geoId, const CDAE2MotionWriter::InstanceLightInfo& instanceLightInfo);

    void addCameraUniqueIdInstanceCameraInfoPair(const COLLADAFW::UniqueId& geoId, const CDAE2MotionWriter::InstanceCameraInfo& instanceCameraInfo);

    const CDAE2MotionWriter::UniqueIdFWMaterialMap& getUniqueIdFWMaterialMap() { return m_DEA2MotionWriter->getUniqueIdFWMaterialMap(); }

    const CDAE2MotionWriter::UniqueIdFWImageMap& getUniqueIdFWImageMap() { return m_DEA2MotionWriter->getUniqueIdFWImageMap(); }

    const CDAE2MotionWriter::UniqueIdFWEffectMap& getUniqueIdFWEffectMap() { return m_DEA2MotionWriter->getUniqueIdFWEffectMap(); }

    const COLLADABU::URI& getInputFile() const { return m_DEA2MotionWriter->getInputFile(); }

    // TODO usunac
    const COLLADABU::URI& getOutputFile() const { return m_DEA2MotionWriter->getOutputFile(); }
    // TODO zast¹pic
    // static void reportError(const String& method, const String& message, OgreWriter::Severity severity);

private:

    // Blokada konstruktora kopiuj¹cego i operatora przypisania
    CBaseWriter( const CBaseWriter& pre );
    const CBaseWriter& operator= ( const CBaseWriter& pre );
};

#endif // __DEA2CubeITMBaseWriter_H__