
#include "ChartWidget.h"



ChartWidget::ChartWidget(ChartService* service)
:   service(service)
{
    setupUi(this);

	c3dFile="Przejscie1.c3d";
	//createEMGChanels();
	mainViewer=new ChartViewer(this,0,c3dFile);
	mainViewer->setMinimumSize(300,300);
	mainChart->addWidget(mainViewer);
}

ChartWidget::~ChartWidget()
{

}

void ChartWidget::createLargeChart(int chartIndex){
	mainViewer->changeView(chartIndex);
	//mainViewer=new ChartViewer(this,chartIndex,c3dFile);
	////mainViewer->setMinimumSize(300,300);
	//mainChart->addWidget(mainViewer);



}

void ChartWidget::createEMGChanels(){
	listViewer=new ChartViewer*[16];
	for(int i=0;i<16;i++){
		listViewer[i] = new ChartViewer(this,i+12,c3dFile);
		//createChart(listViewer[i],i+12);
		allChart->addWidget( listViewer[i] );
	}
}
void ChartWidget::createGRFChanels(){
	listViewer=new ChartViewer*[12];
	for(int i=0;i<12;i++){
		listViewer[i] = new ChartViewer(this,i,c3dFile);
		//createChart(listViewer[i],i);
		allChart->addWidget( listViewer[i] );
	}
	 
}

void ChartWidget::comboBoxChanged(int index){
	switch(index){
		case 0: {
			for(int i = 0; i < 12; i++) 
				delete[] listViewer[i]; 
				delete[] listViewer;
				createEMGChanels();
				break;
			 }
		case 1: {
			for(int i = 0; i < 16; i++) 
				delete[] listViewer[i]; 
				delete[] listViewer;
				createGRFChanels();
				break;
			 }
	}
	
}
ChartViewer* ChartWidget::getViewer(){
return mainViewer;
}