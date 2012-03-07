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

class TextEdit : public QMainWindow
{
    Q_OBJECT

    enum SaveMode {
        toHtml,
        toOdt
    };

public:
    TextEdit(QWidget *parent = 0);

public:
    void setHtml(const QString& html);

    const QList<QObject*>& getFileActions() const { return fileList; }
    const QList<QObject*>& getEditActions() const { return editList; }
    const QList<QObject*>& getTextActions() const { return textList; }

private:
    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    void fontChanged(const QFont &f);
    void colorChanged(const QColor &c);
    void alignmentChanged(Qt::Alignment a);
    bool maybeSave();
    bool load(const QString &f);
    void setCurrentFileName(const QString &fileName);
    void createFileActions();
    void createEditActions();
    void createTextActions();

private slots:
    void fileNew();
    void fileOpen();
    bool fileSave();

    
    bool fileSaveAs();
    void filePrint();
    void filePrintPreview();
    void filePrintPdf();

    void textBold();
    void textUnderline();
    void textItalic();
    void textFamily(const QString &f);
    void textSize(const QString &p);
    void textStyle(int styleIndex);
    void textColor();
    void textAlign(QAction *a);

    void currentCharFormatChanged(const QTextCharFormat &format);
    void cursorPositionChanged();

    void clipboardDataChanged();
    void about();
    void printPreview(QPrinter *);

private:
    QString base64ToResource(QTextDocument* document, const QString& encrypted) const;
    QString resourceToBase64(QTextDocument* document, const QString& resource) const;
    bool tryChangeLinkToBase64(QString& result, const QString& link) const;
    void processFrame( QTextFrame * root, QTextDocument* savedDocument, SaveMode saveMode );
    void changeLinksToBase64( QTextFrame * root, QTextDocument* document);

private: 
    QList<QObject*> editList;
    QList<QObject*> fileList;
    QList<QObject*> textList;

    // tymczasowo!
    QAction* actionSave;
    QAction* actionTextBold;
    QAction* actionTextUnderline;
    QAction* actionTextItalic;
    QAction* actionTextColor;
    QAction* actionAlignLeft;
    QAction* actionAlignCenter;
    QAction* actionAlignRight;
    QAction* actionAlignJustify;
    QAction* actionUndo;
    QAction* actionRedo;
    QAction* actionCut;
    QAction* actionCopy;
    QAction* actionPaste;
    QAction* actionNew;
    QAction* actionOpen;
    QAction* actionSaveAs;
    QAction* actionPrint;
    QAction* actionPrintPreview;
    QAction* actionExportPdf;

    QComboBox *comboStyle;
    QFontComboBox *comboFont;
    QComboBox *comboSize;

    //QToolBar *tb;
    QString fileName;
    QTextEdit *textEdit;
    
};

#endif
