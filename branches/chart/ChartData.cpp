#include "ChartData.h"


ChartData::ChartData(std::string c3dFile, int chanel){
	std::ifstream plik;


	if(!plik) {
		std::cout<<"blad przy otwarciu pliku";

	}else{
 
		c3dParser* parser = 0;  
		parser = new c3dParser();

		C3D_Data* c3d = parser->parseData(c3dFile);
		exportAnalogChannel(chanel,c3d);

		ChartData::normalizedY=new float[rNumber];
		ChartData::normalizedX=new float[rNumber];
		ChartData::normalize();
	}

}

void ChartData::normalize(){
	ChartData::setMax();
	ChartData::setMin();
	ChartData::yScale=ChartData::yMax-ChartData::yMin;
	ChartData::xScale=ChartData::xMax-ChartData::xMin;
	for(int i=0;i<ChartData::rNumber;i++){
		ChartData::normalizedY[i]=(ChartData::y[i]-ChartData::yMin)/ChartData::yScale;
		ChartData::normalizedX[i]=(ChartData::x[i]-ChartData::xMin)/ChartData::xScale;
	}

}
void ChartData::setMin(){
	ChartData::xMin=ChartData::x[0];
	ChartData::yMin=ChartData::y[0];
	for(int i=0;i<ChartData::rNumber;i++){
		if(ChartData::x[i]<ChartData::xMin)
			ChartData::xMin=ChartData::x[i];
		if(ChartData::y[i]<ChartData::yMin)
			ChartData::yMin=ChartData::y[i];
	}

}

void ChartData::setMax(){
	ChartData::xMax=ChartData::x[0];
	ChartData::yMax=ChartData::y[0];
	for(int i=0;i<ChartData::rNumber;i++){
		if(ChartData::x[i]>ChartData::xMax)
			ChartData::xMax=ChartData::x[i];
		if(ChartData::y[i]>ChartData::yMax)
			ChartData::yMax=ChartData::y[i];
	}

}
float ChartData::getValue(int frame){
	return ChartData::y[frame];

}

float ChartData::getNormalizedYValue(int frame){
	return ChartData::normalizedY[frame];

}


float ChartData::getNormalizedXValue(int frame){
	return ChartData::normalizedX[frame];

}
float ChartData::getScaleX(){return ChartData::xScale;}
float ChartData::getScaleY(){return ChartData::yScale;}
int ChartData::getRNumber(){return ChartData::rNumber;}
void ChartData::setRNumber(std::string c3dFile){
	std::ifstream plik;
	plik.open(c3dFile.c_str(),std::ios::in);
	std::string line; 
	rNumber=0;
	while(getline(plik,line))
		rNumber++;
	plik.close();
}

float ChartData::getXMin(){
	return xMin;
}
float ChartData::getYMin(){
	return yMin;
}

void ChartData::exportAnalogChannel(int numberOfChannel, C3D_Data* c3dd)
{


	float* result = new float[c3dd->getData()->getNumberOfFrames() *
		c3dd->getHeader()->getNumberOfAnalogSamplesPerFrame()];
	int counter = 0;
	ChartData::rNumber=c3dd->getData()->getNumberOfFrames()*c3dd->getHeader()->getNumberOfAnalogSamplesPerFrame();
	ChartData::x=new float[rNumber];
	ChartData::y=new float[rNumber];
	FPS=c3dd->getHeader()->getNumberOfAnalogSamplesPerFrame()*c3dd->getHeader()->getFrameRate();
	for(int i=0; i < c3dd->getData()->getNumberOfFrames(); i++) // every frame
	{

		for(int j=0; j <
			c3dd->getHeader()->getNumberOfAnalogSamplesPerFrame(); j++) // every sample per frame
		{

			result[counter++] =
				c3dd->getData()->getAnalog()[i*c3dd->getHeader()->getNumberOfAnalogSamplesPerFrame()*c3dd->getData()->getAnalogChannelsNumber()
				+ j*c3dd->getData()->getAnalogChannelsNumber() + numberOfChannel];

			ChartData::x[counter-1]=counter;
			ChartData::y[counter-1]=result[counter-1];


		}

	}

}
int ChartData::getFPS(){
 return FPS;
}