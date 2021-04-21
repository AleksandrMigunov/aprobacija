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
#include <QApplication>
#include <QtCore>
#include <QtGui>
#include <QCoreApplication>
#include <QVariant>
#include <QTranslator>
#include <QSettings>
#include <QVariant>
#include <QLocale>
#include <QMainWindow>
#include "converter.h"
#include "dialogfind.h"
#include "dialogreplace.h"
#include "dialogreplaceall.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator translator;

    QSettings *m_settings = new QSettings("aprobacija_settings.ini", QSettings::IniFormat);
    m_settings->beginGroup("MainWindow");
    QVariant lang;
    QString language = m_settings->value("language", lang).toString();
    m_settings->endGroup();

    if (language == "Russian") {
        translator.load("main_ru.qm");
    }

    else if (language == "") {
        QString str = QLocale::system().name();
        if ((str == "ru") || (str == "ru_RU")) {
            translator.load("main_ru.qm");
        }
    }

    a.installTranslator(&translator);

    MainWindow w;
    w.show();

    return a.exec();
}
