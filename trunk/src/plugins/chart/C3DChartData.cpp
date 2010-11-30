#include "ChartPCH.h"

#include "C3DChartData.h"
#include <core/c3dParser.h>
#include <core/C3D_Data.h>
#include "Data.h"

C3DChartData::C3DChartData(std::string c3dFile, int chanel){
	std::ifstream plik(c3dFile.c_str());


	if(!plik) {
		std::cout<<"blad przy otwarciu pliku";

	}else{
 
		c3dParser* parser = 0;  
		parser = new c3dParser();

		C3D_Data* c3d = parser->parseData(c3dFile);
		exportAnalogChannel(chanel,c3d);

			setMax();
			setMin();
			setScaleY(getYMax()-getYMin());
			setScaleX(getXMax()-getXMin());
			normalize();
	}

}

void C3DChartData::exportAnalogChannel(int numberOfChannel, C3D_Data* c3dd)
{

float* x;
float* y;
	float* result = new float[c3dd->getData()->getNumberOfFrames() *
		c3dd->getHeader()->getNumberOfAnalogSamplesPerFrame()];
	int counter = 0;
	setRNumber(c3dd->getData()->getNumberOfFrames()*c3dd->getHeader()->getNumberOfAnalogSamplesPerFrame());
	x=new float[getRNumber()];
	y=new float[getRNumber()];
	setFPS(c3dd->getHeader()->getNumberOfAnalogSamplesPerFrame()*c3dd->getHeader()->getFrameRate());
	for(int i=0; i < c3dd->getData()->getNumberOfFrames(); i++) // every frame
	{

		for(int j=0; j <
			c3dd->getHeader()->getNumberOfAnalogSamplesPerFrame(); j++) // every sample per frame
		{

			result[counter++] =
				c3dd->getData()->getAnalog()[i*c3dd->getHeader()->getNumberOfAnalogSamplesPerFrame()*c3dd->getData()->getAnalogChannelsNumber()
				+ j*c3dd->getData()->getAnalogChannelsNumber() + numberOfChannel];

			x[counter-1]=counter;
			y[counter-1]=result[counter-1];


		}

	}
setX(x);
setY(y);
switch(numberOfChannel){
case 0:setName("Fx1"); break;
case 1:setName("Fy1"); break;
case 2:setName("Fz1"); break;
case 3:setName("Mx1"); break;
case 4:setName("My1"); break;
case 5:setName("Mz1"); break;
case 6:setName("Fx2"); break;
case 7:setName("Fy2"); break;
case 8:setName("Fz2"); break;
case 9:setName("Mx2"); break;
case 10:setName("My2"); break;
case 11:setName("Mz2"); break;
case 12:setName("EMG 1"); break;
case 13:setName("EMG 2"); break;
case 14:setName("EMG 3"); break;
case 15:setName("EMG 4"); break;
case 16:setName("EMG 5"); break;
case 17:setName("EMG 6"); break;
case 18:setName("EMG 7"); break;
case 19:setName("EMG 8"); break;
case 20:setName("EMG 9"); break;
case 21:setName("EMG 10"); break;
case 22:setName("EMG 11"); break;
case 23:setName("EMG 12"); break;
case 24:setName("EMG 13"); break;
case 25:setName("EMG 14"); break;
case 26:setName("EMG 15"); break;
case 27:setName("EMG 16"); break;
}
}