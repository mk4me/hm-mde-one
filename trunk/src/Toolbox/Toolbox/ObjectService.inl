//--------------------------------------------------------------------------------------------------
inline void ObjectService::addVieModelFlag( MODEL_FLAG::VIEW flag )
{
    if(_viewModelFlag & flag)
        removeVieModelFlag(flag);
    else
        _viewModelFlag |= flag;
}

//--------------------------------------------------------------------------------------------------
inline void ObjectService::removeVieModelFlag( MODEL_FLAG::VIEW flag )
{
    _viewModelFlag &= ~flag;
}

//--------------------------------------------------------------------------------------------------
inline int ObjectService::getViewModelFlag()
{
    if(_viewModelFlag == NULL)
        return MODEL_FLAG::NONE;
    return _viewModelFlag;
}

//--------------------------------------------------------------------------------------------------
inline void ObjectService::setPrimitiveModeFlag( osg::PrimitiveSet::Mode flag )
{
    _primitiveModeFlag = flag;
    _viewModelFlag |= MODEL_FLAG::MODEL;
}

//--------------------------------------------------------------------------------------------------
inline int ObjectService::getPrimitiveModeFlag()
{
    return _primitiveModeFlag;
}

//--------------------------------------------------------------------------------------------------
inline void ObjectService::setViewModelFlag(int flag )
{
    _viewModelFlag = flag;
}