/********************************************************************
	created:  2015/01/02	14:04:01
	filename: CostumeSkeletonMotionHelper.h
	author:	  Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef __HEADER_GUARD_IMU__COSTUMESKELETONMOTIONHELPER_H__
#define __HEADER_GUARD_IMU__COSTUMESKELETONMOTIONHELPER_H__

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <plugins/imuCostume/IIMUDataSource.h>

class QDialog;
class QWidget;
class QProgressDialog;

//! Obiekt realizuj�cy wizualny post�p inicjalizacji kostiumu
//! inicjalizacj� algorytm�w estyamcji orientacji dla czujnik�w oraz kalibracj�
//! Procedur� mo�na anulowa�
class CostumeSkeletonMotionHelper : public QObject
{
	Q_OBJECT

private:

	//Struktura opisuj�ca post�p inicjalizacji algorytmu
	struct AlgoProgress
	{
		//! Algorytm
		IMU::IIMUOrientationEstimationAlgorithmPtr algo;
		//! Ostatni czas
		uint32_t lastTime;
		//! Licznik
		unsigned int counter;
	};

public:
	//! \param sensorsStream Sturmie� danych sensor�w
	//! \param estimationAlgorithms [out] Algorytmy estymacji orientacji czujnik�w
	//! \param calibrationAlgorithm [out] Algorytm kalibrajci kostiumu
	//! \param maxSamples Maksymalna ilo�� pr�bek w ca�ej fazie inicjalizacji
	//! \param calibratinStageChangeValue Ilo�� pr�bek po kt�ych rozpoczynamy kalibracj� (algorytmy estymacji orientacji zosta�y zainicjowane)
	//! \param parent Obiekt rodzic
	CostumeSkeletonMotionHelper(IMU::SensorsStreamPtr sensorsStream,
		IMU::CostumeProfilePtr costumeProfile,
		const unsigned int maxSamples,
		const unsigned int calibratinStageChangeValue, QWidget * parent = nullptr);

	//! Destruktor wirtualny
	virtual ~CostumeSkeletonMotionHelper();

	//! Metoda blokuj�ca odpalaj�ca dialog z post�pem
	//! \return Warto�� zwr�cona przez dialog
	int exec();

	//! \return Czy kalibracja zako�czona sukcesem
	bool isComplete() const;

private slots:

	//! Cykliczne odpalana metoda czytaj�ca dane ze strumienia, je�li s�, i realizuj�ca inicjalizacj� kostiumu
	void perform();
	//! Metoda anuluj�ca inicjalizacj� kostiumu
	void cancel();

private:

	void estimate(const IMU::SensorsStreamData & streamData);
	
	//! Zwraca minimalny licznik inicjowanych algorytm�w
	unsigned int minCounter() const;
	//! Zeruje liczniki
	void resetCounters();

private:
	//! Strumie� danych z kostiumu
	IMU::SensorsStreamPtr sensorsStream;
	//! Obserwator strumienia kostiumu
	utils::shared_ptr<threadingUtils::ResetableStreamStatusObserver> observer;
	//! Profil wg kt�rego b�dziemy si� konfigurowa�
	IMU::CostumeProfilePtr costumeProfile;
	//! Mapa post�pu algorytm�w
	std::map<imuCostume::Costume::SensorID, AlgoProgress> algorithmsProgress;
	//! Numer pr�bki przy kt�rej przechodzimy do fazy kalibracji
	unsigned int calibratinStageChangeValue;
	//! Poprzednia pr�bka czasu
	unsigned int previousTime;
	//! Czy to pierwsza pr�bka
	bool first;
	//! Timer realizuj�cy odpytywanie strumienia z danych
	QTimer timer;
	//! Widget konfiguracji - instrukcje itp itd jak kalibrowa�
	QWidget * cw;
	//! Dialog z postepem inicjalizacji kostiumu
	QProgressDialog * pd;

	QDialog * dialog;
	//! Info czy zako�czono inicjalizacj�
	bool complete;
};

#endif	// __HEADER_GUARD_IMU__COSTUMESKELETONMOTIONHELPER_H__