/********************************************************************
created:  2010/11/22
filename: C3DChartData.h
author:	  Michal Szafarski

purpose:  Klasa ta sluzy do pobierania danych z pliku c3d
*********************************************************************/
#ifndef __HEADER_GUARD__C3DCHARTDATA_H__
#define __HEADER_GUARD__C3DCHARTDATA_H__

#include "ChartData.h"

class C3DChartData : public ChartData{
private:
	void exportAnalogChannel(int numberOfChannel, C3D_Data* c3dd);

public:
	//konstruktor 
	C3DChartData(std::string c3dFile, int chanel);


};


#endif