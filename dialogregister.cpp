#include "dialogregister.h"
#include "ui_dialogregister.h"
#include "cryption.h"

#include <QSettings>
#include <QMessageBox>
#include <QThread>
#include <QtDebug>
#include <QFile>

DialogRegister::DialogRegister(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogRegister)
{
    ui->setupUi(this);
    setFixedSize(sizeHint());
    iniStyle();
    setWindowTitle(tr("注册信息"));
    setAttribute(Qt::WA_StyledBackground,true);
    readRemainder();
    readSettings();
}


void DialogRegister::iniStyle()
{
    QFile file(":/ElegantDark.qss");/*QSS文件所在的路径*/
    file.open(QIODevice::ReadOnly);
    if(file.isOpen())
    {
        QString strFile = file.readAll();
        this->setStyleSheet(strFile);
        file.close();/*记得关闭QSS文件*/
    }
}


DialogRegister::~DialogRegister()
{
    delete ui;
}


void DialogRegister::readRemainder()
{
    Cryption cryption;
    QString remainder;
    cryption.readRemainder(remainder);
    ui->lineEditRemainder->setText(remainder);
}


void DialogRegister::readSettings()
{
    QSettings settings("The Bit Party","ChiaNFTMaker");
    QString program = settings.value("program").toString();
    ui->lineEditChiaProgram->setText(program);
    QString fingerprint = settings.value("fingerprint").toString();
    ui->lineEditFingerprint->setText(fingerprint);
    double fee = settings.value("fee",0).toDouble();
    ui->lineEditFee->setText(QString::number(fee));
}


void DialogRegister::on_pushButtonRecharge_clicked()
{
    //注册表操作
    QSettings settings("The Bit Party","ChiaNFTMaker");
    QString program = ui->lineEditChiaProgram->text();
    QString fingerprint = ui->lineEditFingerprint->text();
    double fee = ui->lineEditFee->text().toDouble();
    double num = ui->doubleSpinBoxRecharge->value();


    if(program.isEmpty()){
        QMessageBox::information(this,tr("温馨提示"),tr("请填写chia程序位置"));
        return;
    }else{
        settings.setValue("program",program);
    }
    if(fingerprint.isEmpty()){
        QMessageBox::information(this,tr("温馨提示"),tr("请填写指纹"));
        return;
    }else{
        settings.setValue("fingerprint",fingerprint);
    }
    if(fee>=0.01){
        QMessageBox::information(this,tr("温馨提示"),tr("交易费填写过大，您肯定写错了"));
        return;
    }else{
        settings.setValue("fee",fee);
    }
    ui->pushButtonRecharge->setEnabled(false);
    //创建充值及等待打款被区块链确认的线程
    Cryption *cryption = new Cryption();
    QThread *thread = new QThread();
    cryption->moveToThread(thread);
    cryption->getRechargeArg(num,fee,program,fingerprint);
    connect(thread,SIGNAL(started()),cryption,SLOT(recharge()));
    connect(cryption,SIGNAL(finished()),thread,SLOT(quit()));
    connect(cryption,SIGNAL(finished()),this,SLOT(on_finished()));
    connect(cryption,SIGNAL(wrongMessage(QString)),this,SLOT(on_wrongMessage(QString)));
    connect(cryption,SIGNAL(rightMessage(QString)),this,SLOT(on_rightMessage(QString)));
    thread->start();

}


void DialogRegister::on_wrongMessage(const QString &message)
{
    QMessageBox::information(this,tr("温馨提示"),message);
    Cryption *cryption = (Cryption *)sender();
    delete cryption;

}
void DialogRegister::on_rightMessage(const QString &message)
{
    ui->labelMessage->setText(tr("状态提示:")+message);
    readRemainder();
}


void DialogRegister::on_finished()
{
    ui->pushButtonRecharge->setEnabled(true);
}











