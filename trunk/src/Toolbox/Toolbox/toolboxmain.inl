// TODO:  refactorng KONIECZNIE -  tymczasem dla testów, spróbowac zrobiæ jedna medote zarz¹cdaj¹ca tymi pozosta³ymi
inline void ToolboxMain::pointViewModel()
{
    CModelWithSkeleton* model;

    ObjectService* pScene = _pServiceManager->GetSystemServiceAs<ObjectService>(); 
    pScene->setPrimitiveModeFlag(osg::PrimitiveSet::POINTS);
    model = dynamic_cast<CModelWithSkeleton*>(pScene->getModel());  // zawsze mamy tylko jeden model.
    model->updateMesh();
}

inline void ToolboxMain::linesViewModel()
{
    _pScene->setPrimitiveModeFlag(osg::PrimitiveSet::LINES);
    _model->updateMesh();
}

inline void ToolboxMain::line_stripViewModel()
{
    _pScene->setPrimitiveModeFlag(osg::PrimitiveSet::LINE_STRIP);
    _model->updateMesh();
}

inline void ToolboxMain::line_loopViewModel()
{
    _pScene->setPrimitiveModeFlag(osg::PrimitiveSet::LINE_LOOP);
    _model->updateMesh();
}

inline void ToolboxMain::triangleViewModel()
{
    _pScene->setPrimitiveModeFlag(osg::PrimitiveSet::TRIANGLES);
    _model->updateMesh();
}

inline void ToolboxMain::triangle_stripViewModel()
{
    _pScene->setPrimitiveModeFlag(osg::PrimitiveSet::TRIANGLE_STRIP);
    _model->updateMesh();
}

inline void ToolboxMain::triangle_funViewModel()
{
    _pScene->setPrimitiveModeFlag(osg::PrimitiveSet::TRIANGLE_FAN);
    _model->updateMesh();
}

inline void ToolboxMain::quadsViewModel()
{
    _pScene->setPrimitiveModeFlag(osg::PrimitiveSet::QUADS);
    _model->updateMesh();
}

inline void ToolboxMain::quad_stripViewModel()
{
    _pScene->setPrimitiveModeFlag(osg::PrimitiveSet::QUAD_STRIP);
    _model->updateMesh();
}

inline void ToolboxMain::polygonViewModel()
{
    _pScene->setPrimitiveModeFlag(osg::PrimitiveSet::POLYGON);
    _model->updateMesh();
}