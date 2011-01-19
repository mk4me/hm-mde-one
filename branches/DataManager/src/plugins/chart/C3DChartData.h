/********************************************************************
created:  2010/11/22
filename: C3DChartData.h
author:	  Michal Szafarski

purpose:  Klasa ta sluzy do pobierania danych z pliku c3d
*********************************************************************/
#ifndef __HEADER_GUARD__C3DCHARTDATA_H__
#define __HEADER_GUARD__C3DCHARTDATA_H__

#include <core/ChartData.h>
#include <core/C3D_Data.h>
class C3DChartData : public ChartData{
private:
	void exportAnalogChannel(int numberOfChannel, C3D_Data* c3dd);

public:
	//konstruktor 
	C3DChartData(C3D_Data* c3d, int chanel);
~C3DChartData();

};


#endif