/********************************************************************
	created:	2013/06/03
	created:	3:6:2013   12:08
	filename: 	ReportsThumbnailContext.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_COREUI__REPORTSTHUMBNAILCONTEXT_H__
#define HEADER_GUARD_COREUI__REPORTSTHUMBNAILCONTEXT_H__

class QPlainTextEdit;
class QComboBox;
class QTabWidget;

#include <QtCore/QObject>
#include <coreui/IAppUsageContext.h>
#include <coreui/Export.h>

namespace coreUI {
//! kontekst dla zak�adki z miniaturkami obraz�w do raportu
class COREUI_EXPORT ReportsThumbnailContext : public QObject, public coreUI::IAppUsageContext
{
    Q_OBJECT;
public:
    //! konstruktor
    //! \param flexiTabWidget flexi widget kontekstu
    //! \param hmm g��wne okno aplikacji
    ReportsThumbnailContext(QTabWidget * flexiTabWidget, QWidget* reportsTab);
public:
    //! wywo�ywane, po aktywowaniu si� kontekstu, do flexi bara trafi� akcje zwi�zane z tworzeniem raport�w
    //! \param contextWidget wiget, kt�ry wywo�a� kontekst
    virtual void activateContext(QWidget * contextWidget);
    //! wywo�ywane, gdy kontekst przestaje by� aktywny
    //! z flexibara znikaj� akcje zwi�zane z tworzeniem raport�w
    virtual void deactivateContext(QWidget * nextContextWidget, bool refresh);
    //! rejestracja widgeta z miniaturkami raport�w
    virtual void onRegisterContextWidget(QWidget * contextWidget);
    //! wyrejstrowanie widgeta z miniaturkami raport�w
    virtual void onUnregisterContextWidget(QWidget * contextWidget);

public:
    //! Utworzenie raportu na podstawie szablonu i stylu, z do��czonymi obrazami
    static QString createRaport(const QWidget* thumbs, const QString& projectName, const QString& templateReport = QString(), const QString& css = QString());

Q_SIGNALS:
    //! Sygna� generowany, gdy u�ytkownik klikn�� przycisk 'utw�rz raport'
    //! \param str Wygenerowany raport (w HTML)
    void reportCreated(const QString& str);

private:
    //! flexi widget
    QTabWidget * flexiTabWidget;
    //! sekcja obs�ugiwanego flexi widgeta
    QWidget* flexiSection;
    //! identyfikator grupy dla miniatur raport�w
    int groupID;
    //! nazwa tworzonego raportu
    QPlainTextEdit* projectName;
    //! kombo box, z szablonami raport�w
    QComboBox* projectTemplate;
    //! kombo box ze stylami raport�w
    QComboBox* cssCombo;
    //! miejsce, gdzie trafiaja miniaturki
    QWidget* reportsTab;
};
DEFINE_SMART_POINTERS(ReportsThumbnailContext);
}
#endif