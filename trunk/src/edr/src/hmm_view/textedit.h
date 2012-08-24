/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QtGui/QMainWindow>
#include <QtCore/QMap>
#include <QtCore/QPointer>
#include <QtGui/QTextDocument>

QT_FORWARD_DECLARE_CLASS(QAction)
QT_FORWARD_DECLARE_CLASS(QComboBox)
QT_FORWARD_DECLARE_CLASS(QFontComboBox)
QT_FORWARD_DECLARE_CLASS(QTextEdit)
QT_FORWARD_DECLARE_CLASS(QTextCharFormat)
QT_FORWARD_DECLARE_CLASS(QMenu)

//! Kontrolka zawiera edytor tekstu rich format
class TextEdit : public QMainWindow
{
    Q_OBJECT
    //! mo�liwe formaty zapisu
    enum SaveMode {
        toHtml,
        toOdt
    };

public:
    //! konstruktor
    //! \param parent 
    TextEdit(QWidget *parent = 0);

public:
    //! zasila kontrolk� w kod html
    //! \param html kod, kt�ry ma zosta� wy�wietlony 
    void setHtml(const QString& html);
    //! \return akcje zwi�zane z zapisem / odczytem plik�w
    const QList<QObject*>& getFileActions() const { return fileList; }
    //! \return akcje zwi�zane z edycj� tekstu
    const QList<QObject*>& getEditActions() const { return editList; }
    //! \return akcje zwi�zane z operacjami na tek�cie
    const QList<QObject*>& getTextActions() const { return textList; }

private:
    //! wywo�ywane po zmianie czcionki
    //! \param f nowa czcionka
    void fontChanged(const QFont &f);
    //! wywo�ywane po zmianie koloru
    //! \param c nowy kolor tesktu
    void colorChanged(const QColor &c);
    //! wywo�ywane po zmianie wyr�wnania
    //! \param a nowe wyr�wnanie zaznaczonego tekstu
    void alignmentChanged(Qt::Alignment a);
    //! ��czy stary format z nowym (np. bold + italic)
    //! \param format nowy format
    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    //! pr�ba zapisu
    //! \return true lub false, gdy u�ytkownik zrezygnowa� z zapisu
    bool maybeSave();
    //! wczytuje plik
    //! \param f �cie�ka pliku do wczytania
    bool load(const QString &f);
    //! ustawia nazw� aktualnego pliku
    //! \param fileName nazwa aktualnie edytowanego pliku
    void setCurrentFileName(const QString &fileName);
    //! tw�rz akcje zwi�zane z zapisem / odczytem plik�w
    void createFileActions();
    //! tw�rz akcje zwi�zane z edycj� tekstu
    void createEditActions();
    //! tw�rz akcje zwi�zane z operacjami na tek�cie
    void createTextActions();

private slots:
    //! tw�rz nowy plik
    void fileNew();
    //! otw�rz nowy plik
    void fileOpen();
    //! zapisz plik
    //! \return true lub false, gdy u�ytkownik zrezygnowa�
    bool fileSave();
    //! zapisz plik jako..
    //! \return true lub false, gdy u�ytkownik zrezygnowa�
    bool fileSaveAs();
    //! drukuj
    void filePrint();
    //! pogl�d wydruku
    void filePrintPreview();
    //! tw�rz plik pdf
    void filePrintPdf();
    //! pogrubienie tekstu
    void textBold();
    //! podkre�lenie tekstu
    void textUnderline();
    //! kursywa tekstu
    void textItalic();
    //! zmieni�a si� czcionka
    //! \param f nowa czcionka
    void textFamily(const QString &f);
    //! zmieni� si� rozmiar czcionki
    //! \param p nowy rozmiar czcionki
    void textSize(const QString &p);
    //! zmieni� si� styl
    //! \param styleIndex 
    void textStyle(int styleIndex);
    //! zmieni� si� kolor tekstu
    void textColor();
    //! wyr�wnaj tekst
    //! \param a 
    void textAlign(QAction *a);
    //! zmieni� si� format
    //! \param format 
    void currentCharFormatChanged(const QTextCharFormat &format);
    //! zmieni�a si� pozycja kursora
    void cursorPositionChanged();
    //! zmieni�a si� zawarto�� schowka
    void clipboardDataChanged();
    //! 
    void about();
    //! poka� podgl�d wydruku
    //! \param * 
    void printPreview(QPrinter *);

private:
    //! metoda tworzy zas�b dokumentu na podstawie kodu base 64
    //! \param document dokument, do kt�rego trafi nowy zas�b
    //! \param encrypted obrazek zakodowany jako base 64
    //! \return �cie�ka do zasobu
    QString base64ToResource(QTextDocument* document, const QString& encrypted) const;
    //! metoda tworzy obrazek w kodzie base64 na podstawie zasobu dokumentu
    //! \param document dokument zawieraj�cy zas�b
    //! \param resource �cie�ka do zasobu
    //! \return obrazek w kodzie base64
    QString resourceToBase64(QTextDocument* document, const QString& resource) const;
    //! metoda zamienia odno�nik (html) na obrazek w kodzie base64
    //! \param result obrazek w kodziebase64
    //! \param link link do obrazka (html)
    //! \return true, je�li si� uda�o, false, w przeciwnym razie
    bool tryChangeLinkToBase64(QString& result, const QString& link) const;
    //! metoda przechodzi przez tekst i przygotowuje go do zapisu
    //! \param root ramka z zapisywanym tekstem
    //! \param savedDocument zapisywany dokument
    //! \param saveMode tryb zapisu, na tej podstawie tworzone s� zasoby, lub obrazki w base64
    void processFrame( QTextFrame * root, QTextDocument* savedDocument, SaveMode saveMode );
    //! zamienia wszystkie odno�niki na obrazki w kodzie base64
    //! \param root ramka ze zmienianym tekstem
    //! \param savedDocument zmieniany dokument
    void changeLinksToBase64( QTextFrame * root, QTextDocument* document);

private: 
    //! lista akcji zwi�zanych z edycj� tekstu
    QList<QObject*> editList;
    //! lista akcji zwi�zanych z zapisem / odczytem plik�w
    QList<QObject*> fileList;
    //! lista akcji zwi�zanych z operacjami na tek�cie
    QList<QObject*> textList;

