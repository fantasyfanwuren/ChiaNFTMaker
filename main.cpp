#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QSettings>
#include <QTranslator>
#include <QLocale>

QTranslator * trans = nullptr;


QString readSetting();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    trans = new QTranslator;
    bool loadSuccess;
    if(readSetting()=="cn"){
        loadSuccess = trans->load(":/cn.qm");

    }else{
        loadSuccess = trans->load(":/en.qm");

    }
    if(loadSuccess){
        a.installTranslator(trans);
    }
    MainWindow w;
    w.show();
    return a.exec();
}

QString readSetting()
{
    QSettings settings = QSettings("The Bit Party","ChiaNFTMaker");
    QString language = settings.value("language").toString();
    if(language.isEmpty()){
        QLocale local;
        if(local.language()==QLocale::Chinese){
            language = "cn";
        }else{
            language = "en";
        }
        settings.setValue("language",language);
    }
    return language;

}


