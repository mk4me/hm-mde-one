/********************************************************************
    created:  2011/11/29
    created:  29:11:2011   10:23
    filename: PatientCardWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___PATIENTCARDWIDGET_H__
#define HEADER_GUARD___PATIENTCARDWIDGET_H__

#include <core/SmartPtr.h>
#include <QtGui/QWidget>
#include <QtGui/QStandardItemModel>
#include "ui_PatientCardWidget.h"
#include <plugins/communication/Entity.h>

class PatientCardWidget : public QWidget, public Ui::PatientCard
{
    Q_OBJECT;

public:

    typedef core::shared_ptr<QPixmap> PhotoPtr;
    typedef core::shared_ptr<const QPixmap> PhotoConstPtr;

    class IFilter
    {
    public:

        virtual ~IFilter() {};

        virtual bool check(const communication::MotionShallowCopy::Session * session) const = 0;
    };

    typedef core::shared_ptr<IFilter> IFilterPtr;
    typedef core::shared_ptr<const IFilter> IFilterConstPtr;

public:

    PatientCardWidget();
    PatientCardWidget(const communication::MedicalShallowCopy::Patient * patient, const communication::MotionShallowCopy::Performer * performer, const PhotoConstPtr & photo = PhotoConstPtr(), const IFilterConstPtr & filter = IFilterConstPtr());

    virtual ~PatientCardWidget();

    void setPatient(const communication::MedicalShallowCopy::Patient * patient, const communication::MotionShallowCopy::Performer * performer, const PhotoConstPtr & photo = PhotoConstPtr());
    void setFilter(const IFilterConstPtr & filter);
    void resetCard();
    void resetFilter();

    const communication::MedicalShallowCopy::Patient * getPatient() const;
    const communication::MotionShallowCopy::Performer * getPerformer() const;

    const PhotoConstPtr & getPhoto() const;
    const IFilterConstPtr & getFilter() const;

public slots:
    
    void refreshCard();

private slots:

    void setActiveSession(unsigned int id);
    void clearActiveSession();

private:

    void refreshDisorders();
    void refreshSessions();

    QTreeWidgetItem * createBranch(const std::string & name, const std::map<int, const communication::MotionShallowCopy::Session *> & sessions);

    void fillAntropometricData(const communication::MotionShallowCopy::PerformerConf * configuration);

    static void setAttribute(QTableWidget * table, int row, int column, const std::string & attribute, const communication::MotionShallowCopy::Attrs & attributes);

private:

    const communication::MedicalShallowCopy::Patient * patient;
    const communication::MotionShallowCopy::Performer * performer;

    IFilterConstPtr filter;
    PhotoConstPtr photo;
    QStandardItemModel * antropometricGeneralDataModel;
    QStandardItemModel * antropometricLeftRightDataModel;
};

#endif HEADER_GUARD___PATIENTCARDWIDGET_H__

