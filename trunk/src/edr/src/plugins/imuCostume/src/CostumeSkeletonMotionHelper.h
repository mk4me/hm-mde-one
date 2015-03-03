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

class QProgressDialog;

//! Obiekt realizuj¹cy wizualny postêp inicjalizacji kostiumu
//! inicjalizacjê algorytmów estyamcji orientacji dla czujników oraz kalibracjê
//! Procedurê mo¿na anulowaæ
class CostumeSkeletonMotionHelper : public QObject
{
	Q_OBJECT

private:

	struct AlgoProgress
	{
		IMU::IIMUOrientationEstimationAlgorithmPtr algo;
		uint32_t lastTime;
		unsigned int counter;
	};

public:
	//! \param sensorsStream Sturmieñ danych sensorów
	//! \param estimationAlgorithms [out] Algorytmy estymacji orientacji czujników
	//! \param calibrationAlgorithm [out] Algorytm kalibrajci kostiumu
	//! \param maxSamples Maksymalna iloœæ próbek w ca³ej fazie inicjalizacji
	//! \param calibratinStageChangeValue Iloœæ próbek po któych rozpoczynamy kalibracjê (algorytmy estymacji orientacji zosta³y zainicjowane)
	//! \param parent Obiekt rodzic
	CostumeSkeletonMotionHelper(IMU::SensorsStreamPtr sensorsStream,
		IMU::CostumeProfileInstance * costumeProfile,
		const unsigned int maxSamples,
		const unsigned int calibratinStageChangeValue, QWidget * parent = nullptr);

	//! Destruktor wirtualny
	virtual ~CostumeSkeletonMotionHelper();

	//! Metoda blokuj¹ca odpalaj¹ca dialog z postêpem
	//! \return Wartoœæ zwrócona przez dialog
	int exec();

	//! \return Czy kalibracja zakoñczona sukcesem
	bool isComplete() const;

private slots:

	//! Cykliczne odpalana metoda czytaj¹ca dane ze strumienia, jeœli s¹, i realizuj¹ca inicjalizacjê kostiumu
	void perform();
	//! Metoda anuluj¹ca inicjalizacjê kostiumu
	void cancel();

private:

	static void estimate(std::map<imuCostume::Costume::SensorID, AlgoProgress> & algos,
		const IMU::SensorsData & sensorsData, const uint32_t currentTime);

	unsigned int minCounter() const;

	void resetCounters();

private:
	//! Strumieñ danych z kostiumu
	IMU::SensorsStreamPtr sensorsStream;
	//! Obserwator strumienia kostiumu
	utils::shared_ptr<threadingUtils::ResetableStreamStatusObserver> observer;
	//! Profil wg którego bêdziemy siê konfigurowaæ
	IMU::CostumeProfileInstance * costumeProfile;

	std::map<imuCostume::Costume::SensorID, AlgoProgress> algorithmsProgress;

	//! Numer próbki przy której przechodzimy do fazy kalibracji
	unsigned int calibratinStageChangeValue;
	//! Poprzednia próbka czasu
	unsigned int previousTime;
	//! Czy to pierwsza próbka
	bool first;
	//! Timer realizuj¹cy odpytywanie strumienia z danych
	QTimer timer;
	//! Dialog z postepem inicjalizacji kostiumu
	QProgressDialog * pd;
	//!
	bool complete;
};

#endif	// __HEADER_GUARD_IMU__COSTUMESKELETONMOTIONHELPER_H__