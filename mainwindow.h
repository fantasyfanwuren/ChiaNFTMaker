#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "dialogregister.h"
#include <QMainWindow>
#include <QSettings>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QProgressBar;
class FormAdd;
class QLabel;
class FormMint;
class FormOffer;
class QTranslator;
class DialogRegister;
class DialogAbout;
class DialogHelp;


class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QSettings settings = QSettings("The Bit Party","ChiaNFTMaker");
    FormAdd * formAdd;
    QLayout *layoutSet;
    FormMint *formMint;
    FormOffer *formOffer;
    QTranslator *trans;
    DialogRegister dialogRegister;
    DialogAbout *about;
    DialogHelp *help;
private:
    void iniSetForm();
    void iniStyle();
    void getSettings();
    void setConnect();
    void add();
    void mint();
    void offer();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_toolButtonAdd_clicked();

    void on_toolButtonMint_clicked();

    void on_toolButtonOffer_clicked();

    void on_chinese_triggered();

    void on_actionEN_triggered();

    void on_progress(const int & value,const int & max,const QString &message);

    void on_toolButtonStart_clicked();

    void on_logon_triggered();

    void on_save_triggered();

    void on_software_triggered();

    void on_documentHelp_triggered();

    void on_composeHelp_triggered();

    void on_mintHelp_triggered();

    void on_offerHelp_triggered();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
