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
    //! mo¿liwe formaty zapisu
    enum SaveMode {
        toHtml,
        toOdt
    };

public:
    //! konstruktor
    //! \param parent 
    TextEdit(QWidget *parent = 0);

public:
    //! zasila kontrolkê w kod html
    //! \param html kod, który ma zostaæ wyœwietlony 
    void setHtml(const QString& html);
    //! \return akcje zwi¹zane z zapisem / odczytem plików
    const QList<QObject*>& getFileActions() const { return fileList; }
    //! \return akcje zwi¹zane z edycj¹ tekstu
    const QList<QObject*>& getEditActions() const { return editList; }
    //! \return akcje zwi¹zane z operacjami na tekœcie
    const QList<QObject*>& getTextActions() const { return textList; }

private:
    //! wywo³ywane po zmianie czcionki
    //! \param f nowa czcionka
    void fontChanged(const QFont &f);
    //! wywo³ywane po zmianie koloru
    //! \param c nowy kolor tesktu
    void colorChanged(const QColor &c);
    //! wywo³ywane po zmianie wyrównania
    //! \param a nowe wyrównanie zaznaczonego tekstu
    void alignmentChanged(Qt::Alignment a);
    //! ³¹czy stary format z nowym (np. bold + italic)
    //! \param format nowy format
    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    //! próba zapisu
    //! \return true lub false, gdy u¿ytkownik zrezygnowa³ z zapisu
    bool maybeSave();
    //! wczytuje plik
    //! \param f œcie¿ka pliku do wczytania
    bool load(const QString &f);
    //! ustawia nazwê aktualnego pliku
    //! \param fileName nazwa aktualnie edytowanego pliku
    void setCurrentFileName(const QString &fileName);
    //! twórz akcje zwi¹zane z zapisem / odczytem plików
    void createFileActions();
    //! twórz akcje zwi¹zane z edycj¹ tekstu
    void createEditActions();
    //! twórz akcje zwi¹zane z operacjami na tekœcie
    void createTextActions();

private slots:
    //! twórz nowy plik
    void fileNew();
    //! otwórz nowy plik
    void fileOpen();
    //! zapisz plik
    //! \return true lub false, gdy u¿ytkownik zrezygnowa³
    bool fileSave();
    //! zapisz plik jako..
    //! \return true lub false, gdy u¿ytkownik zrezygnowa³
    bool fileSaveAs();
    //! drukuj
    void filePrint();
    //! pogl¹d wydruku
    void filePrintPreview();
    //! twórz plik pdf
    void filePrintPdf();
    //! pogrubienie tekstu
    void textBold();
    //! podkreœlenie tekstu
    void textUnderline();
    //! kursywa tekstu
    void textItalic();
    //! zmieni³a siê czcionka
    //! \param f nowa czcionka
    void textFamily(const QString &f);
    //! zmieni³ siê rozmiar czcionki
    //! \param p nowy rozmiar czcionki
    void textSize(const QString &p);
    //! zmieni³ siê styl
    //! \param styleIndex 
    void textStyle(int styleIndex);
    //! zmieni³ siê kolor tekstu
    void textColor();
    //! wyrównaj tekst
    //! \param a 
    void textAlign(QAction *a);
    //! zmieni³ siê format
    //! \param format 
    void currentCharFormatChanged(const QTextCharFormat &format);
    //! zmieni³a siê pozycja kursora
    void cursorPositionChanged();
    //! zmieni³a siê zawartoœæ schowka
    void clipboardDataChanged();
    //! 
    void about();
    //! poka¿ podgl¹d wydruku
    //! \param * 
    void printPreview(QPrinter *);

private:
    //! metoda tworzy zasób dokumentu na podstawie kodu base 64
    //! \param document dokument, do którego trafi nowy zasób
    //! \param encrypted obrazek zakodowany jako base 64
    //! \return œcie¿ka do zasobu
    QString base64ToResource(QTextDocument* document, const QString& encrypted) const;
    //! metoda tworzy obrazek w kodzie base64 na podstawie zasobu dokumentu
    //! \param document dokument zawieraj¹cy zasób
    //! \param resource œcie¿ka do zasobu
    //! \return obrazek w kodzie base64
    QString resourceToBase64(QTextDocument* document, const QString& resource) const;
    //! metoda zamienia odnoœnik (html) na obrazek w kodzie base64
    //! \param result obrazek w kodziebase64
    //! \param link link do obrazka (html)
    //! \return true, jeœli siê uda³o, false, w przeciwnym razie
    bool tryChangeLinkToBase64(QString& result, const QString& link) const;
    //! metoda przechodzi przez tekst i przygotowuje go do zapisu
    //! \param root ramka z zapisywanym tekstem
    //! \param savedDocument zapisywany dokument
    //! \param saveMode tryb zapisu, na tej podstawie tworzone s¹ zasoby, lub obrazki w base64
    void processFrame( QTextFrame * root, QTextDocument* savedDocument, SaveMode saveMode );
    //! zamienia wszystkie odnoœniki na obrazki w kodzie base64
    //! \param root ramka ze zmienianym tekstem
    //! \param savedDocument zmieniany dokument
    void changeLinksToBase64( QTextFrame * root, QTextDocument* document);

private: 
    //! lista akcji zwi¹zanych z edycj¹ tekstu
    QList<QObject*> editList;
    //! lista akcji zwi¹zanych z zapisem / odczytem plików
    QList<QObject*> fileList;
    //! lista akcji zwi¹zanych z operacjami na tekœcie
    QList<QObject*> textList;

    //! akcja, która trafi do flexi widgeta
    QAction* actionSave;
    //! akcja, która trafi do flexi widgeta
    QAction* actionTextBold;
    //! akcja, która trafi do flexi widgeta
    QAction* actionTextUnderline;
    //! akcja, która trafi do flexi widgeta
    QAction* actionTextItalic;
    //! akcja, która trafi do flexi widgeta
    QAction* actionTextColor;
    //! akcja, która trafi do flexi widgeta
    QAction* actionAlignLeft;
    //! akcja, która trafi do flexi widgeta
    QAction* actionAlignCenter;
    //! akcja, która trafi do flexi widgeta
    QAction* actionAlignRight;
    //! akcja, która trafi do flexi widgeta
    QAction* actionAlignJustify;
    //! akcja, która trafi do flexi widgeta
    QAction* actionUndo;
    //! akcja, która trafi do flexi widgeta
    QAction* actionRedo;
    //! akcja, która trafi do flexi widgeta
    QAction* actionCut;
    //! akcja, która trafi do flexi widgeta
    QAction* actionCopy;
    //! akcja, która trafi do flexi widgeta
    QAction* actionPaste;
    //! akcja, która trafi do flexi widgeta
    QAction* actionNew;
    //! akcja, która trafi do flexi widgeta
    QAction* actionOpen;
    //! akcja, która trafi do flexi widgeta
    QAction* actionSaveAs;
    //! akcja, która trafi do flexi widgeta
    QAction* actionPrint;
    //! akcja, która trafi do flexi widgeta
    QAction* actionPrintPreview;
    //! akcja, która trafi do flexi widgeta
    QAction* actionExportPdf;

    //! kombo ze stylami, które trafi do flexi widgeta
    QComboBox *comboStyle;
    //! kombo z czcionkami, które trafi do flexi widgeta
    QFontComboBox *comboFont;
    //! kombo z rozmiarami, które trafi do flexi widgeta
    QComboBox *comboSize;

    //! aktualna nazwa edytowanego pliku
    QString fileName;
    //! kontrolka z edytowanym tekstem
    QTextEdit *textEdit;
    
};

#endif