    //! akcja, kt�ra trafi do flexi widgeta
    QAction* actionSave;
    //! akcja, kt�ra trafi do flexi widgeta
    QAction* actionTextBold;
    //! akcja, kt�ra trafi do flexi widgeta
    QAction* actionTextUnderline;
    //! akcja, kt�ra trafi do flexi widgeta
    QAction* actionTextItalic;
    //! akcja, kt�ra trafi do flexi widgeta
    QAction* actionTextColor;
    //! akcja, kt�ra trafi do flexi widgeta
    QAction* actionAlignLeft;
    //! akcja, kt�ra trafi do flexi widgeta
    QAction* actionAlignCenter;
    //! akcja, kt�ra trafi do flexi widgeta
    QAction* actionAlignRight;
    //! akcja, kt�ra trafi do flexi widgeta
    QAction* actionAlignJustify;
    //! akcja, kt�ra trafi do flexi widgeta
    QAction* actionUndo;
    //! akcja, kt�ra trafi do flexi widgeta
    QAction* actionRedo;
    //! akcja, kt�ra trafi do flexi widgeta
    QAction* actionCut;
    //! akcja, kt�ra trafi do flexi widgeta
    QAction* actionCopy;
    //! akcja, kt�ra trafi do flexi widgeta
    QAction* actionPaste;
    //! akcja, kt�ra trafi do flexi widgeta
    QAction* actionNew;
    //! akcja, kt�ra trafi do flexi widgeta
    QAction* actionOpen;
    //! akcja, kt�ra trafi do flexi widgeta
    QAction* actionSaveAs;
    //! akcja, kt�ra trafi do flexi widgeta
    QAction* actionPrint;
    //! akcja, kt�ra trafi do flexi widgeta
    QAction* actionPrintPreview;
    //! akcja, kt�ra trafi do flexi widgeta
    QAction* actionExportPdf;

    //! kombo ze stylami, kt�re trafi do flexi widgeta
    QComboBox *comboStyle;
    //! kombo z czcionkami, kt�re trafi do flexi widgeta
    QFontComboBox *comboFont;
    //! kombo z rozmiarami, kt�re trafi do flexi widgeta
    QComboBox *comboSize;

    //! aktualna nazwa edytowanego pliku
    QString fileName;
    //! kontrolka z edytowanym tekstem
    QTextEdit *textEdit;
    
};

#endif
