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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QMainWindow>
#include <QSettings>
#include <QCloseEvent>
#include <QPrinter>
#include <QTextCodec>
#include <QString>
#include <QWidget>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void readSettings();
    void writeSettings();
    void closeEvent(QCloseEvent *event);

    QString language;

private slots:
    void on_textEdit_Report_selectionChanged();

    void on_textEdit_Questions_selectionChanged();

    void on_textEdit_TargetText_selectionChanged();

    void on_textEdit_BaseText_selectionChanged();

    void on_actionOpen_questions_triggered();

    void on_actionOpen_report_triggered();

    void on_actionOpen_target_text_triggered();

    void on_actionOpen_base_text_triggered();

    void on_actionClose_questions_triggered();

    void on_actionClose_report_triggered();

    void on_actionClose_target_text_triggered();

    void on_actionClose_base_text_triggered();

    void on_actionNew_report_triggered();

    void on_actionSave_report_triggered();

    void on_actionSave_report_as_triggered();

    void on_actionPrint_questions_triggered();

    void on_actionPrint_report_triggered();

    void on_actionPrint_target_text_triggered();

    void on_actionPrint_base_text_triggered();

    void paintPreviewQuestions(QPrinter *printer);

    void on_actionPrint_preview_questions_triggered();

    void paintPreviewReport(QPrinter *printer);

    void on_actionPrint_preview_report_triggered();

    void paintPreviewTargetText(QPrinter *printer);

    void on_actionPrint_preview_target_text_triggered();

    void paintPreviewBaseText(QPrinter *printer);

    void on_actionPrint_preview_base_text_triggered();

    void on_actionConvert_questions_to_PDF_triggered();

    void on_actionConvert_report_to_PDF_triggered();

    void on_actionConvert_target_text_to_PDF_triggered();

    void on_actionConvert_base_text_to_PDF_triggered();

    void on_actionConvert_questions_to_ODT_triggered();

    void on_actionConvert_report_to_ODT_triggered();

    void on_actionConvert_target_text_to_ODT_triggered();

    void on_actionConvert_base_text_to_ODT_triggered();

    void on_actionExit_triggered();

    void on_actionUndo_triggered();

    void on_actionRedo_triggered();

    void on_actionCut_triggered();

    void on_actionCopy_from_questions_triggered();

    void on_actionCopy_from_report_triggered();

    void on_actionCopy_from_target_text_triggered();

    void on_actionCopy_from_base_text_triggered();

    void on_actionPaste_triggered();

    void on_actionCopy_and_paste_from_questions_triggered();

    void on_actionCopy_and_paste_from_report_triggered();

    void on_actionCopy_and_paste_from_target_text_triggered();

    void on_actionCopy_and_paste_from_base_text_triggered();

    void on_actionDelete_triggered();

    void on_actionSelect_all_from_questions_triggered();

    void on_actionSelect_all_from_report_triggered();

    void on_actionSelect_all_from_target_text_triggered();

    void on_actionSelect_all_from_base_text_triggered();

    void on_actionFind_in_questions_triggered();

    void on_actionFind_in_report_triggered();

    void on_actionFind_in_target_text_triggered();

    void on_actionFind_in_base_text_triggered();

    void on_actionFind_and_replace_triggered();

    void on_actionFind_and_replace_all_triggered();

    void on_actionZoom_in_all_windows_triggered();

    void on_actionZoom_in_questions_triggered();

    void on_actionZoom_in_report_triggered();

    void on_actionZoom_in_target_text_triggered();

    void on_actionZoom_in_base_text_triggered();

    void on_actionZoom_out_all_windows_triggered();

    void on_actionZoom_out_questions_triggered();

    void on_actionZoom_out_report_triggered();

    void on_actionZoom_out_target_text_triggered();

    void on_actionZoom_out_base_text_triggered();

    void on_actionFont_of_questions_triggered();

    void on_actionFont_of_report_triggered();

    void on_actionFont_of_target_text_triggered();

    void on_actionFont_of_base_text_triggered();

    void on_actionText_color_of_questions_triggered();

    void on_actionText_color_of_report_triggered();

    void on_actionText_color_of_target_text_triggered();

    void on_actionText_color_of_base_text_triggered();

    void on_actionBackground_color_of_questions_triggered();

    void on_actionBackground_color_of_report_triggered();

    void on_actionBackground_color_of_target_text_triggered();

    void on_actionBackground_color_of_base_text_triggered();

    void on_actionPalette_color_of_all_windows_triggered();

    void on_actionPalette_color_of_questions_triggered();

    void on_actionPalette_color_of_report_triggered();

    void on_actionPalette_color_of_target_text_triggered();

    void on_actionPalette_color_of_base_text_triggered();

    void on_actionFormatted_target_text_triggered();

    void on_actionUnformatted_target_text_triggered();

    void on_actionFormatted_base_text_triggered();

    void on_actionUnformatted_base_text_triggered();

    void on_actionFormatted_questions_triggered();

    void on_actionUnformatted_questions_triggered();

    void on_actionEncoding_of_questions_triggered();

    void on_actionEncoding_of_report_triggered();

    void on_actionEncoding_of_target_text_triggered();

    void on_actionEncoding_of_base_text_triggered();

    void on_actionInsert_book_title_triggered();

    void on_actionInsert_chapter_number_triggered();

    void on_actionInsert_chapters_numbers_triggered();

    void on_actionInsert_verse_number_triggered();

    void on_actionInsert_verses_numbers_triggered();

    void on_actionInsert_question_number_triggered();

    void on_actionInsert_time_of_work_start_triggered();

    void on_actionInsert_time_of_work_end_triggered();

    void on_actionInsert_report_title_triggered();

    void on_actionRespondent_answered_correctly_triggered();

    void on_actionRespondent_answered_correctly_but_there_are_comments_triggered();

    void on_actionRespondent_answered_partially_correctly_triggered();

    void on_actionRespondent_answered_partially_correctly_and_there_are_comments_triggered();

    void on_actionRespondent_answered_incorrectly_triggered();

    void on_actionRespondent_answered_incorrectly_and_there_are_comments_triggered();

    void on_actionRespondent_was_unable_to_answer_triggered();

    void on_actionRespondent_was_unable_to_answer_and_there_are_comments_triggered();

    void on_actionTester_did_not_ask_the_question_because_there_was_no_need_triggered();

    void on_actionUnknown_word_better_to_replace_it_triggered();

    void on_actionToo_long_and_complicated_sentence_triggered();

    void on_actionPronoun_antecedent_is_unclear_triggered();

    void on_actionBetter_to_change_the_word_order_in_the_sentence_triggered();

    void on_actionFigurative_expression_is_unclear_triggered();

    void on_actionConnection_between_sentences_is_unclear_triggered();

    void on_actionConnection_between_words_is_unclear_all_the_words_are_clear_triggered();

    void on_actionMain_thought_is_unclear_triggered();

    void on_actionThere_is_something_unappropriate_for_the_local_culture_triggered();

    void on_actionRespondent_did_not_like_something_in_the_text_triggered();

    void on_actionRespondent_liked_something_in_the_text_triggered();

    void on_actionAnother_comment_or_problem_triggered();

    void on_actionEnglish_triggered();

    void on_actionRussian_triggered();

    void on_actionInsert_time_automatically_triggered();

    void on_actionInsert_title_automatically_triggered();

    void on_actionLoad_files_automatically_triggered();

    void on_actionQuestionsUSFM_HTML_triggered();

    void on_actionTarget_textUSFM_HTML_triggered();

    void on_actionBase_textUSFM_HTML_triggered();

    void on_actionEncoding_for_USFM_HTML_triggered();

    void on_actionAbout_triggered();

    void on_actionTextBold_triggered();

    void on_actionTextItalic_triggered();

    void on_actionTextUnderline_triggered();

    void on_actionAlignLeft_triggered();

    void on_actionAlignRight_triggered();

    void on_actionAlignCenter_triggered();

    void on_actionAlignJustify_triggered();

    void on_textEdit_Questions_customContextMenuRequested();

    void on_textEdit_Report_customContextMenuRequested();

    void on_textEdit_TargetText_customContextMenuRequested();

    void on_textEdit_BaseText_customContextMenuRequested();

    void on_actionFormatted_Transcelerator_triggered();

    void on_actionAbout_Qt_triggered();

