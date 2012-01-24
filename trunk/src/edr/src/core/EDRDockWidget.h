/********************************************************************
    created:  2011/04/28
    created:  28:4:2011   16:34
    filename: EDRDockWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__EDRDOCKWIDGET_H__
#define HEADER_GUARD_CORE__EDRDOCKWIDGET_H__

#include <QtGui/QDockWidget>

//! Klasa rozszerza klasycznego QDockWidget o mo¿liwosæ prze³anczania trybu oddokowywania na przeciwny,
//! ustawaiania okna jako "niesmiertelnego" przy zamykaniu go oraz modyfikacji jego flag przy oddokowywaniu
class EDRDockWidget : public QDockWidget
{
    Q_OBJECT
        Q_PROPERTY(QString windowTitle READ windowTitle WRITE setWindowTitle NOTIFY windowTitleChanged);

public:
    explicit EDRDockWidget(const QString &title, QWidget *parent = 0, Qt::WindowFlags flags = 0);
    explicit EDRDockWidget(QWidget *parent = nullptr, Qt::WindowFlags flags = 0);

    ~EDRDockWidget();

signals:

    void windowTitleChanged(const QString & title);

public slots:

    //! Zmienia stan dokowania.
    void toggleFloating();

public:
    //// przykrywamy oryginaln¹ metodê ¿eby móc emitowaæ sygna³!
    void setWindowTitle(const QString & title);

    bool isPermanent() const;
    void setPermanent(bool permanent);
};

#endif  //  HEADER_GUARD_CORE__EDRDOCKWIDGET_H__

