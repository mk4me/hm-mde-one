/********************************************************************
    created:  2012/12/17
    created:  17:12:2012   8:46
    filename: IDFFeatures.h
    author:   Mateusz Janiak
    
    purpose:  Zbi�r interfejs�w charakterystycznych dla przetwarzania
			  danych. U�ywamy ich na kilki poziomach przetwarzania.
*********************************************************************/
#ifndef HEADER_GUARD___IDFFEATURES_H__
#define HEADER_GUARD___IDFFEATURES_H__

namespace df{

	// Interfejs obiektu, kt�rego dzia�anie mo�na wstrzywa�/wznowi�
	class IDFPausable
	{
    public:
        virtual ~IDFPausable() {}

		//! Pauzuj obiekt
		virtual void pause() = 0;
		//! Wzn�w obiekt
		virtual void resume() = 0;
		//! \ret Czy obiekt jest wstrzymany
		virtual const bool paused() const = 0;
	};

	//! Interfejs obiektu do kt�rego mo�na si� do��czy� czekaj�c na zako�czenie jego dzia��nia
	class IDFJoinable
	{
    public:
        virtual ~IDFJoinable() {}

		//! Do��cz do biektu i czekaj na zako�czenie jego dzia��nia
		virtual void join() = 0;
	};

	//! Interfejs obiektu kt�rego przetwarzanie mo�na anulowa�
	class IDFCancelable
	{
    public:
        virtual ~IDFCancelable() {}

		//! Anuluj aktualne dzia�anie obiektu
		virtual void cancel() = 0;
	};

	//! Forward declaration
	class IDFLoggerHelper;

	//! Interfejs obiektu kt�ry chce mie� mozliwo�� logowania swojego stanu
	class IDFLoggable
	{
	public:        
        virtual ~IDFLoggable() {}

		//! Ustawia obiekt przez kt�y mo�na logowa�
		//! \param logger Obiekt przez kt�ry logujemy
		virtual void initLogger(IDFLoggerHelper * logger) = 0;
	};

	//! Interfejs obiektu kt�rego stan mo�na resetowa�
	class IDFResetable
	{
	public:
        virtual ~IDFResetable() {}

		//! Resetuj stan obiektu do inicjalnego
		virtual void reset() = 0;
	};

}

#endif	//	HEADER_GUARD___IDFFEATURES_H__
