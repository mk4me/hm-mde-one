#include <dfmlib/DFInterface.h>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

DFInterface::DFInterface(bool enable) : objectEnable(enable)
{
}


DFInterface::~DFInterface(void)
{
}

bool DFInterface::setEnable(bool enable){
    if(objectEnable == enable){
        return false;
    }

    objectEnable = enable;

    onEnableChange();

    return true;
}

bool DFInterface::isEnable() const{
	return objectEnable;
}

void DFInterface::reset(){
}

void DFInterface::onEnableChange(){

}

void DFInterface::notify(){
	if(objectEnable == true){
		process();
	}
}

}