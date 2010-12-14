#include "ChartPCH.h"

#include "C3DChartData.h"

#include "Data.h"

C3DChartData::C3DChartData(C3D_Data* c3d, int chanel){

	exportAnalogChannel(chanel,c3d);



}
C3DChartData::~C3DChartData(){

}
void C3DChartData::exportAnalogChannel(int numberOfChannel, C3D_Data* c3dd)
{

	std::vector<float> xData;
	std::vector<float> yData;
	float* result = new float[c3dd->getData()->getNumberOfFrames() *
		c3dd->getHeader()->getNumberOfAnalogSamplesPerFrame()];
	int counter = 0;
	setRNumber(c3dd->getData()->getNumberOfFrames()*c3dd->getHeader()->getNumberOfAnalogSamplesPerFrame());
	if(getRNumber()<=0)
	{
		throw std::runtime_error("plik c3d nie posiada wymaganych przez wykresy danych");
	}
	
		
		setFPS(c3dd->getHeader()->getNumberOfAnalogSamplesPerFrame()*c3dd->getHeader()->getFrameRate());
		for(int i=0; i < c3dd->getData()->getNumberOfFrames(); i++) // every frame
		{

			for(int j=0; j <
				c3dd->getHeader()->getNumberOfAnalogSamplesPerFrame(); j++) // every sample per frame
			{

				result[counter++] =
					c3dd->getData()->getAnalog()[i*c3dd->getHeader()->getNumberOfAnalogSamplesPerFrame()*c3dd->getData()->getAnalogChannelsNumber()
					+ j*c3dd->getData()->getAnalogChannelsNumber() + numberOfChannel];

				xData.push_back(counter);
				yData.push_back(result[counter-1]);


			}

		}
		GroupData* gData = c3dd->getParameters()->getGroupsByName()["ANALOG"];


		setName(gData->getParameterByName()["LABELS"]->getDataAsStringsArray()[numberOfChannel]);
		setScale(gData->getParameterByName()["SCALE"]->getDataAsFloatArray()[numberOfChannel]);
		setOffset(gData->getParameterByName()["OFFSET"]->getDataAsFloatArray()[numberOfChannel]);
		setDescription(gData->getParameterByName()["DESCRIPTIONS"]->getDataAsStringsArray()[numberOfChannel]);
		setUnit(gData->getParameterByName()["UNITS"]->getDataAsStringsArray()[numberOfChannel]);


		setX(xData);
		setY(yData);

		setMax();
		setMin();
		setScaleY(getYMax()-getYMin());
		setScaleX(getXMax()-getXMin());
		normalize();
	
}