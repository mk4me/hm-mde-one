#include "C3DChartData.h"
#include <core/c3dParser.h>
#include <core/C3D_Data.h>
#include "Data.h"
#include "ChartPCH.h"

C3DChartData::C3DChartData(std::string c3dFile, int chanel){
	std::ifstream plik;


	if(!plik) {
		std::cout<<"blad przy otwarciu pliku";

	}else{
 
		c3dParser* parser = 0;  
		parser = new c3dParser();

		C3D_Data* c3d = parser->parseData(c3dFile);
		exportAnalogChannel(chanel,c3d);


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
}