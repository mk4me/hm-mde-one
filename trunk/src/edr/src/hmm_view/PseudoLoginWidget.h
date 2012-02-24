/********************************************************************
    created:  2012/02/23
    created:  23:2:2012   10:26
    filename: PseudoLoginWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___PSEUDOLOGINWIDGET_H__
#define HEADER_GUARD___PSEUDOLOGINWIDGET_H__

#include "ui_PseudoLoginWidget.h"
#include <QtGui/QValidator>
#include <QtCore/QTimer>
#include <QtGui/QProgressBar>
#include <QtGui/QMessageBox>
#include <QtGui/QDesktopWidget>

class EmailValidator : public QValidator
{
public:
    EmailValidator() {}
    ~EmailValidator() {}

    virtual State validate(QString &input, int &pos) const
    {
        static const QRegExp emailRegex("\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\b", Qt::CaseInsensitive);

        if(input.isEmpty() == true){
            return Invalid;
        }

        if (emailRegex.exactMatch(input)) {
            return Acceptable;
        }

        return Invalid;
    }
};

class PseudoLoginWidget : public QDialog, private Ui::PseudoLoginWidget
{
    Q_OBJECT

public:
    PseudoLoginWidget(QWidget *parent = 0, Qt::WindowFlags f = 0) : QDialog(parent, f), ok(false)
    {
        setupUi(this);

        setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::MSWindowsFixedSizeDialogHint);

        //lineEdit->setValidator(new EmailValidator());

        connect(pushButton, SIGNAL(clicked(bool)), this, SLOT(onLogin(bool)));
        
        progress->hide();
    }

    ~PseudoLoginWidget()
    {

    }

private slots:

    void onLogin(bool clicked)
    {
        if(lineEdit->text() == "k.lebek@polsl.pl"){
            //symulujemy logowanie
            ok = true;
        }else{
            ok = false;
        }

        progress->show();
        progress->setRange(0,0);
        //progress->setValue(0);
        QTimer::singleShot(2000, this, SLOT(finalizeLogin()));
    }

    void finalizeLogin()
    {
        progress->hide();
        progress->setRange(0,100);
        //progress->setValue(0);
        
        if(ok == true){
            accept();
        }else{
            QMessageBox msgBox;
            msgBox.setWindowTitle("Login problem");
            msgBox.setText("Invalid username or password - please try again or quit application.");
            msgBox.setInformativeText("Unsuccessful login");
            msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Close);
            msgBox.setDefaultButton(QMessageBox::Retry);
            msgBox.setIcon(QMessageBox::Warning);
            int ret = msgBox.exec();

            if(ret == QMessageBox::Close){
                reject();
            }else{
                lineEdit->clear();
                lineEdit_2->clear();
            }
        }
    }

private:

    bool ok;
};


#endif	//	HEADER_GUARD___PSEUDOLOGINWIDGET_H__