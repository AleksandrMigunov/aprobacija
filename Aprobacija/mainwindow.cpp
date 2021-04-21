/*
Copyright (C) 2021 Aleksandr Migunov

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QPrinter>
#include <QPrintDialog>
#include <QInputDialog>
#include <QFont>
#include <QFontDialog>
#include <QColor>
#include <QColorDialog>
#include <QPalette>
#include <QDateTime>
#include <QPushButton>
#include <QTextDocument>
#include <QTextDocumentWriter>
#include <QPrintPreviewDialog>
#include <QPainter>
#include <QSettings>
#include <QDesktopWidget>
#include <QCloseEvent>
#include <QDir>
#include <QTextCodec>
#include <QByteArray>
#include <QWidget>
#include "converter.h"
#include "dialogfind.h"
#include "dialogreplace.h"
#include "dialogreplaceall.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ZoomReport = ZoomQuestions = ZoomTargetText = ZoomBaseText = 0;
    EncodingReport = EncodingQuestions = EncodingTargetText = EncodingBaseText = EncodingHTML = "UTF-8";
    m_settings = new QSettings("aprobacija_settings.ini", QSettings::IniFormat, this);
    readSettings();

    if (!FilesNotLoadAuto) {
        open_files_at_start();
    }

    textEdit_focused("Report");
    set_zoom_at_start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// /////////////////////////////////////////
//Functions at start and exit
// /////////////////////////////////////////

//reading settings at start
void MainWindow::readSettings() {
    QDesktopWidget desk;
    QRect rect = desk.availableGeometry();
    m_settings->beginGroup("MainWindow");
    QPoint pos = m_settings->value("pos", QPoint(0, 0)).toPoint();
    QSize size = m_settings->value("size", rect.size()).toSize();
    if (pos.x() < 0) {
        pos.setX(0);
    }
    if (pos.y() < 0) {
        pos.setY(0);
    }
    if (size.width() > rect.width()) {
        size.setWidth(rect.width());
    }
    if (size.height() > rect.height()) {
        size.setHeight(rect.height());
    }

    resize(size);
    move(pos);

    FilenameReport = m_settings->value("report", report).toString();
    FilenameQuestions = m_settings->value("questions", questions).toString();
    FilenameTargetText = m_settings->value("target_text", target_text).toString();
    FilenameBaseText = m_settings->value("base_text", base_text).toString();
    language = m_settings->value("language", lang).toString();
    TimeAuto = m_settings->value("time_setting", time_auto).toBool();
    TitleAuto = m_settings->value("title_setting", title_auto).toBool();
    FilesNotLoadAuto = m_settings->value("files_not_loading", files_not_load_auto).toBool();

    ZoomReport = m_settings->value("zoom_report", zoom_report).toInt();
    ZoomQuestions = m_settings->value("zoom_questions", zoom_questions).toInt();
    ZoomTargetText = m_settings->value("zoom_target_text", zoom_target_text).toInt();
    ZoomBaseText = m_settings->value("zoom_base_text", zoom_base_text).toInt();
    m_settings->endGroup();
}

//writing settings at finish
void MainWindow::writeSettings() {
    m_settings->beginGroup("MainWindow");
    m_settings->setValue("pos", pos());
    m_settings->setValue("size", size());

    m_settings->setValue("report", FilenameReport);
    m_settings->setValue("questions", FilenameQuestions);
    m_settings->setValue("target_text", FilenameTargetText);
    m_settings->setValue("base_text", FilenameBaseText);
    m_settings->setValue("language", language);
    m_settings->setValue("time_setting", TimeAuto);
    m_settings->setValue("title_setting", TitleAuto);
    m_settings->setValue("files_not_loading", FilesNotLoadAuto);

    m_settings->setValue("zoom_report", ZoomReport);
    m_settings->setValue("zoom_questions", ZoomQuestions);
    m_settings->setValue("zoom_target_text", ZoomTargetText);
    m_settings->setValue("zoom_base_text", ZoomBaseText);
    m_settings->endGroup();
    m_settings->sync();
}

//setting zoom in or zoom out at start
void MainWindow::set_zoom_at_start() {
    if (ZoomReport > 0) {
        for (int n = 0; n < ZoomReport; n++) {
            ui->textEdit_Report->zoomIn();
        }
    }
    else if (ZoomReport < 0) {
        for (int n = 0; n > ZoomReport; n--) {
            ui->textEdit_Report->zoomOut();
        }
    }

    if (ZoomQuestions > 0) {
        for (int n = 0; n < ZoomQuestions; n++) {
            ui->textEdit_Questions->zoomIn();
        }
    }
    else if (ZoomQuestions < 0) {
        for (int n = 0; n > ZoomQuestions; n--) {
            ui->textEdit_Questions->zoomOut();
        }
    }

    if (ZoomTargetText > 0) {
        for (int n = 0; n < ZoomTargetText; n++) {
            ui->textEdit_TargetText->zoomIn();
        }
    }
    else if (ZoomTargetText < 0) {
        for (int n = 0; n > ZoomTargetText; n--) {
            ui->textEdit_TargetText->zoomOut();
        }
    }

    if (ZoomBaseText > 0) {
        for (int n = 0; n < ZoomBaseText; n++) {
            ui->textEdit_BaseText->zoomIn();
        }
    }
    else if (ZoomBaseText < 0) {
        for (int n = 0; n > ZoomBaseText; n--) {
            ui->textEdit_BaseText->zoomOut();
        }
    }
}

//closing the program
void MainWindow::closeEvent(QCloseEvent *event) {
    close_report();

    QMessageBox* messagebox = new QMessageBox (QMessageBox::Question,
                                               tr("Confirm"), tr("Exit program?"),
                                               QMessageBox::Yes | QMessageBox::No, this);
    messagebox->setButtonText(QMessageBox::Yes, tr("Yes"));
    messagebox->setButtonText(QMessageBox::No, tr("No"));
    int n = messagebox->exec();
    delete messagebox;

    if (n == QMessageBox::Yes) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

//setting highlighted textEdit
//all the methods that affect textEdits will call this method in the beginning
void MainWindow::textEdit_focused(QString textEditName) {
    //setting the chosen textEdit and corresponding label as highlighted
    if (textEditName == "Report") {
        ui->textEdit_Report->setFrameStyle(QFrame::WinPanel | QFrame::Plain);
        ui->label_Report->setStyleSheet("color: rgb(255, 255, 255);background-color: rgb(0, 0, 255);");
    }
    else if (textEditName == "Questions") {
        ui->textEdit_Questions->setFrameStyle(QFrame::WinPanel | QFrame::Plain);
        ui->label_Questions->setStyleSheet("color: rgb(255, 255, 255);background-color: rgb(0, 0, 255);");
    }
    else if (textEditName == "TargetText") {
        ui->textEdit_TargetText->setFrameStyle(QFrame::WinPanel | QFrame::Plain);
        ui->label_TargetText->setStyleSheet("color: rgb(255, 255, 255);background-color: rgb(0, 0, 255);");
    }
    else if (textEditName == "BaseText") {
        ui->textEdit_BaseText->setFrameStyle(QFrame::WinPanel | QFrame::Plain);
        ui->label_BaseText->setStyleSheet("color: rgb(255, 255, 255);background-color: rgb(0, 0, 255);");
    }

    //unsetting highlightening of others
    if (textEditName != "Report") {
        ui->textEdit_Report->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
        ui->label_Report->setStyleSheet("color: rgb(0, 0, 0);background-color: rgb(240, 240, 240);");
    }
    if (textEditName != "Questions") {
        ui->textEdit_Questions->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
        ui->label_Questions->setStyleSheet("color: rgb(0, 0, 0);background-color: rgb(240, 240, 240);");
    }
    if (textEditName != "TargetText") {
        ui->textEdit_TargetText->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
        ui->label_TargetText->setStyleSheet("color: rgb(0, 0, 0);background-color: rgb(240, 240, 240);");
    }
    if (textEditName != "BaseText") {
        ui->textEdit_BaseText->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
        ui->label_BaseText->setStyleSheet("color: rgb(0, 0, 0);background-color: rgb(240, 240, 240);");
    }
}

//opening files that were previously opened when the program starts
//their paths and names are stored in FilenameReport, FilenameQuestions, FilenameTargetText, and FilenameBaseText
void MainWindow::open_files_at_start() {
    //opening report
    if (FilenameReport != "") {
        open_report(FilenameReport);
    }

    else {
        if (TitleAuto) {
            on_actionInsert_report_title_triggered();
        }
    }

    //opening questions
    if (FilenameQuestions != "") {
        open_questions(FilenameQuestions);
    }

    //opening target text
    if (FilenameTargetText != "") {
        open_target_text(FilenameTargetText);
    }

    //opening base text
    if (FilenameBaseText != "") {
        open_base_text(FilenameBaseText);
    }

}

//opening report file
void MainWindow::open_report(QString file_report) {
    QFile FileReport(file_report);
    if(FileReport.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream in(&FileReport);

        if (EncodingReport == "UTF-8") in.setCodec("UTF-8");
        else if (EncodingReport == "Unicode") in.setCodec("Unicode");
        else if (EncodingReport == "Windows-1251") in.setCodec("Windows-1251");
        else if (EncodingReport == "KOI8-R") in.setCodec("KOI8-R");
        else if (EncodingReport == "IBM866") in.setCodec("IBM866");
        else if (EncodingReport == "ISO 8859-5") in.setCodec("ISO 8859-5");
        else if (EncodingReport == "Windows-1250") in.setCodec("Windows-1250");
        else if (EncodingReport == "Windows-1252") in.setCodec("Windows-1252");
        else if (EncodingReport == "Windows-1253") in.setCodec("Windows-1253");
        else if (EncodingReport == "Windows-1254") in.setCodec("Windows-1254");
        else if (EncodingReport == "Windows-1255") in.setCodec("Windows-1255");
        else if (EncodingReport == "Windows-1256") in.setCodec("Windows-1256");
        else if (EncodingReport == "Windows-1257") in.setCodec("Windows-1257");
        else if (EncodingReport == "ISO 8859-1") in.setCodec("ISO 8859-1");
        else if (EncodingReport == "ISO 8859-2") in.setCodec("ISO 8859-2");
        else if (EncodingReport == "ISO 8859-3") in.setCodec("ISO 8859-3");
        else if (EncodingReport == "ISO 8859-4") in.setCodec("ISO 8859-4");
        else if (EncodingReport == "ISO 8859-9") in.setCodec("ISO 8859-9");
        else if (EncodingReport == "ISO 8859-10") in.setCodec("ISO 8859-10");
        else if (EncodingReport == "ISO 8859-13") in.setCodec("ISO 8859-13");
        else if (EncodingReport == "ISO 8859-14") in.setCodec("ISO 8859-14");
        else if (EncodingReport == "ISO 8859-15") in.setCodec("ISO 8859-15");
        else if (EncodingReport == "ISO 8859-16") in.setCodec("ISO 8859-16");
        else if (EncodingReport == "ISO 8859-6") in.setCodec("ISO 8859-6");
        else if (EncodingReport == "ISO 8859-7") in.setCodec("ISO 8859-7");
        else if (EncodingReport == "ISO 8859-8") in.setCodec("ISO 8859-8");
        else in.setCodec("UTF-8");

        QString text = in.readAll();
        FileReport.close();

        if (FilenameReport.endsWith(".html") || FilenameReport.endsWith(".htm")) {
            ui->textEdit_Report->setAcceptRichText(true);
        }
        else {
            ui->textEdit_Report->setAcceptRichText(false);
        }
        ui->textEdit_Report->setText(text);
        TextReport = text;

        //moving cursor to the end of report
        if (!(FilenameReport.endsWith(".html") || FilenameReport.endsWith(".htm"))) {
            QTextCursor cursor = ui->textEdit_Report->textCursor();
            int pos = text.length();
            cursor.setPosition(pos);
            ui->textEdit_Report->setTextCursor(cursor);
            ui->textEdit_Report->insertPlainText("\n");
            TextReport = text + "\n";
        }
        if (TimeAuto) {
            on_actionInsert_time_of_work_start_triggered();
        }
    }
}

//opening questions file
void MainWindow::open_questions(QString file_questions) {
    QFile FileQuestions(file_questions);
    if(FileQuestions.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream in(&FileQuestions);

        if (EncodingQuestions == "UTF-8") in.setCodec("UTF-8");
        else if (EncodingQuestions == "Unicode") in.setCodec("Unicode");
        else if (EncodingQuestions == "Windows-1251") in.setCodec("Windows-1251");
        else if (EncodingQuestions == "KOI8-R") in.setCodec("KOI8-R");
        else if (EncodingQuestions == "IBM866") in.setCodec("IBM866");
        else if (EncodingQuestions == "ISO 8859-5") in.setCodec("ISO 8859-5");
        else if (EncodingQuestions == "Windows-1250") in.setCodec("Windows-1250");
        else if (EncodingQuestions == "Windows-1252") in.setCodec("Windows-1252");
        else if (EncodingQuestions == "Windows-1253") in.setCodec("Windows-1253");
        else if (EncodingQuestions == "Windows-1254") in.setCodec("Windows-1254");
        else if (EncodingQuestions == "Windows-1255") in.setCodec("Windows-1255");
        else if (EncodingQuestions == "Windows-1256") in.setCodec("Windows-1256");
        else if (EncodingQuestions == "Windows-1257") in.setCodec("Windows-1257");
        else if (EncodingQuestions == "ISO 8859-1") in.setCodec("ISO 8859-1");
        else if (EncodingQuestions == "ISO 8859-2") in.setCodec("ISO 8859-2");
        else if (EncodingQuestions == "ISO 8859-3") in.setCodec("ISO 8859-3");
        else if (EncodingQuestions == "ISO 8859-4") in.setCodec("ISO 8859-4");
        else if (EncodingQuestions == "ISO 8859-9") in.setCodec("ISO 8859-9");
        else if (EncodingQuestions == "ISO 8859-10") in.setCodec("ISO 8859-10");
        else if (EncodingQuestions == "ISO 8859-13") in.setCodec("ISO 8859-13");
        else if (EncodingQuestions == "ISO 8859-14") in.setCodec("ISO 8859-14");
        else if (EncodingQuestions == "ISO 8859-15") in.setCodec("ISO 8859-15");
        else if (EncodingQuestions == "ISO 8859-16") in.setCodec("ISO 8859-16");
        else if (EncodingQuestions == "ISO 8859-6") in.setCodec("ISO 8859-6");
        else if (EncodingQuestions == "ISO 8859-7") in.setCodec("ISO 8859-7");
        else if (EncodingQuestions == "ISO 8859-8") in.setCodec("ISO 8859-8");
        else in.setCodec("UTF-8");

        QString text = in.readAll();

        FileQuestions.close();
        ui->textEdit_Questions->setText(text);
    }
}

//opening target text
void MainWindow::open_target_text(QString file_target_text) {
    QFile FileTargetText(file_target_text);
    if(FileTargetText.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream in(&FileTargetText);

        if (EncodingTargetText == "UTF-8") in.setCodec("UTF-8");
        else if (EncodingTargetText == "Unicode") in.setCodec("Unicode");
        else if (EncodingTargetText == "Windows-1251") in.setCodec("Windows-1251");
        else if (EncodingTargetText == "KOI8-R") in.setCodec("KOI8-R");
        else if (EncodingTargetText == "IBM866") in.setCodec("IBM866");
        else if (EncodingTargetText == "ISO 8859-5") in.setCodec("ISO 8859-5");
        else if (EncodingTargetText == "Windows-1250") in.setCodec("Windows-1250");
        else if (EncodingTargetText == "Windows-1252") in.setCodec("Windows-1252");
        else if (EncodingTargetText == "Windows-1253") in.setCodec("Windows-1253");
        else if (EncodingTargetText == "Windows-1254") in.setCodec("Windows-1254");
        else if (EncodingTargetText == "Windows-1255") in.setCodec("Windows-1255");
        else if (EncodingTargetText == "Windows-1256") in.setCodec("Windows-1256");
        else if (EncodingTargetText == "Windows-1257") in.setCodec("Windows-1257");
        else if (EncodingTargetText == "ISO 8859-1") in.setCodec("ISO 8859-1");
        else if (EncodingTargetText == "ISO 8859-2") in.setCodec("ISO 8859-2");
        else if (EncodingTargetText == "ISO 8859-3") in.setCodec("ISO 8859-3");
        else if (EncodingTargetText == "ISO 8859-4") in.setCodec("ISO 8859-4");
        else if (EncodingTargetText == "ISO 8859-9") in.setCodec("ISO 8859-9");
        else if (EncodingTargetText == "ISO 8859-10") in.setCodec("ISO 8859-10");
        else if (EncodingTargetText == "ISO 8859-13") in.setCodec("ISO 8859-13");
        else if (EncodingTargetText == "ISO 8859-14") in.setCodec("ISO 8859-14");
        else if (EncodingTargetText == "ISO 8859-15") in.setCodec("ISO 8859-15");
        else if (EncodingTargetText == "ISO 8859-16") in.setCodec("ISO 8859-16");
        else if (EncodingTargetText == "ISO 8859-6") in.setCodec("ISO 8859-6");
        else if (EncodingTargetText == "ISO 8859-7") in.setCodec("ISO 8859-7");
        else if (EncodingTargetText == "ISO 8859-8") in.setCodec("ISO 8859-8");
        else in.setCodec("UTF-8");

        QString text = in.readAll();

        if (FilenameTargetText.endsWith(".sfm") || FilenameTargetText.endsWith(".SFM")) {
            QString text_converted = converter_to_html(text);
            ui->textEdit_TargetText->setHtml(text_converted);
        }
        else {
            ui->textEdit_TargetText->setText(text);
        }
    }
}


//opening base text
void MainWindow::open_base_text(QString file_base_text) {
    QFile FileBaseText(file_base_text);
    if(FileBaseText.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream in(&FileBaseText);

        if (EncodingBaseText == "UTF-8") in.setCodec("UTF-8");
        else if (EncodingBaseText == "Unicode") in.setCodec("Unicode");
        else if (EncodingBaseText == "Windows-1251") in.setCodec("Windows-1251");
        else if (EncodingBaseText == "KOI8-R") in.setCodec("KOI8-R");
        else if (EncodingBaseText == "IBM866") in.setCodec("IBM866");
        else if (EncodingBaseText == "ISO 8859-5") in.setCodec("ISO 8859-5");
        else if (EncodingBaseText == "Windows-1250") in.setCodec("Windows-1250");
        else if (EncodingBaseText == "Windows-1252") in.setCodec("Windows-1252");
        else if (EncodingBaseText == "Windows-1253") in.setCodec("Windows-1253");
        else if (EncodingBaseText == "Windows-1254") in.setCodec("Windows-1254");
        else if (EncodingBaseText == "Windows-1255") in.setCodec("Windows-1255");
        else if (EncodingBaseText == "Windows-1256") in.setCodec("Windows-1256");
        else if (EncodingBaseText == "Windows-1257") in.setCodec("Windows-1257");
        else if (EncodingBaseText == "ISO 8859-1") in.setCodec("ISO 8859-1");
        else if (EncodingBaseText == "ISO 8859-2") in.setCodec("ISO 8859-2");
        else if (EncodingBaseText == "ISO 8859-3") in.setCodec("ISO 8859-3");
        else if (EncodingBaseText == "ISO 8859-4") in.setCodec("ISO 8859-4");
        else if (EncodingBaseText == "ISO 8859-9") in.setCodec("ISO 8859-9");
        else if (EncodingBaseText == "ISO 8859-10") in.setCodec("ISO 8859-10");
        else if (EncodingBaseText == "ISO 8859-13") in.setCodec("ISO 8859-13");
        else if (EncodingBaseText == "ISO 8859-14") in.setCodec("ISO 8859-14");
        else if (EncodingBaseText == "ISO 8859-15") in.setCodec("ISO 8859-15");
        else if (EncodingBaseText == "ISO 8859-16") in.setCodec("ISO 8859-16");
        else if (EncodingBaseText == "ISO 8859-6") in.setCodec("ISO 8859-6");
        else if (EncodingBaseText == "ISO 8859-7") in.setCodec("ISO 8859-7");
        else if (EncodingBaseText == "ISO 8859-8") in.setCodec("ISO 8859-8");
        else in.setCodec("UTF-8");

        QString text = in.readAll();

        FileBaseText.close();

        if (FilenameBaseText.endsWith(".sfm") || FilenameBaseText.endsWith(".SFM")) {
            QString text_converted = converter_to_html(text);
            ui->textEdit_BaseText->setHtml(text_converted);
        }
        else {
            ui->textEdit_BaseText->setText(text);
        }

    }
}

//closing report file
void MainWindow::close_report() {
//checking if the text of report was changed
//if time is inserted automatically, the time of end of work will be added
    QString str = ui->textEdit_Report->toPlainText();
    if ((!(str.isEmpty() || str.isNull())) && TimeAuto) {
        on_actionInsert_time_of_work_end_triggered();
        str = str + "Work finished"; //this is to make sure that the time will be saved
    }
    if ((!(str.isEmpty() || str.isNull())) && (str != TextReport)) {
        QMessageBox* messagebox = new QMessageBox (QMessageBox::Warning,
                                                   tr("Warning"), tr("Do you want to save the report file?"),
                                                   QMessageBox::Yes | QMessageBox::No, this);
        messagebox->setButtonText(QMessageBox::Yes, tr("Yes"));
        messagebox->setButtonText(QMessageBox::No, tr("No"));
        int n = messagebox->exec();
        delete messagebox;
        if (n == QMessageBox::Yes) {
            if (FilenameReport.isEmpty() || FilenameReport.isNull()) {
                on_actionSave_report_as_triggered();
            }
            else {
                on_actionSave_report_triggered();
            }
        }
    }
}


// /////////////////////////////////////////////////////////////////
//Setting windows highlighted depending on window selected
// /////////////////////////////////////////////////////////////////

//report window selected
void MainWindow::on_textEdit_Report_selectionChanged()
{
    textEdit_focused("Report");
}

//questions window selected
void MainWindow::on_textEdit_Questions_selectionChanged()
{
    textEdit_focused("Questions");
}

//target text window selected
void MainWindow::on_textEdit_TargetText_selectionChanged()
{
    textEdit_focused("TargetText");
}

//base text window selected
void MainWindow::on_textEdit_BaseText_selectionChanged()
{
    textEdit_focused("BaseText");
}


// ///////////////////////////////////////////////////////////////////////////////////
//MENU FILE OPTIONS
// ///////////////////////////////////////////////////////////////////////////////////

// ///////////////////////////////////////////////////////////////
//Option "Open"
// ///////////////////////////////////////////////////////////////

//opening questions file
void MainWindow::on_actionOpen_questions_triggered()
{
    textEdit_focused("Questions");

    QString file = QFileDialog::getOpenFileName(this, tr("Open the questions file"), "",
                                              tr("All files for opening (*.txt *.sfm *.SFM *.html *.htm);;\
Text files (*.txt);;Paratext files (*.sfm *.SFM);;HTML files (*.html *.htm)"));

    if(!file.isEmpty()) {
        FilenameQuestions = file;
        open_questions(FilenameQuestions);
    }
}

//opening report file
void MainWindow::on_actionOpen_report_triggered()
{
    textEdit_focused("Report");
    close_report(); //closing previously opened report file

    //opening (another) report file
    QString file = QFileDialog::getOpenFileName(this, tr("Open the report file"), "",
                                                tr("All files for opening (*.txt *.html *.htm);;\
Text files (*.txt);;HTML files (*.html *.htm)"));

    if(!file.isEmpty())
    {
        FilenameReport = file;
        open_report(FilenameReport);
    }
}

//opening target text file
void MainWindow::on_actionOpen_target_text_triggered()
{
    textEdit_focused("TargetText");

    QString file = QFileDialog::getOpenFileName(this, tr("Open the target text file"), "",
                                                tr("All files for opening (*.txt *.sfm *.SFM *.html *.htm);;\
Text files (*.txt);;Paratext files (*.sfm *.SFM);;HTML files (*.html *.htm)"));

    if(!file.isEmpty())
    {
        FilenameTargetText = file;
        open_target_text(FilenameTargetText);
    }
}

//opening base text file
void MainWindow::on_actionOpen_base_text_triggered()
{
    textEdit_focused("BaseText");

    QString file = QFileDialog::getOpenFileName(this, tr("Open the base text file"), "",
                                                tr("All files for opening (*.txt *.sfm *.SFM *.html *.htm);;\
Text files (*.txt);;Paratext files (*.sfm *.SFM);;HTML files (*.html *.htm)"));

    if(!file.isEmpty())
    {
        FilenameBaseText = file;
        open_base_text(FilenameBaseText);
    }
}


// ///////////////////////////////////////////////////////////////
//Option "Close"
// ///////////////////////////////////////////////////////////////
//closing questions
void MainWindow::on_actionClose_questions_triggered()
{
    textEdit_focused("Questions");
    ui->textEdit_Questions->setText("");
}

//closing report
void MainWindow::on_actionClose_report_triggered()
{
    textEdit_focused("Report");
    close_report();
    ui->textEdit_Report->setText("");
}

//closing target text
void MainWindow::on_actionClose_target_text_triggered()
{
    textEdit_focused("TargetText");
    ui->textEdit_TargetText->setText("");
}

//closing base text
void MainWindow::on_actionClose_base_text_triggered()
{
    textEdit_focused("BaseText");
    ui->textEdit_BaseText->setText("");
}

// //////////////////////////////////////
//Option "New report"
// //////////////////////////////////////

//creating new report file
void MainWindow::on_actionNew_report_triggered() {
    textEdit_focused("Report");
    close_report();

    FilenameReport = "";

    ui->textEdit_Report->setAcceptRichText(true);
    ui->textEdit_Report->setText("");

    if (TitleAuto) {
         on_actionInsert_report_title_triggered();
    }


    if (TimeAuto) {
        on_actionInsert_time_of_work_start_triggered();
    }
}

// ////////////////////////////////////////////
//Option "Save report"
// ////////////////////////////////////////////
//saving report in existing file
void MainWindow::on_actionSave_report_triggered()
{
    textEdit_focused("Report");

    QFile FileReport(FilenameReport);

    if (FilenameReport.isEmpty() || FilenameReport.isNull()) {
        on_actionSave_report_as_triggered();
    }

    if(FileReport.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream out(&FileReport);

        if (EncodingReport == "UTF-8") out.setCodec("UTF-8");
        else if (EncodingReport == "Unicode") out.setCodec("Unicode");
        else if (EncodingReport == "Windows-1251") out.setCodec("Windows-1251");
        else if (EncodingReport == "KOI8-R") out.setCodec("KOI8-R");
        else if (EncodingReport == "IBM866") out.setCodec("IBM866");
        else if (EncodingReport == "ISO 8859-5") out.setCodec("ISO 8859-5");
        else if (EncodingReport == "Windows-1250") out.setCodec("Windows-1250");
        else if (EncodingReport == "Windows-1252") out.setCodec("Windows-1252");
        else if (EncodingReport == "Windows-1253") out.setCodec("Windows-1253");
        else if (EncodingReport == "Windows-1254") out.setCodec("Windows-1254");
        else if (EncodingReport == "Windows-1255") out.setCodec("Windows-1255");
        else if (EncodingReport == "Windows-1256") out.setCodec("Windows-1256");
        else if (EncodingReport == "Windows-1257") out.setCodec("Windows-1257");
        else if (EncodingReport == "ISO 8859-1") out.setCodec("ISO 8859-1");
        else if (EncodingReport == "ISO 8859-2") out.setCodec("ISO 8859-2");
        else if (EncodingReport == "ISO 8859-3") out.setCodec("ISO 8859-3");
        else if (EncodingReport == "ISO 8859-4") out.setCodec("ISO 8859-4");
        else if (EncodingReport == "ISO 8859-9") out.setCodec("ISO 8859-9");
        else if (EncodingReport == "ISO 8859-10") out.setCodec("ISO 8859-10");
        else if (EncodingReport == "ISO 8859-13") out.setCodec("ISO 8859-13");
        else if (EncodingReport == "ISO 8859-14") out.setCodec("ISO 8859-14");
        else if (EncodingReport == "ISO 8859-15") out.setCodec("ISO 8859-15");
        else if (EncodingReport == "ISO 8859-16") out.setCodec("ISO 8859-16");
        else if (EncodingReport == "ISO 8859-6") out.setCodec("ISO 8859-6");
        else if (EncodingReport == "ISO 8859-7") out.setCodec("ISO 8859-7");
        else if (EncodingReport == "ISO 8859-8") out.setCodec("ISO 8859-8");
        else out.setCodec("UTF-8");


        if (ui->textEdit_Report->acceptRichText()) {
            out << ui->textEdit_Report->toHtml();
        }
        else {
            out << ui->textEdit_Report->toPlainText();

        }

        FileReport.flush();
        FileReport.close();

        TextReport = ui->textEdit_Report->toPlainText();
    }
}

// /////////////////////////////////////////////////////
//Option "Save report as"
// /////////////////////////////////////////////////////
//saving report in a new file
void MainWindow::on_actionSave_report_as_triggered()
{
    textEdit_focused("Report");

    QStringList items;
    items << tr("Plain text (txt)") << tr("Rich text (HTML)");

    bool ok;
    QString item = QInputDialog::getItem(this, tr("Saving file"), tr("Choose how to save file:"),
                                         items, 0, false, &ok);

    if (ok && !item.isEmpty()) {

        QString file;

        bool isHTML = false;

        if (item == tr("Rich text (HTML)")) {
            ui->textEdit_Report->setAcceptRichText(true);
            file = QFileDialog::getSaveFileName(this, tr("Save the report file"), "",
                                                tr("HTML files (*.html)"));
        }
        else {
            ui->textEdit_Report->setAcceptRichText(false);
            file = QFileDialog::getSaveFileName(this, tr("Save the report file"), "",
                                                tr("Text files (*.txt)"));
        }

        if(!file.isEmpty())
        {
            if (isHTML == true) {
                file = file + ".html";
            }
            else {
                file = file + ".txt";
            }
            QDir direct;
            QString dir = direct.filePath(file);
            FilenameReport = dir;
            on_actionSave_report_triggered();
        }
    }
}

// ////////////////////////////////////////////////////////
//Option "Print"
// ////////////////////////////////////////////////////////
//printing questions
void MainWindow::on_actionPrint_questions_triggered()
{
    textEdit_focused("Questions");

    QPrinter printer (QPrinter::HighResolution);
    printer.setPrinterName(tr("Printer name"));
    QPrintDialog dialog(&printer, this);
    if(dialog.exec() == QDialog::Rejected) return;
    ui->textEdit_Questions->print(&printer);
}

//printing report
void MainWindow::on_actionPrint_report_triggered()
{
    textEdit_focused("Report");

    QPrinter printer (QPrinter::HighResolution);
    printer.setPrinterName(tr("Printer name"));
    QPrintDialog dialog(&printer, this);
    if(dialog.exec() == QDialog::Rejected) return;
    ui->textEdit_Report->print(&printer);
}

//printing target text
void MainWindow::on_actionPrint_target_text_triggered()
{
    textEdit_focused("TargetText");

    QPrinter printer (QPrinter::HighResolution);
    printer.setPrinterName(tr("Printer name"));
    QPrintDialog dialog(&printer, this);
    if(dialog.exec() == QDialog::Rejected) return;
    ui->textEdit_TargetText->print(&printer);
}

//printing base text
void MainWindow::on_actionPrint_base_text_triggered()
{
    textEdit_focused("BaseText");

    QPrinter printer (QPrinter::HighResolution);
    printer.setPrinterName(tr("Printer name"));
    QPrintDialog dialog(&printer, this);
    if(dialog.exec() == QDialog::Rejected) return;
    ui->textEdit_BaseText->print(&printer);
}

// //////////////////////////////////////////////////////
//Option "Print preview"
// //////////////////////////////////////////////////////

//print preview of questions
void MainWindow::on_actionPrint_preview_questions_triggered()
{
    textEdit_focused("Questions");

    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this, Qt::Window);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(paintPreviewQuestions(QPrinter*)));
    preview.exec();
}

void MainWindow::paintPreviewQuestions(QPrinter *printer) {
    ui->textEdit_Questions->print(printer);
}

//print preview of report
void MainWindow::on_actionPrint_preview_report_triggered()
{
    textEdit_focused("Report");

    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this, Qt::Window);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(paintPreviewReport(QPrinter*)));
    preview.exec();
}

void MainWindow::paintPreviewReport(QPrinter *printer) {
    ui->textEdit_Report->print(printer);
}

//print preview of target text
void MainWindow::on_actionPrint_preview_target_text_triggered()
{
    textEdit_focused("TargetText");

    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this, Qt::Window);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(paintPreviewTargetText(QPrinter*)));
    preview.exec();
}

void MainWindow::paintPreviewTargetText(QPrinter *printer) {
    ui->textEdit_TargetText->print(printer);
}

//print preview of base text
void MainWindow::on_actionPrint_preview_base_text_triggered()
{
    textEdit_focused("BaseText");

    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this, Qt::Window);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(paintPreviewBaseText(QPrinter*)));
    preview.exec();
}

void MainWindow::paintPreviewBaseText(QPrinter *printer) {
    ui->textEdit_BaseText->print(printer);
}


// /////////////////////////////////////////////////////////////
//Option "Convert to PDF"
// /////////////////////////////////////////////////////////////

//converting to PDF
void MainWindow::convert_to_PDF(QString writing, QString document_name) {
    QTextDocument document;
    document.setHtml(writing);

    QString Filename;
    if (document_name == "Questions") {
        Filename = FilenameQuestions;
    }
    else if (document_name == "Report") {
        Filename = FilenameReport;
    }
    else if (document_name == "TargetText") {
        Filename = FilenameTargetText;
    }
    else if (document_name == "BaseText") {
        Filename = FilenameBaseText;
    }
    else return;

    if (Filename.endsWith(".txt")) {
        Filename = Filename.replace(".txt", ".pdf");
    }
    else if (Filename.endsWith(".html")) {
        Filename = Filename.replace(".html", ".pdf");
    }
    else if (Filename.endsWith(".htm")) {
        Filename = Filename.replace(".htm", ".pdf");
    }
    else if (Filename.endsWith(".sfm")) {
        Filename = Filename.replace(".sfm", ".pdf");
    }
    else if (Filename.endsWith(".SFM")) {
        Filename = Filename.replace(".SFM", ".pdf");
    }
    else {
        Filename = Filename + ".pdf";
    }

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFileName(Filename);
    printer.setPaperSize(QPrinter::A4);
    printer.setOutputFormat(QPrinter::PdfFormat);
    document.print(&printer);
    QMessageBox::information(this, tr("Conversion to PDF"), tr("Created file") + " " + Filename);
}

//converting questions to PDF
void MainWindow::on_actionConvert_questions_to_PDF_triggered()
{
    textEdit_focused("Questions");

    QString writing = ui->textEdit_Questions->toHtml();
    QString writing_2 = ui->textEdit_Questions->toPlainText();
    if (writing_2.isEmpty() || writing_2.isNull()) {
        QMessageBox::warning(this, tr("Warning"), tr("The quesions file is not open"));
        return;
    }

    convert_to_PDF(writing, "Questions");
}

//converting report to PDF
void MainWindow::on_actionConvert_report_to_PDF_triggered()
{
    textEdit_focused("Report");

    QString writing = ui->textEdit_Report->toHtml();
    QString writing_2 = ui->textEdit_Report->toPlainText();
    if (writing_2.isEmpty() || writing_2.isNull()) {
        QMessageBox::warning(this, tr("Warning"), tr("The report is empty"));
        return;
    }

    if (FilenameReport.isEmpty() || FilenameReport.isNull()) {
        bool bOK;
        FilenameReport = QInputDialog::getText(this, tr("Choose file name"), tr("Enter file name:"),
                                                 QLineEdit::Normal, "", &bOK);
        if (!bOK) return;
    }

    convert_to_PDF(writing, "Report");
}

//converting target text to PDF
void MainWindow::on_actionConvert_target_text_to_PDF_triggered()
{
    textEdit_focused("TargetText");

    QString writing = ui->textEdit_TargetText->toHtml();
    QString writing_2 = ui->textEdit_TargetText->toPlainText();
    if (writing_2.isEmpty() || writing_2.isNull()) {
        QMessageBox::warning(this, tr("Warning"), tr("The target text is not open"));
        return;
    }

    convert_to_PDF(writing, "TargetText");
}

//converting base text to PDF
void MainWindow::on_actionConvert_base_text_to_PDF_triggered()
{
    textEdit_focused("BaseText");

    QString writing = ui->textEdit_BaseText->toHtml();
    QString writing_2 = ui->textEdit_BaseText->toPlainText();
    if (writing_2.isEmpty() || writing_2.isNull()) {
        QMessageBox::warning(this, tr("Warning"), tr("The base text is not open"));
        return;
    }

    convert_to_PDF(writing, "BaseText");
}

// ////////////////////////////////////////////////////////
//Option "Convert to ODF"
// ////////////////////////////////////////////////////////
//converting to ODT
void MainWindow::convert_to_ODT(QString writing, QString document_name) {
    QTextDocument document;
    document.setHtml(writing);

    QString Filename;
    if (document_name == "Questions") {
        Filename = FilenameQuestions;
    }
    else if (document_name == "Report") {
        Filename = FilenameReport;
    }
    else if (document_name == "TargetText") {
        Filename = FilenameTargetText;
    }
    else if (document_name == "BaseText") {
        Filename = FilenameBaseText;
    }
    else return;

    if (Filename.endsWith(".txt")) {
        Filename = Filename.replace(".txt", ".odt");
    }
    else if (Filename.endsWith(".html")) {
        Filename = Filename.replace(".html", ".odt");
    }
    else if (Filename.endsWith(".htm")) {
        Filename = Filename.replace(".htm", ".odt");
    }
    else if (Filename.endsWith(".sfm")) {
        Filename = Filename.replace(".sfm", ".odt");
    }
    else if (Filename.endsWith(".SFM")) {
        Filename = Filename.replace(".SFM", ".odt");
    }
    else {
        Filename = Filename + ".odt";
    }

    QTextDocumentWriter writer;
    writer.setFormat("odf");
    writer.setFileName(Filename);
    writer.write(&document);
    QMessageBox::information(this, tr("Conversion to ODT"), tr("Created file") + " " + Filename);
}

//converting questions to ODT
void MainWindow::on_actionConvert_questions_to_ODT_triggered()
{
    textEdit_focused("Questions");

    QString writing = ui->textEdit_Questions->toHtml();
    QString writing_2 = ui->textEdit_Questions->toPlainText();
    if (writing_2.isEmpty() || writing_2.isNull()) {
        QMessageBox::warning(this, tr("Warning"), tr("The quesions file is not open"));
        return;
    }

    convert_to_ODT(writing, "Questions");
}

//converting report to ODT
void MainWindow::on_actionConvert_report_to_ODT_triggered()
{
    textEdit_focused("Report");

    QString writing = ui->textEdit_Report->toHtml();
    QString writing_2 = ui->textEdit_Report->toPlainText();
    if (writing_2.isEmpty() || writing_2.isNull()) {
        QMessageBox::warning(this, tr("Warning"), tr("The report is empty"));
        return;
    }

    if (FilenameReport.isEmpty() || FilenameReport.isNull()) {
        bool bOK;
        FilenameReport = QInputDialog::getText(this, tr("Choose file name"), tr("Enter file name:"),
                                                 QLineEdit::Normal, "", &bOK);
        if (!bOK) return;
    }

    convert_to_ODT(writing, "Report");
}

//converting target text to ODT
void MainWindow::on_actionConvert_target_text_to_ODT_triggered()
{
    textEdit_focused("TargetText");

    QString writing = ui->textEdit_TargetText->toHtml();
    QString writing_2 = ui->textEdit_TargetText->toPlainText();
    if (writing_2.isEmpty() || writing_2.isNull()) {
        QMessageBox::warning(this, tr("Warning"), tr("The target text is not open"));
        return;
    }

    convert_to_ODT(writing, "TargetText");
}

//converting base text to ODT
void MainWindow::on_actionConvert_base_text_to_ODT_triggered()
{
    textEdit_focused("BaseText");

    QString writing = ui->textEdit_BaseText->toHtml();
    QString writing_2 = ui->textEdit_BaseText->toPlainText();
    if (writing_2.isEmpty() || writing_2.isNull()) {
        QMessageBox::warning(this, tr("Warning"), tr("The base text is not open"));
        return;
    }

    convert_to_ODT(writing, "BaseText");
}

// ////////////////////////////////////////////////////
//Option "Exit"
// ////////////////////////////////////////////////////
//exiting the program
void MainWindow::on_actionExit_triggered()
{
    textEdit_focused("Report");
    close(); //this will call exit event
}

// //////////////////////////////////////////////////////////////////////////////
//MENU EDIT
// //////////////////////////////////////////////////////////////////////////////

// /////////////////////////////////////////////////////////////////////////////
//Option "Undo"
// ////////////////////////////////////////////////////////////////////////////
//undo in report
void MainWindow::on_actionUndo_triggered()
{
    textEdit_focused("Report");
    ui->textEdit_Report->undo();
}

// /////////////////////////////////////////////////////////////////////////////
//Option "Redo"
// ////////////////////////////////////////////////////////////////////////////
//redo in report
void MainWindow::on_actionRedo_triggered()
{
    textEdit_focused("Report");
    ui->textEdit_Report->redo();
}

// ///////////////////////////////////////////////////////////////////////////
//Option "Cut"
// ///////////////////////////////////////////////////////////////////////////
//cut from report
void MainWindow::on_actionCut_triggered()
{
    textEdit_focused("Report");
    ui->textEdit_Report->cut();
}

// //////////////////////////////////////////////////////////////////////////
//Option "Copy"
// //////////////////////////////////////////////////////////////////////////
//copy from questions
void MainWindow::on_actionCopy_from_questions_triggered()
{
    textEdit_focused("Questions");
    ui->textEdit_Questions->copy();
}

//copy from report
void MainWindow::on_actionCopy_from_report_triggered()
{
    textEdit_focused("Report");
    ui->textEdit_Report->copy();
}

//copy from target text
void MainWindow::on_actionCopy_from_target_text_triggered()
{
    textEdit_focused("TargetText");
    ui->textEdit_TargetText->copy();
}

//copy from base text
void MainWindow::on_actionCopy_from_base_text_triggered()
{
    textEdit_focused("BaseText");
    ui->textEdit_BaseText->copy();
}

// //////////////////////////////////////////////////////////////
//Option "Paste"
// //////////////////////////////////////////////////////////////
//paste into report
void MainWindow::on_actionPaste_triggered()
{
    textEdit_focused("Report");
    ui->textEdit_Report->paste();
}

// ///////////////////////////////////////////////////////////////
//Option "Copy and paste"
// ///////////////////////////////////////////////////////////////
//copy and paste from questions
void MainWindow::on_actionCopy_and_paste_from_questions_triggered()
{
    textEdit_focused("Report");

    ui->textEdit_Questions->copy();
    ui->textEdit_Report->paste();
    ui->textEdit_Report->insertPlainText("\n");
}

//copy and paste from report
void MainWindow::on_actionCopy_and_paste_from_report_triggered()
{
    textEdit_focused("Report");

    ui->textEdit_Report->copy();
    ui->textEdit_Report->paste();
    ui->textEdit_Report->insertPlainText("\n");
}

//copy and paste from target text
void MainWindow::on_actionCopy_and_paste_from_target_text_triggered()
{
    textEdit_focused("Report");

    ui->textEdit_TargetText->copy();
    ui->textEdit_Report->paste();
    ui->textEdit_Report->insertPlainText("\n");
}

//copy and paste from base text
void MainWindow::on_actionCopy_and_paste_from_base_text_triggered()
{
    textEdit_focused("Report");

    ui->textEdit_BaseText->copy();
    ui->textEdit_Report->paste();
    ui->textEdit_Report->insertPlainText("\n");
}

// /////////////////////////////////////////////////////////////////
//Option "Delete"
// /////////////////////////////////////////////////////////////////
//deleting from report
void MainWindow::on_actionDelete_triggered()
{
    textEdit_focused("Report");

    QTextCursor cursor = ui->textEdit_Report->textCursor();
    cursor.removeSelectedText();
}

// /////////////////////////////////////////////////////////////////
//Option "Select all"
// /////////////////////////////////////////////////////////////////
//selecting all from questions
void MainWindow::on_actionSelect_all_from_questions_triggered()
{
    textEdit_focused("Questions");
    ui->textEdit_Questions->selectAll();
}

//selecting all from report
void MainWindow::on_actionSelect_all_from_report_triggered()
{
    textEdit_focused("Report");
    ui->textEdit_Report->selectAll();
}

//selecting all from target text
void MainWindow::on_actionSelect_all_from_target_text_triggered()
{
    textEdit_focused("TargetText");
    ui->textEdit_TargetText->selectAll();
}

//selecting all from base text
void MainWindow::on_actionSelect_all_from_base_text_triggered()
{
    textEdit_focused("BaseText");
    ui->textEdit_BaseText->selectAll();
}

// ///////////////////////////////////////////////////////////////////
//Searching
// ///////////////////////////////////////////////////////////////////
//search
void MainWindow::search_word(QString document_name) {
    DialogFind dialogfindDocument;
    dialogfindDocument.exec();
    dialogfindDocument.show(); //in this way, this dialog will be shown when search continues

    bool Found = false;

    QFlags<QTextDocument::FindFlag> searchOptions;

    if (dialogfindDocument.CaseSensitive == true) {
        if (dialogfindDocument.WholeWord == true) {
            if (dialogfindDocument.SearchDirection == "Forward") {
                searchOptions = QTextDocument::FindCaseSensitively | QTextDocument::FindWholeWords;
            }
            else if (dialogfindDocument.SearchDirection == "Backward") {
                searchOptions = QTextDocument::FindBackward | QTextDocument::FindCaseSensitively |
                        QTextDocument::FindWholeWords;
            }
        }
        else {
            if (dialogfindDocument.SearchDirection == "Forward") {
                searchOptions = QTextDocument::FindCaseSensitively;
            }
            else if (dialogfindDocument.SearchDirection == "Backward") {
                searchOptions = QTextDocument::FindBackward | QTextDocument::FindCaseSensitively;
            }
        }
    }

    else {
        if (dialogfindDocument.WholeWord == true) {
            if (dialogfindDocument.SearchDirection == "Forward") {
                searchOptions = QTextDocument::FindWholeWords;
            }
            else if (dialogfindDocument.SearchDirection == "Backward") {
                searchOptions = QTextDocument::FindBackward | QTextDocument::FindWholeWords;
            }

        }
        else {
            if (dialogfindDocument.SearchDirection == "Forward") {
                searchOptions = 0;
            }
            else if (dialogfindDocument.SearchDirection == "Backward") {
                searchOptions = QTextDocument::FindBackward;
            }
        }
    }

    if (document_name == "Questions") {
        Found = ui->textEdit_Questions->find(dialogfindDocument.search, searchOptions);
    }
    else if (document_name == "Report") {
        Found = ui->textEdit_Report->find(dialogfindDocument.search, searchOptions);
    }
    else if (document_name == "TargetText") {
        Found = ui->textEdit_TargetText->find(dialogfindDocument.search, searchOptions);
    }
    else if (document_name == "BaseText") {
        Found = ui->textEdit_BaseText->find(dialogfindDocument.search, searchOptions);
    }
    else return;

    if ((!(dialogfindDocument.search.isEmpty() || dialogfindDocument.search.isNull())) & !Found) {
        QMessageBox* notFound = new QMessageBox(QMessageBox::Warning, tr("Not found"),
                                                tr("The word was not found"));
        notFound->exec();
        delete notFound;
    }

    if ((!(dialogfindDocument.search.isEmpty() || dialogfindDocument.search.isNull())) & Found) {
        bool continueSearch = true;
        bool foundNew = true;

        while (continueSearch & foundNew) {
            QMessageBox* findNext =
                    new QMessageBox(QMessageBox::Question, tr("Continue?"), tr("Find next?"),
                                    QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

            findNext->setButtonText(QMessageBox::Yes, tr("Yes"));
            findNext->setButtonText(QMessageBox::No, tr("No"));
            findNext->setButtonText(QMessageBox::Cancel, tr("Cancel"));

            int n = findNext->exec();
            delete findNext;

            if (n == QMessageBox::Yes) {
                if (document_name == "Questions") {
                    foundNew = ui->textEdit_Questions->find(dialogfindDocument.search, searchOptions);
                }
                else if (document_name == "Report") {
                    foundNew = ui->textEdit_Report->find(dialogfindDocument.search, searchOptions);
                }
                else if (document_name == "TargetText") {
                    foundNew = ui->textEdit_TargetText->find(dialogfindDocument.search, searchOptions);
                }
                else if (document_name == "BaseText") {
                    foundNew = ui->textEdit_BaseText->find(dialogfindDocument.search, searchOptions);
                }
                else return;
            }
            else if ((n == QMessageBox::No) || (n == QMessageBox::Cancel)) {
                continueSearch = false;
            }
        }

        if (!foundNew) {
            QMessageBox* noNewFound = new QMessageBox(QMessageBox::Warning, tr("End of search"),
                                                      tr("End of search is reached"));
            noNewFound->exec();
            delete noNewFound;
        }
    }
}


// //////////////////////////////////////////////////////////////////
//Option "Find"
// //////////////////////////////////////////////////////////////////
//searching in questions
void MainWindow::on_actionFind_in_questions_triggered() {
    textEdit_focused("Questions");

    QString str = ui->textEdit_Questions->toPlainText();
    if (str.isEmpty() || str.isNull()) {
        QMessageBox* warning = new QMessageBox(QMessageBox::Warning, tr("Warning"), tr("File is not open"));
        warning->exec();
        delete warning;
        return;
    }
    search_word("Questions");
}

//searching in report
void MainWindow::on_actionFind_in_report_triggered() {
    textEdit_focused("Report");

    QString str = ui->textEdit_Report->toPlainText();
    if (str.isEmpty() || str.isNull()) {
        QMessageBox* warning = new QMessageBox(QMessageBox::Warning, tr("Warning"), tr("The report is empty"));
        warning->exec();
        delete warning;
        return;
    }

    search_word("Report");
}

//searching in target text
void MainWindow::on_actionFind_in_target_text_triggered() {
    textEdit_focused("TargetText");

    QString str = ui->textEdit_TargetText->toPlainText();
    if (str.isEmpty() || str.isNull()) {
        QMessageBox* warning = new QMessageBox(QMessageBox::Warning, tr("Warning"), tr("File is not open"));
        warning->exec();
        delete warning;
        return;
    }

    search_word("TargetText");
}

//searching in base text
void MainWindow::on_actionFind_in_base_text_triggered() {
    textEdit_focused("BaseText");

    QString str = ui->textEdit_BaseText->toPlainText();
    if (str.isEmpty() || str.isNull()) {
        QMessageBox* warning = new QMessageBox(QMessageBox::Warning, tr("Warning"), tr("File is not open"));
        warning->exec();
        delete warning;
        return;
    }

    search_word("BaseText");
}

// /////////////////////////////////////////////////////
//Option "Find and replace"
// /////////////////////////////////////////////////////
//searching and replacing in report
void MainWindow::on_actionFind_and_replace_triggered()
{
    textEdit_focused("Report");

    QString str = ui->textEdit_Report->toPlainText();
    if (str.isEmpty() || str.isNull()) {
        QMessageBox* warning = new QMessageBox(QMessageBox::Warning, tr("Warning"), tr("The report is empty"));
        warning->exec();
        delete warning;
        return;
    }

    DialogReplace dialogreplace;
    dialogreplace.exec();
    dialogreplace.show(); //in this way, this dialog will be shown when search continues
    bool Found = false;

    QFlags<QTextDocument::FindFlag> searchOptions;

    if (dialogreplace.CaseSensitive == true) {
        if (dialogreplace.WholeWord == true) {
            if (dialogreplace.SearchDirection == "Forward") {
                searchOptions = QTextDocument::FindCaseSensitively | QTextDocument::FindWholeWords;
            }
            else if (dialogreplace.SearchDirection == "Backward") {
                searchOptions = QTextDocument::FindBackward | QTextDocument::FindCaseSensitively |
                        QTextDocument::FindWholeWords;
            }
        }
        else {
            if (dialogreplace.SearchDirection == "Forward") {
                searchOptions = QTextDocument::FindCaseSensitively;
            }
            else if (dialogreplace.SearchDirection == "Backward") {
                searchOptions = QTextDocument::FindBackward | QTextDocument::FindCaseSensitively;
            }
        }
    }

    else {
        if (dialogreplace.WholeWord == true) {
            if (dialogreplace.SearchDirection == "Forward") {
                searchOptions = QTextDocument::FindWholeWords;
            }
            else if (dialogreplace.SearchDirection == "Backward") {
                searchOptions = QTextDocument::FindBackward | QTextDocument::FindWholeWords;
            }
        }
        else {
            if (dialogreplace.SearchDirection == "Forward") {
                searchOptions = 0;
            }
            else if (dialogreplace.SearchDirection == "Backward") {
                searchOptions = QTextDocument::FindBackward;
            }
        }
    }

    Found = ui->textEdit_Report->find(dialogreplace.search, searchOptions);

    if ((!(dialogreplace.search.isEmpty() || dialogreplace.search.isNull())) & !Found) {
        QMessageBox* notFound = new QMessageBox(QMessageBox::Warning, tr("Not found"),
                                                tr("The word was not found"));
        notFound->exec();
        delete notFound;
    }

    else if (!(dialogreplace.search.isEmpty() || dialogreplace.search.isNull())) {
        ui->textEdit_Report->cut();
        ui->textEdit_Report->insertPlainText(dialogreplace.replace);
    }

    if ((!(dialogreplace.search.isEmpty() || dialogreplace.search.isNull())) & Found) {
        bool continueSearch = true;
        bool foundNew = true;

        while (continueSearch & foundNew) {
            QMessageBox* findNext =
                    new QMessageBox(QMessageBox::Question, tr("Continue?"), tr("Find and replace next?"),
                                    QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

            findNext->setButtonText(QMessageBox::Yes, tr("Yes"));
            findNext->setButtonText(QMessageBox::No, tr("No"));
            findNext->setButtonText(QMessageBox::Cancel, tr("Cancel"));

            int n = findNext->exec();
            delete findNext;

            if (n == QMessageBox::Yes) {
                foundNew = ui->textEdit_Report->find(dialogreplace.search, searchOptions);

                if (foundNew) {
                    ui->textEdit_Report->cut();
                    ui->textEdit_Report->insertPlainText(dialogreplace.replace);
                }
            }

            else if ((n == QMessageBox::No) || (n == QMessageBox::Cancel)) {
                continueSearch = false;
            }
        }

        if (!foundNew) {
            QMessageBox* noNewFound = new QMessageBox(QMessageBox::Warning, tr("End of search"),
                                                      tr("End of search is reached"));
            noNewFound->exec();
            delete noNewFound;
        }
    }

}

// /////////////////////////////////////////////////////////
//Option "Find and replace all"
// /////////////////////////////////////////////////////////
//searching and replacing all in report
void MainWindow::on_actionFind_and_replace_all_triggered()
{
    textEdit_focused("Report");

    QString str = ui->textEdit_Report->toPlainText();
    if (str.isEmpty() || str.isNull()) {
        QMessageBox* warning = new QMessageBox(QMessageBox::Warning, tr("Warning"), tr("The report is empty"));
        warning->exec();
        delete warning;
        return;
    }

    DialogReplaceAll dialogreplaceall;
    dialogreplaceall.exec();
    dialogreplaceall.show();

    if (!(dialogreplaceall.search.isEmpty() || dialogreplaceall.search.isNull())) {
        QString text = ui->textEdit_Report->toPlainText();
        text.replace(dialogreplaceall.search, dialogreplaceall.replace);
        ui->textEdit_Report->setText(text);
        QMessageBox::information(this, tr("Found and replaced"), tr("Find and replace all are done"));
    }
}

// ////////////////////////////////////////////////////////////////////
//MENU VIEW
// ////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////
//Option "Zoom in"
// ////////////////////////////////////////////////////////
//zoom in all windows
void MainWindow::on_actionZoom_in_all_windows_triggered()
{
    ui->textEdit_Questions->zoomIn();
    ui->textEdit_Report->zoomIn();
    ui->textEdit_TargetText->zoomIn();
    ui->textEdit_BaseText->zoomIn();

    ZoomQuestions++;
    ZoomReport++;
    ZoomTargetText++;
    ZoomBaseText++;
}

//zoom in questions
void MainWindow::on_actionZoom_in_questions_triggered()
{
    textEdit_focused("Questions");

    ui->textEdit_Questions->zoomIn();
    ZoomQuestions++;
}

//zoom in report
void MainWindow::on_actionZoom_in_report_triggered()
{
    textEdit_focused("Report");

    ui->textEdit_Report->zoomIn();
    ZoomReport++;
}

//zoom in target text
void MainWindow::on_actionZoom_in_target_text_triggered()
{
    textEdit_focused("TargetText");

    ui->textEdit_TargetText->zoomIn();
    ZoomTargetText++;
}

//zoom in base text
void MainWindow::on_actionZoom_in_base_text_triggered()
{
    textEdit_focused("BaseText");

    ui->textEdit_BaseText->zoomIn();
    ZoomBaseText++;
}

// ///////////////////////////////////////////////////
//Option "Zoom out"
// //////////////////////////////////////////////////
//zoom out all windows
void MainWindow::on_actionZoom_out_all_windows_triggered()
{
    ui->textEdit_Questions->zoomOut();
    ui->textEdit_Report->zoomOut();
    ui->textEdit_TargetText->zoomOut();
    ui->textEdit_BaseText->zoomOut();

    ZoomQuestions--;
    ZoomReport--;
    ZoomTargetText--;
    ZoomBaseText--;
}

//zoom out questions
void MainWindow::on_actionZoom_out_questions_triggered()
{
    textEdit_focused("Questions");

    ui->textEdit_Questions->zoomOut();
    ZoomQuestions--;
}

//zoom out report
void MainWindow::on_actionZoom_out_report_triggered()
{
    textEdit_focused("Report");

    ui->textEdit_Report->zoomOut();
    ZoomReport--;
}

//zoom out target text
void MainWindow::on_actionZoom_out_target_text_triggered()
{
    textEdit_focused("TargetText");

    ui->textEdit_TargetText->zoomOut();
    ZoomTargetText--;
}

//zoom out base text
void MainWindow::on_actionZoom_out_base_text_triggered()
{
    textEdit_focused("BaseText");

    ui->textEdit_BaseText->zoomOut();
    ZoomBaseText--;
}

// //////////////////////////////////////////////
//Option "Font"
// //////////////////////////////////////////////
//changing font in questions
void MainWindow::on_actionFont_of_questions_triggered()
{
    textEdit_focused("Questions");

    bool ok;
    QFont font = QFontDialog::getFont(&ok, this);
    if (ok) {
        ui->textEdit_Questions->setFont(font);
    } else return;
}

//changing font in report
void MainWindow::on_actionFont_of_report_triggered()
{
    textEdit_focused("Report");

    bool ok;
    QFont font = QFontDialog::getFont(&ok, this);
    if (ok) {
        ui->textEdit_Report->setFont(font);
    } else return;
}

//changing font in target text
void MainWindow::on_actionFont_of_target_text_triggered()
{
    textEdit_focused("TargetText");

    bool ok;
    QFont font = QFontDialog::getFont(&ok, this);
    if (ok) {
        ui->textEdit_TargetText->setFont(font);
    } else return;
}

//changing font in base text
void MainWindow::on_actionFont_of_base_text_triggered()
{
    textEdit_focused("BaseText");

    bool ok;
    QFont font = QFontDialog::getFont(&ok, this);
    if (ok) {
        ui->textEdit_BaseText->setFont(font);
    } else return;
}

// ///////////////////////////////////////////////////////////
//Option "Text color"
// ///////////////////////////////////////////////////////////
//text color in questions
void MainWindow::on_actionText_color_of_questions_triggered()
{
    textEdit_focused("Questions");

    QColor color = QColorDialog::getColor(Qt::white, this, tr("Choose Color"));
    if(color.isValid()) {
        ui->textEdit_Questions->setTextColor(color);
    }
}

//text color in report
void MainWindow::on_actionText_color_of_report_triggered()
{
    textEdit_focused("Report");

    QColor color = QColorDialog::getColor(Qt::white, this, tr("Choose Color"));
    if(color.isValid()) {
        ui->textEdit_Report->setTextColor(color);
    }
}

//text color in target text
void MainWindow::on_actionText_color_of_target_text_triggered()
{
    textEdit_focused("TargetText");

    QColor color = QColorDialog::getColor(Qt::white, this, tr("Choose Color"));
    if(color.isValid()) {
        ui->textEdit_TargetText->setTextColor(color);
    }
}

//text color in base text
void MainWindow::on_actionText_color_of_base_text_triggered()
{
    textEdit_focused("BaseText");

    QColor color = QColorDialog::getColor(Qt::white, this, tr("Choose Color"));
    if(color.isValid()) {
        ui->textEdit_BaseText->setTextColor(color);
    }
}

// ////////////////////////////////////////////////////////////
//Option "Background color"
// ////////////////////////////////////////////////////////////
//text background color in questions
void MainWindow::on_actionBackground_color_of_questions_triggered()
{
    textEdit_focused("Questions");

    QColor color = QColorDialog::getColor(Qt::white, this, tr("Choose Color"));
    if(color.isValid()) {
        ui->textEdit_Questions->setTextBackgroundColor(color);
    }
}

//text background color in report
void MainWindow::on_actionBackground_color_of_report_triggered()
{
    textEdit_focused("Report");

    QColor color = QColorDialog::getColor(Qt::white, this, tr("Choose Color"));
    if(color.isValid()) {
        ui->textEdit_Report->setTextBackgroundColor(color);
    }
}

//text background color in target text
void MainWindow::on_actionBackground_color_of_target_text_triggered()
{
    textEdit_focused("TargetText");

    QColor color = QColorDialog::getColor(Qt::white, this, tr("Choose Color"));
    if(color.isValid()) {
        ui->textEdit_TargetText->setTextBackgroundColor(color);
    }
}

//text background color in base text
void MainWindow::on_actionBackground_color_of_base_text_triggered()
{
    textEdit_focused("BaseText");

    QColor color = QColorDialog::getColor(Qt::white, this, tr("Choose Color"));
    if(color.isValid()) {
        ui->textEdit_BaseText->setTextBackgroundColor(color);
    }
}

// /////////////////////////////////////////////////
//Option "Palette color"
// /////////////////////////////////////////////////
//window background color in all windows
void MainWindow::on_actionPalette_color_of_all_windows_triggered()
{
    QColor color = QColorDialog::getColor(Qt::white, this, tr("Choose Color"));
    QPalette palette;
    palette.setColor(QPalette::Base, color);
    if(color.isValid()) {
        ui->textEdit_Questions->setPalette(palette);
        ui->textEdit_Report->setPalette(palette);
        ui->textEdit_TargetText->setPalette(palette);
        ui->textEdit_BaseText->setPalette(palette);
    }
}

//window background color in questions
void MainWindow::on_actionPalette_color_of_questions_triggered()
{
    textEdit_focused("Questions");

    QColor color = QColorDialog::getColor(Qt::white, this, tr("Choose Color"));
    QPalette palette;
    palette.setColor(QPalette::Base, color);
    if(color.isValid()) {
        ui->textEdit_Questions->setPalette(palette);
    }
}

//window background color in report
void MainWindow::on_actionPalette_color_of_report_triggered()
{
    textEdit_focused("Report");

    QColor color = QColorDialog::getColor(Qt::white, this, tr("Choose Color"));
    QPalette palette;
    palette.setColor(QPalette::Base, color);
    if(color.isValid()) {
        ui->textEdit_Report->setPalette(palette);
    }
}

//window background color in target text
void MainWindow::on_actionPalette_color_of_target_text_triggered()
{
    textEdit_focused("TargetText");

    QColor color = QColorDialog::getColor(Qt::white, this, tr("Choose Color"));
    QPalette palette;
    palette.setColor(QPalette::Base, color);
    if(color.isValid()) {
        ui->textEdit_TargetText->setPalette(palette);
    }
}

//window background color in base text
void MainWindow::on_actionPalette_color_of_base_text_triggered()
{
    textEdit_focused("BaseText");

    QColor color = QColorDialog::getColor(Qt::white, this, tr("Choose Color"));
    QPalette palette;
    palette.setColor(QPalette::Base, color);
    if(color.isValid()) {
        ui->textEdit_BaseText->setPalette(palette);
    }
}


// //////////////////////////////////////////////////////////
//Formatted text
// //////////////////////////////////////////////////////////

void MainWindow::formatted(QString document_name) {
    QString file = "";

    if (document_name == "TargetText") {
        file = FilenameTargetText;
    }
    else if (document_name == "BaseText") {
        file = FilenameBaseText;
    }
    else if (document_name == "Questions") {
        file = FilenameQuestions;
    }
    else return;

    if (file.isEmpty() || file.isNull()) {
        file = QFileDialog::getOpenFileName(this, tr("Open the file"), "",
                                            "Text files (*.txt);;Paratext files (*.sfm *.SFM);;\
HTML files (*.html *.htm)");
    }

    if(!file.isEmpty())
    {
        QFile FileText(file);
        if(FileText.open(QFile::ReadOnly | QFile::Text))
        {
            QTextStream in(&FileText);
            in.setCodec("UTF-8");
            QString text = in.readAll();
            FileText.close();
            QString text_converted = converter_to_html(text);

            if (document_name == "TargetText") {
                ui->textEdit_TargetText->setHtml(text_converted);
                FilenameTargetText = file;
            }
            else if (document_name == "BaseText") {
                ui->textEdit_BaseText->setHtml(text_converted);
                FilenameBaseText = file;
            }
            else if (document_name == "Questions") {
                ui->textEdit_Questions->setHtml(text_converted);
                FilenameQuestions = file;
            }
        }
    }
}

// /////////////////////////////////////////////////////
//Unformatted text
// /////////////////////////////////////////////////////

void MainWindow::unformatted(QString document_name)
{
    QString file = "";

    if (document_name == "TargetText") {
        file = FilenameTargetText;
    }
    else if (document_name == "BaseText") {
        file = FilenameBaseText;
    }
    else if (document_name == "Questions") {
        file = FilenameQuestions;
    }
    else return;

    if (file.isEmpty() || file.isNull()) {
        file = QFileDialog::getOpenFileName(this, tr("Open the file"), "",
                                            "Text files (*.txt);;Paratext files (*.sfm *.SFM);;\
HTML files (*.html *.htm)");
    }

    if(!file.isEmpty())
    {
        QFile FileText(file);
        if(FileText.open(QFile::ReadOnly | QFile::Text))
        {
            QTextStream in(&FileText);
            in.setCodec("UTF-8");
            QString text = in.readAll();

            FileText.close();

            if (document_name == "TargetText") {
                ui->textEdit_TargetText->setPlainText(text);
                FilenameTargetText = file;
            }
            else if (document_name == "BaseText") {
                ui->textEdit_BaseText->setPlainText(text);
                FilenameBaseText = file;
            }
            else if (document_name == "Questions") {
                ui->textEdit_Questions->setPlainText(text);
                FilenameQuestions = file;
            }

        }
    }
}

// ///////////////////////////////////////////////////////
//Option "Target text"
// ///////////////////////////////////////////////////////
//changing target text to formatted
void MainWindow::on_actionFormatted_target_text_triggered()
{
    textEdit_focused("TargetText");
    formatted("TargetText");
}

//changing target text to unformatted
void MainWindow::on_actionUnformatted_target_text_triggered()
{
    textEdit_focused("TargetText");
    unformatted("TargetText");
}

// ////////////////////////////////////////////////////
//Option "Base text"
// ////////////////////////////////////////////////////
//changing base text to formatted
void MainWindow::on_actionFormatted_base_text_triggered()
{
    textEdit_focused("BaseText");
    formatted ("BaseText");
}

//changing base text to unformatted
void MainWindow::on_actionUnformatted_base_text_triggered()
{
    textEdit_focused("BaseText");
    unformatted("BaseText");
}

// //////////////////////////////////////////////////////
//Option "Questions"
// //////////////////////////////////////////////////////
//changing questions to formatted according to Transcelerator
void MainWindow::on_actionFormatted_Transcelerator_triggered()
{
    textEdit_focused("Questions");

    QString file = FilenameQuestions;

    if (FilenameQuestions.isEmpty() || FilenameQuestions.isNull()) {
        file = QFileDialog::getOpenFileName(this, tr("Open the file"), "",
                                            "Text files (*.txt);;Paratext files (*.sfm *.SFM);;\
HTML files (*.html *.htm)");
    }

    if(!FilenameQuestions.isEmpty())
    {
        QFile FileText(file);
        if(FileText.open(QFile::ReadOnly | QFile::Text))
        {
            QTextStream in(&FileText);
            in.setCodec("UTF-8");
            QString text = in.readAll();
            FileText.close();
            QString text_converted = converter_from_transcelerator(text);

            ui->textEdit_Questions->setHtml(text_converted);
            FilenameQuestions = file;
        }
    }
}

//changing questions to formatted
void MainWindow::on_actionFormatted_questions_triggered()
{
    textEdit_focused("Questions");
    formatted ("Questions");
}

//changing questions to unformatted
void MainWindow::on_actionUnformatted_questions_triggered()
{
    textEdit_focused("Questions");
    unformatted("Questions");
}

// ////////////////////////////////////////////////////////
//Option "Encoding"
// ////////////////////////////////////////////////////////
//encoding of questions
void MainWindow::on_actionEncoding_of_questions_triggered()
{
    textEdit_focused("Questions");
    change_encoding("Questions");
    open_questions(FilenameQuestions);
}

//encoding of report
void MainWindow::on_actionEncoding_of_report_triggered()
{
    textEdit_focused("Report");
    change_encoding("Report");
    open_report(FilenameReport);
}

//encoding of target text
void MainWindow::on_actionEncoding_of_target_text_triggered()
{
    textEdit_focused("TargetText");
    change_encoding("TargetText");
    open_target_text(FilenameTargetText);
}

//encoding of base text
void MainWindow::on_actionEncoding_of_base_text_triggered()
{
    textEdit_focused("BaseText");
    change_encoding("BaseText");
    open_base_text(FilenameBaseText);
}

//changing encoding
void MainWindow::change_encoding(QString document_name) {
    QStringList items;
    items << "UTF-8" << "Unicode (UTF-16)" << tr("Windows-1251 (Cyrillic)") << tr("KOI8-R (Cyrillic)") <<
             tr("CP866 / IBM866 (Cyrillic)") << tr("ISO 8859-5 (Cyrillic)") <<
             tr("Windows-1250 (Central European)") << tr("Windows-1252 (Western)") <<
             tr("Windows-1253 (Greek)") << tr("Windows-1254 (Turkish)") <<
             tr("Windows-1255 (Hebrew)") << tr("Windows-1256 (Arabic)") << tr("Windows-1257 (Baltic)") <<
             "ISO 8859-1 (Latin-1)" << "ISO 8859-2 (Latin-2)" <<
             "ISO 8859-3 (Latin-3)" << "ISO 8859-4 (Latin-4)" <<
             "ISO 8859-9 (Latin-5)" << "ISO 8859-10 (Latin-6)" <<
             "ISO 8859-13 (Latin-7)" << "ISO 8859-14 (Latin-8)" <<
             "ISO 8859-15 (Latin-9)" << "ISO 8859-16 (Latin-10)" <<
             tr("ISO 8859-6 (Arabic)") << tr("ISO 8859-7 (Greek)") << tr("ISO 8859-8 (Hebrew)");

    bool ok;
    QString item = QInputDialog::getItem(this, tr("Changing encoding"), tr("Choose encoding:"),
                                         items, 0, false, &ok);

    if (ok && !item.isEmpty()) {
        QString EncodingDocument;

        if (item == "UTF-8") EncodingDocument = "UTF-8";
        else if (item == "Unicode (UTF-16)") EncodingDocument = "Unicode";
        else if (item == tr("Windows-1251 (Cyrillic)")) EncodingDocument = "Windows-1251";
        else if (item == tr("KOI8-R (Cyrillic)")) EncodingDocument = "KOI8-R";
        else if (item == tr("CP866 / IBM866 (Cyrillic)")) EncodingDocument = "IBM866";
        else if (item == tr("ISO 8859-5 (Cyrillic)")) EncodingDocument = "ISO 8859-5";
        else if (item == tr("Windows-1250 (Central European)")) EncodingDocument = "Windows-1250";
        else if (item == tr("Windows-1252 (Western)")) EncodingDocument = "Windows-1252";
        else if (item == tr("Windows-1253 (Greek)")) EncodingDocument = "Windows-1253";
        else if (item == tr("Windows-1254 (Turkish)")) EncodingDocument = "Windows-1254";
        else if (item == tr("Windows-1255 (Hebrew)")) EncodingDocument = "Windows-1255";
        else if (item == tr("Windows-1256 (Arabic)")) EncodingDocument = "Windows-1256";
        else if (item == tr("Windows-1257 (Baltic)")) EncodingDocument = "Windows-1257";
        else if (item == "ISO 8859-1 (Latin-1)") EncodingDocument = "ISO 8859-1";
        else if (item == "ISO 8859-2 (Latin-2)") EncodingDocument = "ISO 8859-2";
        else if (item == "ISO 8859-3 (Latin-3)") EncodingDocument = "ISO 8859-3";
        else if (item == "ISO 8859-4 (Latin-4)") EncodingDocument = "ISO 8859-4";
        else if (item == "ISO 8859-9 (Latin-5)") EncodingDocument = "ISO 8859-9";
        else if (item == "ISO 8859-10 (Latin-6)") EncodingDocument = "ISO 8859-10";
        else if (item == "ISO 8859-13 (Latin-7)") EncodingDocument = "ISO 8859-13";
        else if (item == "ISO 8859-14 (Latin-8)") EncodingDocument = "ISO 8859-14";
        else if (item == "ISO 8859-15 (Latin-9)") EncodingDocument = "ISO 8859-15";
        else if (item == "ISO 8859-16 (Latin-10)") EncodingDocument = "ISO 8859-16";
        else if (item == tr("ISO 8859-6 (Arabic)")) EncodingDocument = "ISO 8859-6";
        else if (item == tr("ISO 8859-7 (Greek)")) EncodingDocument = "ISO 8859-7";
        else if (item == tr("ISO 8859-8 (Hebrew)")) EncodingDocument = "ISO 8859-8";
        else return;

        if (document_name == "Report") EncodingReport = EncodingDocument;
        else if (document_name == "Questions") EncodingQuestions = EncodingDocument;
        else if (document_name == "TargetText") EncodingTargetText = EncodingDocument;
        else if (document_name == "BaseText") EncodingBaseText = EncodingDocument;
        else if (document_name == "HTML") EncodingHTML = EncodingDocument;

        else return;
    }
}


// ///////////////////////////////////////////////////////////////
//MENU INSERT
// ///////////////////////////////////////////////////////////////

// //////////////////////////////////////////////////////
//Option "Insert book title"
// //////////////////////////////////////////////////////
//inserting book title into report
void MainWindow::on_actionInsert_book_title_triggered()
{
    textEdit_focused("Report");

    QString book_title = QInputDialog::getText(this, tr("Book title"), tr("Enter book title:"),
                                               QLineEdit::Normal);
    ui->textEdit_Report->insertPlainText(book_title + "\n");
}

// /////////////////////////////////////////////////////
//Option "Insert number"
// /////////////////////////////////////////////////////
//inserting chapter number into report
void MainWindow::on_actionInsert_chapter_number_triggered()
{
    textEdit_focused("Report");

    int chapter_number = QInputDialog::getInt(this, tr("Chapter number"), tr("Enter chapter number:"),
                                              QLineEdit::Normal, 1);
    ui->textEdit_Report->insertPlainText(tr("Chapter") + " " + QString::number(chapter_number) + "\n");
}

//inserting chapters numbers into report
void MainWindow::on_actionInsert_chapters_numbers_triggered()
{
    textEdit_focused("Report");

    int chapter_number_first = QInputDialog::getInt(this, tr("First chapter number"),
                                                    tr("Enter first chapter number:"), QLineEdit::Normal, 1);
    int chapter_number_last = QInputDialog::getInt(this, tr("Last chapter number"),
                                                   tr("Enter last chapter number:"), QLineEdit::Normal, 2);
    ui->textEdit_Report->insertPlainText(tr("Chapters") + " " + QString::number(chapter_number_first) + "-"
                                         + QString::number(chapter_number_last) + "\n");
}

//inserting verse number into report
void MainWindow::on_actionInsert_verse_number_triggered()
{
    textEdit_focused("Report");

    int verse_number = QInputDialog::getInt(this, tr("Verse number"), tr("Enter verse number:"),
                                            QLineEdit::Normal, 1);
    ui->textEdit_Report->insertPlainText(tr("Verse") + " " + QString::number(verse_number) + "\n");
}

//inserting verses numbers into report
void MainWindow::on_actionInsert_verses_numbers_triggered()
{
    textEdit_focused("Report");

    int verse_number_first = QInputDialog::getInt(this, tr("First verse number"), tr("Enter first verse number:"),
                                                  QLineEdit::Normal, 1);
    int verse_number_last = QInputDialog::getInt(this, tr("Last verse number"), tr("Enter last verse number:"),
                                                 QLineEdit::Normal, 2);
    ui->textEdit_Report->insertPlainText(tr("Verses") + " " + QString::number(verse_number_first) + "-"
                                         + QString::number(verse_number_last) + "\n");
}

// ////////////////////////////////////////////////////////
//Option "Insert question number"
// ////////////////////////////////////////////////////////
//inserting question number into report
void MainWindow::on_actionInsert_question_number_triggered()
{
    textEdit_focused("Report");

    int question_number = QInputDialog::getInt(this, tr("Question number"), tr("Enter question number:"),
                                               QLineEdit::Normal, 1);
    ui->textEdit_Report->insertPlainText(tr("Question") + " " + QString::number(question_number) + "\n");
}

// //////////////////////////////////////////////////////////
//Option "Insert time"
// //////////////////////////////////////////////////////////
//inserting time of work start
void MainWindow::on_actionInsert_time_of_work_start_triggered()
{
    textEdit_focused("Report");

    QDateTime work_start = QDateTime::currentDateTime();
    ui->textEdit_Report->insertPlainText(tr("Work began at:") + " " + work_start.toString("dd/MM/yyyy hh:mm:ss")
                                         + "\n");
}

//inserting time of work end
void MainWindow::on_actionInsert_time_of_work_end_triggered()
{
    textEdit_focused("Report");

    QDateTime work_end = QDateTime::currentDateTime();
    ui->textEdit_Report->insertPlainText(tr("Work ended at:") + " " + work_end.toString("dd/MM/yyyy hh:mm:ss")
                                         + "\n");
}

// ////////////////////////////////////////////////////////////
//Option "Insert report title"
// ////////////////////////////////////////////////////////////
//inserting report title
void MainWindow::on_actionInsert_report_title_triggered()
{
    textEdit_focused("Report");

    ui->textEdit_Report->insertHtml(tr("<b>The comprehension testing report</b><br>") + "\n");
}

// //////////////////////////////////////////////////////////////////////////////
//MENU RESPONDENT'S ANSWER"
// //////////////////////////////////////////////////////////////////////////////

//inserting "Respondent answered correctly" into report
void MainWindow::on_actionRespondent_answered_correctly_triggered()
{
    textEdit_focused("Report");

    ui->textEdit_Report->insertPlainText(tr("Respondent answered correctly") + ".\n");
}

//inserting "Respondent answered correctly but there are comments" into report
void MainWindow::on_actionRespondent_answered_correctly_but_there_are_comments_triggered()
{
    textEdit_focused("Report");

    ui->textEdit_Report->insertPlainText(tr("Respondent answered correctly but there are comments") + ".\n");
}

//inserting "Respondent answered partially correctly" into report
void MainWindow::on_actionRespondent_answered_partially_correctly_triggered()
{
    textEdit_focused("Report");

    ui->textEdit_Report->insertPlainText(tr("Respondent answered partially correctly") + ".\n");
}

//inserting "Respondent answered partially correctly and there are comments" into report
void MainWindow::on_actionRespondent_answered_partially_correctly_and_there_are_comments_triggered()
{
    textEdit_focused("Report");

    ui->textEdit_Report->insertPlainText(tr("Respondent answered partially correctly and there are comments")
                                         + ".\n");
}

//inserting "Respondent answered incorrectly" into report
void MainWindow::on_actionRespondent_answered_incorrectly_triggered()
{
    textEdit_focused("Report");

    ui->textEdit_Report->insertPlainText(tr("Respondent answered incorrectly") + ".\n");
}

//inserting "Respondent answered incorrectly and there are comments" into report
void MainWindow::on_actionRespondent_answered_incorrectly_and_there_are_comments_triggered()
{
    textEdit_focused("Report");

    ui->textEdit_Report->insertPlainText(tr("Respondent answered incorrectly and there are comments") + ".\n");
}

//inserting "Respondent was unable to answer" into report
void MainWindow::on_actionRespondent_was_unable_to_answer_triggered()
{
    textEdit_focused("Report");

    ui->textEdit_Report->insertPlainText(tr("Respondent was unable to answer") + ".\n");
}

//inserting "Respondent was unable to answer and there are comments" into report
void MainWindow::on_actionRespondent_was_unable_to_answer_and_there_are_comments_triggered()
{
    textEdit_focused("Report");

    ui->textEdit_Report->insertPlainText(tr("Respondent was unable to answer and there are comments") + ".\n");
}

//inserting "Tester did not ask the question because there was no need" into report
void MainWindow::on_actionTester_did_not_ask_the_question_because_there_was_no_need_triggered()
{
    textEdit_focused("Report");

    ui->textEdit_Report->insertPlainText(tr("Tester did not ask the question because there was no need") + ".\n");
}

// ////////////////////////////////////////////////////////////////////
//MENU COMMENTS
// ////////////////////////////////////////////////////////////////////

//inserting "Unknown word, better to replace it" into report
void MainWindow::on_actionUnknown_word_better_to_replace_it_triggered()
{
    textEdit_focused("Report");

    ui->textEdit_Report->insertPlainText(tr("Unknown word, better to replace it") + ".\n");
}

//inserting "Too long and complicated sentence" into report
void MainWindow::on_actionToo_long_and_complicated_sentence_triggered()
{
    textEdit_focused("Report");

    ui->textEdit_Report->insertPlainText(tr("Too long and complicated sentence") + ".\n");
}

//inserting "Pronoun antecedent is unclear" into report
void MainWindow::on_actionPronoun_antecedent_is_unclear_triggered()
{
    textEdit_focused("Report");

    ui->textEdit_Report->insertPlainText(tr("Pronoun antecedent is unclear") + ".\n");
}

//inserting "Better to change the word order in the sentence" into report
void MainWindow::on_actionBetter_to_change_the_word_order_in_the_sentence_triggered()
{
    textEdit_focused("Report");

    ui->textEdit_Report->insertPlainText(tr("Better to change the word order in the sentence") + ".\n");
}

//inserting "Figurative expression is unclear" into report
void MainWindow::on_actionFigurative_expression_is_unclear_triggered()
{
    textEdit_focused("Report");

    ui->textEdit_Report->insertPlainText(tr("Figurative expression is unclear") + ".\n");
}

//inserting "Connection between sentences is unclear" into report
void MainWindow::on_actionConnection_between_sentences_is_unclear_triggered()
{
    textEdit_focused("Report");

    ui->textEdit_Report->insertPlainText(tr("Connection between sentences is unclear") + ".\n");
}

//inserting "Connection between words is unclear (all the words are clear)" into report
void MainWindow::on_actionConnection_between_words_is_unclear_all_the_words_are_clear_triggered()
{
    textEdit_focused("Report");

    ui->textEdit_Report->insertPlainText(tr("Connection between words is unclear (all the words are clear)")
                                         + ".\n");
}

//inserting "Main thought is unclear or meaning is unclear at all" into report
void MainWindow::on_actionMain_thought_is_unclear_triggered()
{
    textEdit_focused("Report");

    ui->textEdit_Report->insertPlainText(tr("Main thought is unclear or meaning is unclear at all") + ".\n");
}

//inserting "There is something unappropriate for the local culture" into report
void MainWindow::on_actionThere_is_something_unappropriate_for_the_local_culture_triggered()
{
    textEdit_focused("Report");

    ui->textEdit_Report->insertPlainText(tr("There is something unappropriate for the local culture") + ".\n");
}

//inserting "Respondent did not like something in the text" into report
void MainWindow::on_actionRespondent_did_not_like_something_in_the_text_triggered()
{
    textEdit_focused("Report");

    ui->textEdit_Report->insertPlainText(tr("Respondent did not like something in the text") + ".\n");
}

//inserting "Respondent liked something in the text" into report
void MainWindow::on_actionRespondent_liked_something_in_the_text_triggered()
{
    textEdit_focused("Report");

    ui->textEdit_Report->insertPlainText(tr("Respondent liked something in the text") + ".\n");
}

//inserting "Another comment or problem" into report
void MainWindow::on_actionAnother_comment_or_problem_triggered()
{
    textEdit_focused("Report");

    ui->textEdit_Report->insertPlainText(tr("Another comment or problem") + ".\n");
}

// ////////////////////////////////////////////////////////////////////////
//MENU OPTIONS
// ////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////
//Option "Interface language"
// ////////////////////////////////////////////////////////////////
//setting English language of interface
void MainWindow::on_actionEnglish_triggered()
{
    language = "English";
    QMessageBox::information(this, tr("Language change"),
                             tr("Interface language will be changed after the program restarts."));
}

//setting Russian language of interface
void MainWindow::on_actionRussian_triggered()
{
    language = "Russian";
    QMessageBox::information(this, tr("Language change"),
                             tr("Interface language will be changed after the program restarts."));
}

// ///////////////////////////////////////////////////////////////
//Option "Insert time automatically"
// ///////////////////////////////////////////////////////////////
//setting automatic insertion of time
void MainWindow::on_actionInsert_time_automatically_triggered()
{
    textEdit_focused("Report");

    QMessageBox* messagebox = new QMessageBox (QMessageBox::Question,
                                               tr("Insert time"),
                                               tr("Insert time of beginning and end of work automatically?"),
                                               QMessageBox::Yes | QMessageBox::No, this);
    messagebox->setButtonText(QMessageBox::Yes, tr("Yes"));
    messagebox->setButtonText(QMessageBox::No, tr("No"));
    int n = messagebox->exec();
    delete messagebox;

    if (n == QMessageBox::Yes) {
        TimeAuto = true;
        QMessageBox::information(this, tr("Insert time"),
                                 tr("Time of beginning and end of work will be inserted automatically."));
     } else {
        TimeAuto = false;
        QMessageBox::information(this, tr("Insert time"),
                                 tr("Time of beginning and end of work will not be inserted automatically."));
    }
}

// //////////////////////////////////////////////////////////////////////
//Option "Insert title automatically"
// //////////////////////////////////////////////////////////////////////
//setting option to insert title into new report file automatically
void MainWindow::on_actionInsert_title_automatically_triggered()
{
    textEdit_focused("Report");

    QMessageBox* messagebox = new QMessageBox (QMessageBox::Question,
                                               tr("Insert title"),
                                               tr("Insert title into new report file automatically?"),
                                               QMessageBox::Yes | QMessageBox::No, this);
    messagebox->setButtonText(QMessageBox::Yes, tr("Yes"));
    messagebox->setButtonText(QMessageBox::No, tr("No"));
    int n = messagebox->exec();
    delete messagebox;

    if (n == QMessageBox::Yes) {
        TitleAuto = true;
        QMessageBox::information(this, tr("Insert title"),
                                 tr("Title will be inserted into new report file automatically."));
     } else {
        TitleAuto = false;
        QMessageBox::information(this, tr("Insert title"),
                                 tr("Title will not be inserted into new report file automatically."));
    }
}

// /////////////////////////////////////////////////////
//Option "Load files automatically"
// /////////////////////////////////////////////////////
//setting option to load or not load files automatically at start
void MainWindow::on_actionLoad_files_automatically_triggered()
{
    QMessageBox* messagebox = new QMessageBox (QMessageBox::Question,
                                               tr("Files loading"),
                                               tr("By default, files are loaded automatically\
 at start of the program. Do you want to stop automatic loading?"),
                                               QMessageBox::Yes | QMessageBox::No, this);
    messagebox->setButtonText(QMessageBox::Yes, tr("Yes"));
    messagebox->setButtonText(QMessageBox::No, tr("No"));
    int n = messagebox->exec();
    delete messagebox;

    if (n == QMessageBox::Yes) {
        FilesNotLoadAuto = true;
        QMessageBox::information(this, tr("Files loading"),
                                 tr("Files will not be loaded automatically at start."));
     } else {
        FilesNotLoadAuto = false;
        QMessageBox::information(this, tr("Files loading"),
                                 tr("Files will be loaded automatically at start."));
    }
}


// //////////////////////////////////////////////////////
//Option "USFM -> HTML"
// //////////////////////////////////////////////////////

//method for USFM -> HTML (without formatting)
void MainWindow::USFM_to_HTML(QString document_name) {
    QString file = "";

    if (document_name == "TargetText") {
        file = FilenameTargetText;
    }
    else if (document_name == "BaseText") {
        file = FilenameBaseText;
    }
    else if (document_name == "Questions") {
        file = FilenameQuestions;
    }
    else return;

    if (file.isEmpty() || file.isNull()) {
        file = QFileDialog::getOpenFileName(this, tr("Open the file"), "",
                                            "Paratext files (*.sfm *.SFM)");
    }

    if(!file.isEmpty() && (file.endsWith(".sfm") || (file.endsWith(".SFM"))))
    {
        QFile FileText(file);
        if(FileText.open(QFile::ReadOnly | QFile::Text))
        {
            QTextStream in(&FileText);

            if (EncodingHTML == "UTF-8") in.setCodec("UTF-8");
            else if (EncodingHTML == "Unicode") in.setCodec("Unicode");
            else if (EncodingHTML == "Windows-1251") in.setCodec("Windows-1251");
            else if (EncodingHTML == "KOI8-R") in.setCodec("KOI8-R");
            else if (EncodingHTML == "IBM866") in.setCodec("IBM866");
            else if (EncodingHTML == "ISO 8859-5") in.setCodec("ISO 8859-5");
            else if (EncodingHTML == "Windows-1250") in.setCodec("Windows-1250");
            else if (EncodingHTML == "Windows-1252") in.setCodec("Windows-1252");
            else if (EncodingHTML == "Windows-1253") in.setCodec("Windows-1253");
            else if (EncodingHTML == "Windows-1254") in.setCodec("Windows-1254");
            else if (EncodingHTML == "Windows-1255") in.setCodec("Windows-1255");
            else if (EncodingHTML == "Windows-1256") in.setCodec("Windows-1256");
            else if (EncodingHTML == "Windows-1257") in.setCodec("Windows-1257");
            else if (EncodingHTML == "ISO 8859-1") in.setCodec("ISO 8859-1");
            else if (EncodingHTML == "ISO 8859-2") in.setCodec("ISO 8859-2");
            else if (EncodingHTML == "ISO 8859-3") in.setCodec("ISO 8859-3");
            else if (EncodingHTML == "ISO 8859-4") in.setCodec("ISO 8859-4");
            else if (EncodingHTML == "ISO 8859-9") in.setCodec("ISO 8859-9");
            else if (EncodingHTML == "ISO 8859-10") in.setCodec("ISO 8859-10");
            else if (EncodingHTML == "ISO 8859-13") in.setCodec("ISO 8859-13");
            else if (EncodingHTML == "ISO 8859-14") in.setCodec("ISO 8859-14");
            else if (EncodingHTML == "ISO 8859-15") in.setCodec("ISO 8859-15");
            else if (EncodingHTML == "ISO 8859-16") in.setCodec("ISO 8859-16");
            else if (EncodingHTML == "ISO 8859-6") in.setCodec("ISO 8859-6");
            else if (EncodingHTML == "ISO 8859-7") in.setCodec("ISO 8859-7");
            else if (EncodingHTML == "ISO 8859-8") in.setCodec("ISO 8859-8");
            else in.setCodec("UTF-8");

            QString text = in.readAll();
            FileText.close();

            QString text_converted = converter_to_html(text);

            if (document_name == "TargetText") {
                ui->textEdit_TargetText->setPlainText(text_converted);
                FilenameTargetText = file;
            }
            else if (document_name == "BaseText") {
                ui->textEdit_BaseText->setPlainText(text_converted);
                FilenameBaseText = file;
            }
            else if (document_name == "Questions") {
                ui->textEdit_Questions->setPlainText(text_converted);
                FilenameQuestions = file;
            }
        }
    }
}

//USFM to HTML of questions
void MainWindow::on_actionQuestionsUSFM_HTML_triggered()
{
    textEdit_focused("Questions");
    USFM_to_HTML("Questions");
}

//USFM to HTML of target text
void MainWindow::on_actionTarget_textUSFM_HTML_triggered()
{
    textEdit_focused("TargetText");
    USFM_to_HTML("TargetText");
}

//USFM to HTML of base text
void MainWindow::on_actionBase_textUSFM_HTML_triggered()
{
    textEdit_focused("BaseText");
    USFM_to_HTML("BaseText");
}

//changing encoding for USFM -> HTML
void MainWindow::on_actionEncoding_for_USFM_HTML_triggered()
{
    change_encoding("HTML");
}



// ///////////////////////////////////////////////////////////////////
//MENU ABOUT
// ///////////////////////////////////////////////////////////////////

//about program
void MainWindow::on_actionAbout_triggered()
{
    QString about_text;
    about_text  = tr("Program for comprehension testing of Bible texts") + "\n\n";

    about_text += tr("Copyright (C) 2021 Aleksandr Migunov") + "\n\n";

    about_text += "This program is free software; you can redistribute it and/or modify \
it under the terms of the GNU General Public License as published by \
the Free Software Foundation; either version 3 of the License, or \
(at your option) any later version. \n\n";

    about_text += "This program is distributed in the hope that it will be useful, \
but WITHOUT ANY WARRANTY; without even the implied warranty of \
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the \
GNU General Public License for more details. \n\n";

    about_text += "You should have received a copy of the GNU General Public License \
along with this program; if not, write to the Free Software \
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.";

    QMessageBox::about(this, tr("About Program"), about_text);
}

//about Qt
void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(0);
}


// ////////////////////////////////////////////////////////////////////
//TOOLBAR
// ////////////////////////////////////////////////////////////////////

//setting bold text
void MainWindow::on_actionTextBold_triggered()
{
    textEdit_focused("Report");

    if (TextBold == false) {
        ui->textEdit_Report->setFontWeight(QFont::Bold);
        TextBold = true;
    }
    else {
        ui->textEdit_Report->setFontWeight(QFont::Normal);
        TextBold = false;
    }
}

//setting italic text
void MainWindow::on_actionTextItalic_triggered()
{
    textEdit_focused("Report");

    if (TextItalic == false) {
        ui->textEdit_Report->setFontItalic(true);
        TextItalic = true;
    }
    else {
        ui->textEdit_Report->setFontItalic(false);
        TextItalic = false;
    }
}

//setting underlined text
void MainWindow::on_actionTextUnderline_triggered()
{
    textEdit_focused("Report");

    if (TextUnderline == false) {
        ui->textEdit_Report->setFontUnderline(true);
        TextUnderline = true;
    }
    else {
        ui->textEdit_Report->setFontUnderline(false);
        TextUnderline = false;
    }
}

//setting left alignment
void MainWindow::on_actionAlignLeft_triggered()
{
    textEdit_focused("Report");

    ui->textEdit_Report->setAlignment(Qt::AlignLeft);
}

//setting center alignment
void MainWindow::on_actionAlignCenter_triggered()
{
    textEdit_focused("Report");

    ui->textEdit_Report->setAlignment(Qt::AlignCenter);
}

//setting justify alignment
void MainWindow::on_actionAlignJustify_triggered()
{
    textEdit_focused("Report");

    ui->textEdit_Report->setAlignment(Qt::AlignJustify);
}

//setting right alignment
void MainWindow::on_actionAlignRight_triggered()
{
    textEdit_focused("Report");

    ui->textEdit_Report->setAlignment(Qt::AlignRight);
}

// ////////////////////////////////////////////////////////
//CONTEXT MENUS
// ////////////////////////////////////////////////////////

//setting contect menu for questions
void MainWindow::on_textEdit_Questions_customContextMenuRequested()
{
    textEdit_focused("Questions");

    QAction *actionOpen = new QAction(tr("Open"), this);
    connect(actionOpen, SIGNAL(triggered()), this, SLOT(on_actionOpen_questions_triggered()));

    QAction *actionClose = new QAction(tr("Close"), this);
    connect(actionClose, SIGNAL(triggered()), this, SLOT(on_actionClose_questions_triggered()));

    QAction *actionPrint = new QAction(tr("Print"), this);
    connect(actionPrint, SIGNAL(triggered()), this, SLOT(on_actionPrint_questions_triggered()));

    QAction *actionPrintPreview = new QAction(tr("Print Preview"), this);
    connect(actionPrintPreview, SIGNAL(triggered()), this, SLOT(on_actionPrint_preview_questions_triggered()));

    QAction *actionConvertToPDF = new QAction(tr("Convert To PDF"), this);
    connect(actionConvertToPDF, SIGNAL(triggered()), this, SLOT(on_actionConvert_questions_to_PDF_triggered()));

    QAction *actionConvertToODT = new QAction(tr("Convert To ODT"), this);
    connect(actionConvertToODT, SIGNAL(triggered()), this, SLOT(on_actionConvert_questions_to_ODT_triggered()));

    QAction *actionFormattedTranscelerator = new QAction(tr("Formatted (Transcelerator)"), this);
    connect(actionFormattedTranscelerator, SIGNAL(triggered()), this,
            SLOT(on_actionFormatted_Transcelerator_triggered()));

    QAction *actionFormatted = new QAction(tr("Formatted (USFM/Paratext)"), this);
    connect(actionFormatted, SIGNAL(triggered()), this, SLOT(on_actionFormatted_questions_triggered()));

    QAction *actionUnformatted = new QAction(tr("Unformatted (USFM/Paratext)"), this);
    connect(actionUnformatted, SIGNAL(triggered()), this, SLOT(on_actionUnformatted_questions_triggered()));

    QAction *actionEncoding = new QAction(tr("Encoding"), this);
    connect(actionEncoding, SIGNAL(triggered()), this, SLOT(on_actionEncoding_of_questions_triggered()));

    QAction *actionCopy = new QAction(tr("Copy"), this);
    connect(actionCopy, SIGNAL(triggered()), this, SLOT(on_actionCopy_from_questions_triggered()));

    QAction *actionCopyPaste = new QAction(tr("Copy and Paste into Report"), this);
    connect(actionCopyPaste, SIGNAL(triggered()), this, SLOT(on_actionCopy_and_paste_from_questions_triggered()));

    QAction *actionSelectAll = new QAction(tr("Select All"), this);
    connect(actionSelectAll, SIGNAL(triggered()), this, SLOT(on_actionSelect_all_from_questions_triggered()));

    QAction *actionFind = new QAction(tr("Find"), this);
    connect(actionFind, SIGNAL(triggered()), this, SLOT(on_actionFind_in_questions_triggered()));

    QAction *actionZoomIn = new QAction(tr("Zoom In"), this);
    connect(actionZoomIn, SIGNAL(triggered()), this, SLOT(on_actionZoom_in_questions_triggered()));

    QAction *actionZoomOut = new QAction(tr("Zoom Out"), this);
    connect(actionZoomOut, SIGNAL(triggered()), this, SLOT(on_actionZoom_out_questions_triggered()));

    QMenu *contextMenu = new QMenu(this);
    contextMenu->addAction(actionOpen);
    contextMenu->addAction(actionClose);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionPrint);
    contextMenu->addAction(actionPrintPreview);
    contextMenu->addAction(actionConvertToPDF);
    contextMenu->addAction(actionConvertToODT);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionFormattedTranscelerator);
    contextMenu->addAction(actionFormatted);
    contextMenu->addAction(actionUnformatted);
    contextMenu->addAction(actionEncoding);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionCopy);
    contextMenu->addAction(actionCopyPaste);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionSelectAll);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionFind);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionZoomIn);
    contextMenu->addAction(actionZoomOut);
    contextMenu->exec(QCursor::pos());
}

//setting context menu for report
void MainWindow::on_textEdit_Report_customContextMenuRequested()
{
    textEdit_focused("Report");

    QAction *actionNew = new QAction(tr("New"), this);
    connect(actionNew, SIGNAL(triggered()), this, SLOT(on_actionNew_report_triggered()));

    QAction *actionOpen = new QAction(tr("Open"), this);
    connect(actionOpen, SIGNAL(triggered()), this, SLOT(on_actionOpen_report_triggered()));

    QAction *actionSave = new QAction(tr("Save"), this);
    connect(actionSave, SIGNAL(triggered()), this, SLOT(on_actionSave_report_triggered()));

    QAction *actionSaveAs = new QAction(tr("Save As"), this);
    connect(actionSaveAs, SIGNAL(triggered()), this, SLOT(on_actionSave_report_as_triggered()));

    QAction *actionPrint = new QAction(tr("Print"), this);
    connect(actionPrint, SIGNAL(triggered()), this, SLOT(on_actionPrint_report_triggered()));

    QAction *actionPrintPreview = new QAction(tr("Print Preview"), this);
    connect(actionPrintPreview, SIGNAL(triggered()), this, SLOT(on_actionPrint_preview_report_triggered()));

    QAction *actionConvertToPDF = new QAction(tr("Convert To PDF"), this);
    connect(actionConvertToPDF, SIGNAL(triggered()), this, SLOT(on_actionConvert_report_to_PDF_triggered()));

    QAction *actionConvertToODT = new QAction(tr("Convert To ODT"), this);
    connect(actionConvertToODT, SIGNAL(triggered()), this, SLOT(on_actionConvert_report_to_ODT_triggered()));

    QAction *actionEncoding = new QAction(tr("Encoding"), this);
    connect(actionEncoding, SIGNAL(triggered()), this, SLOT(on_actionEncoding_of_report_triggered()));

    QAction *actionClose = new QAction(tr("Close"), this);
    connect(actionClose, SIGNAL(triggered()), this, SLOT(on_actionClose_report_triggered()));

    QAction *actionUndo = new QAction(tr("Undo"), this);
    connect(actionUndo, SIGNAL(triggered()), this, SLOT(on_actionUndo_triggered()));

    QAction *actionRedo = new QAction(tr("Redo"), this);
    connect(actionRedo, SIGNAL(triggered()), this, SLOT(on_actionRedo_triggered()));

    QAction *actionCut = new QAction(tr("Cut"), this);
    connect(actionCut, SIGNAL(triggered()), this, SLOT(on_actionCut_triggered()));

    QAction *actionPaste = new QAction(tr("Paste"), this);
    connect(actionPaste, SIGNAL(triggered()), this, SLOT(on_actionPaste_triggered()));

    QAction *actionDelete = new QAction(tr("Delete"), this);
    connect(actionDelete, SIGNAL(triggered()), this, SLOT(on_actionDelete_triggered()));

    QAction *actionSelectAll = new QAction(tr("Select All"), this);
    connect(actionSelectAll, SIGNAL(triggered()), this, SLOT(on_actionSelect_all_from_report_triggered()));

    QAction *actionCopy = new QAction(tr("Copy"), this);
    connect(actionCopy, SIGNAL(triggered()), this, SLOT(on_actionCopy_from_questions_triggered()));

    QAction *actionCopyPaste = new QAction(tr("Copy and Paste"), this);
    connect(actionCopyPaste, SIGNAL(triggered()), this, SLOT(on_actionCopy_and_paste_from_report_triggered()));

    QAction *actionFind = new QAction(tr("Find"), this);
    connect(actionFind, SIGNAL(triggered()), this, SLOT(on_actionFind_in_report_triggered()));

    QAction *actionFindReplace = new QAction(tr("Find and Replace"), this);
    connect(actionFindReplace, SIGNAL(triggered()), this, SLOT(on_actionFind_and_replace_triggered()));

    QAction *actionFindReplaceAll = new QAction(tr("Find and Replace All"), this);
    connect(actionFindReplaceAll, SIGNAL(triggered()), this, SLOT(on_actionFind_and_replace_all_triggered()));

    QAction *actionZoomIn = new QAction(tr("Zoom In"), this);
    connect(actionZoomIn, SIGNAL(triggered()), this, SLOT(on_actionZoom_in_report_triggered()));

    QAction *actionZoomOut = new QAction(tr("Zoom Out"), this);
    connect(actionZoomOut, SIGNAL(triggered()), this, SLOT(on_actionZoom_out_report_triggered()));

    QMenu *contextMenu = new QMenu(this);
    contextMenu->addAction(actionNew);
    contextMenu->addAction(actionOpen);
    contextMenu->addAction(actionSave);
    contextMenu->addAction(actionSaveAs);
    contextMenu->addAction(actionClose);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionPrint);
    contextMenu->addAction(actionPrintPreview);
    contextMenu->addAction(actionConvertToPDF);
    contextMenu->addAction(actionConvertToODT);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionEncoding);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionUndo);
    contextMenu->addAction(actionRedo);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionCut);
    contextMenu->addAction(actionCopy);
    contextMenu->addAction(actionPaste);
    contextMenu->addAction(actionCopyPaste);
    contextMenu->addAction(actionDelete);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionSelectAll);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionFind);
    contextMenu->addAction(actionFindReplace);
    contextMenu->addAction(actionFindReplaceAll);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionZoomIn);
    contextMenu->addAction(actionZoomOut);
    contextMenu->exec(QCursor::pos());
}

//setting context menu for target text
void MainWindow::on_textEdit_TargetText_customContextMenuRequested()
{
    textEdit_focused("TargetText");

    QAction *actionOpen = new QAction(tr("Open"), this);
    connect(actionOpen, SIGNAL(triggered()), this, SLOT(on_actionOpen_target_text_triggered()));

    QAction *actionClose = new QAction(tr("Close"), this);
    connect(actionClose, SIGNAL(triggered()), this, SLOT(on_actionClose_target_text_triggered()));

    QAction *actionPrint = new QAction(tr("Print"), this);
    connect(actionPrint, SIGNAL(triggered()), this, SLOT(on_actionPrint_target_text_triggered()));

    QAction *actionPrintPreview = new QAction(tr("Print Preview"), this);
    connect(actionPrintPreview, SIGNAL(triggered()), this, SLOT(on_actionPrint_preview_target_text_triggered()));

    QAction *actionConvertToPDF = new QAction(tr("Convert To PDF"), this);
    connect(actionConvertToPDF, SIGNAL(triggered()), this, SLOT(on_actionConvert_target_text_to_PDF_triggered()));

    QAction *actionConvertToODT = new QAction(tr("Convert To ODT"), this);
    connect(actionConvertToODT, SIGNAL(triggered()), this, SLOT(on_actionConvert_target_text_to_ODT_triggered()));

    QAction *actionFormatted = new QAction(tr("Formatted (USFM/Paratext)"), this);
    connect(actionFormatted, SIGNAL(triggered()), this, SLOT(on_actionFormatted_target_text_triggered()));

    QAction *actionUnformatted = new QAction(tr("Unformatted (USFM/Paratext)"), this);
    connect(actionUnformatted, SIGNAL(triggered()), this, SLOT(on_actionUnformatted_target_text_triggered()));

    QAction *actionEncoding = new QAction(tr("Encoding"), this);
    connect(actionEncoding, SIGNAL(triggered()), this, SLOT(on_actionEncoding_of_target_text_triggered()));

    QAction *actionCopy = new QAction(tr("Copy"), this);
    connect(actionCopy, SIGNAL(triggered()), this, SLOT(on_actionCopy_from_target_text_triggered()));

    QAction *actionCopyPaste = new QAction(tr("Copy and Paste into Report"), this);
    connect(actionCopyPaste, SIGNAL(triggered()), this, SLOT(on_actionCopy_and_paste_from_target_text_triggered()));

    QAction *actionSelectAll = new QAction(tr("Select All"), this);
    connect(actionSelectAll, SIGNAL(triggered()), this, SLOT(on_actionSelect_all_from_target_text_triggered()));

    QAction *actionFind = new QAction(tr("Find"), this);
    connect(actionFind, SIGNAL(triggered()), this, SLOT(on_actionFind_in_target_text_triggered()));

    QAction *actionZoomIn = new QAction(tr("Zoom In"), this);
    connect(actionZoomIn, SIGNAL(triggered()), this, SLOT(on_actionZoom_in_target_text_triggered()));

    QAction *actionZoomOut = new QAction(tr("Zoom Out"), this);
    connect(actionZoomOut, SIGNAL(triggered()), this, SLOT(on_actionZoom_out_target_text_triggered()));

    QMenu *contextMenu = new QMenu(this);
    contextMenu->addAction(actionOpen);
    contextMenu->addAction(actionClose);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionPrint);
    contextMenu->addAction(actionPrintPreview);
    contextMenu->addAction(actionConvertToPDF);
    contextMenu->addAction(actionConvertToODT);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionFormatted);
    contextMenu->addAction(actionUnformatted);
    contextMenu->addAction(actionEncoding);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionCopy);
    contextMenu->addAction(actionCopyPaste);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionSelectAll);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionFind);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionZoomIn);
    contextMenu->addAction(actionZoomOut);
    contextMenu->exec(QCursor::pos());
}

//setting context menu for base text
void MainWindow::on_textEdit_BaseText_customContextMenuRequested()
{
    textEdit_focused("BaseText");

    QAction *actionOpen = new QAction(tr("Open"), this);
    connect(actionOpen, SIGNAL(triggered()), this, SLOT(on_actionOpen_base_text_triggered()));

    QAction *actionClose = new QAction(tr("Close"), this);
    connect(actionClose, SIGNAL(triggered()), this, SLOT(on_actionClose_base_text_triggered()));

    QAction *actionPrint = new QAction(tr("Print"), this);
    connect(actionPrint, SIGNAL(triggered()), this, SLOT(on_actionPrint_base_text_triggered()));

    QAction *actionPrintPreview = new QAction(tr("Print Preview"), this);
    connect(actionPrintPreview, SIGNAL(triggered()), this, SLOT(on_actionPrint_preview_base_text_triggered()));

    QAction *actionConvertToPDF = new QAction(tr("Convert To PDF"), this);
    connect(actionConvertToPDF, SIGNAL(triggered()), this, SLOT(on_actionConvert_base_text_to_PDF_triggered()));

    QAction *actionConvertToODT = new QAction(tr("Convert To ODT"), this);
    connect(actionConvertToODT, SIGNAL(triggered()), this, SLOT(on_actionConvert_base_text_to_ODT_triggered()));

    QAction *actionFormatted = new QAction(tr("Formatted (USFM/Paratext)"), this);
    connect(actionFormatted, SIGNAL(triggered()), this, SLOT(on_actionFormatted_base_text_triggered()));

    QAction *actionUnformatted = new QAction(tr("Unformatted (USFM/Paratext)"), this);
    connect(actionUnformatted, SIGNAL(triggered()), this, SLOT(on_actionUnformatted_base_text_triggered()));

    QAction *actionEncoding = new QAction(tr("Encoding"), this);
    connect(actionEncoding, SIGNAL(triggered()), this, SLOT(on_actionEncoding_of_base_text_triggered()));

    QAction *actionCopy = new QAction(tr("Copy"), this);
    connect(actionCopy, SIGNAL(triggered()), this, SLOT(on_actionCopy_from_base_text_triggered()));

    QAction *actionCopyPaste = new QAction(tr("Copy and Paste into Report"), this);
    connect(actionCopyPaste, SIGNAL(triggered()), this, SLOT(on_actionCopy_and_paste_from_base_text_triggered()));

    QAction *actionSelectAll = new QAction(tr("Select All"), this);
    connect(actionSelectAll, SIGNAL(triggered()), this, SLOT(on_actionSelect_all_from_base_text_triggered()));

    QAction *actionFind = new QAction(tr("Find"), this);
    connect(actionFind, SIGNAL(triggered()), this, SLOT(on_actionFind_in_base_text_triggered()));

    QAction *actionZoomIn = new QAction(tr("Zoom In"), this);
    connect(actionZoomIn, SIGNAL(triggered()), this, SLOT(on_actionZoom_in_base_text_triggered()));

    QAction *actionZoomOut = new QAction(tr("Zoom Out"), this);
    connect(actionZoomOut, SIGNAL(triggered()), this, SLOT(on_actionZoom_out_base_text_triggered()));

    QMenu *contextMenu = new QMenu(this);
    contextMenu->addAction(actionOpen);
    contextMenu->addAction(actionClose);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionPrint);
    contextMenu->addAction(actionPrintPreview);
    contextMenu->addAction(actionConvertToPDF);
    contextMenu->addAction(actionConvertToODT);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionFormatted);
    contextMenu->addAction(actionUnformatted);
    contextMenu->addAction(actionEncoding);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionCopy);
    contextMenu->addAction(actionCopyPaste);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionSelectAll);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionFind);
    contextMenu->insertSeparator(0);
    contextMenu->addAction(actionZoomIn);
    contextMenu->addAction(actionZoomOut);
    contextMenu->exec(QCursor::pos());

}