private:
    Ui::MainWindow *ui;
    QString FilenameReport;
    QString FilenameQuestions;
    QString FilenameTargetText;
    QString FilenameBaseText;
    QString TextReport;

    QString textEditName;

    QString file_report;
    QString file_questions;
    QString file_target_text;
    QString file_base_text;

    bool TextBold;
    bool TextItalic;
    bool TextUnderline;
    bool TimeAuto;
    bool TitleAuto;
    bool FilesNotLoadAuto;

    int ZoomReport;
    int ZoomQuestions;
    int ZoomTargetText;
    int ZoomBaseText;

    QString EncodingReport;
    QString EncodingQuestions;
    QString EncodingTargetText;
    QString EncodingBaseText;
    QString EncodingHTML;

    void textEdit_focused(QString textEditName);

    void set_zoom_at_start();
    void close_report();

    void open_files_at_start();

    void open_report(QString file_report);
    void open_questions(QString file_questions);
    void open_target_text(QString file_target_text);
    void open_base_text(QString file_base_text);

    void formatted(QString document_name);
    void unformatted(QString document_name);
    void convert_to_PDF(QString writing, QString document_name);
    void convert_to_ODT(QString writing, QString document_name);
    void search_word(QString document_name);
    void change_encoding(QString document_name);

    void USFM_to_HTML(QString document_name);

protected:
    QSettings *m_settings;
    QVariant report;
    QVariant questions;
    QVariant target_text;
    QVariant base_text;
    QVariant lang;
    QVariant time_auto;
    QVariant title_auto;
    QVariant files_not_load_auto;
    QVariant zoom_report;
    QVariant zoom_questions;
    QVariant zoom_target_text;
    QVariant zoom_base_text;
};

#endif // MAINWINDOW_H
