#ifndef _MODEL_EXPORT_FACADE_H
#define _MODEL_EXPORT_FACADE_H


class Facade
{
    FMesh*                  _pMesh;              // mesh
    Skeleton*               _pSkeleton;          // skeleton   // AddBone   // sceneNode to root nowego szkieletu, kt�ry dodajemy do tablicy. 
    GeometryPolygons*       _pPolygons;          // skin
    GeometryMesh*           _pGeometry;          // zapisujemy grupe poligon�w

    ModelExport*            _pModelExport;       // model rozpoczynaj�cy zapisywanianie, na samej gurze hierarchi - zarzadzal kolenoscia 
                                                 // Exportowania danych do odpowiednich plikow

    FModel*                 _pModel;             // obiekt zarzadzaj�cy wlasciwym zapisywaniem odpowiednich formatow do plik�w z pliku *.DAE
                                                 // wszelkie parsownia XML-�w zachodzi tutaj.
    FileChunk*              _pFileChunk;
    FCDSceneNode*           _pSkinJoint;

public:
    void saveSkin();
    void saveTexture();
    void saveModel();
    void saveMesh();
    void saveSkeleton();
    void saveBone();
    void saveAnimation();

private:

    void saveToFile();
};


#endif //_MODEL_EXPORT_FACADE_H