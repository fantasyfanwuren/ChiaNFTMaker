#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "formadd.h"
#include "formmint.h"
#include "formoffer.h"
#include "composethread.h"
#include "ChiaNFTMaker.h"
#include "cryption.h"
#include "dialogabout.h"
#include "dialoghelp.h"



#include <QFile>
#include <QProgressBar>
#include <QLabel>
#include <QMessageBox>
#include <QInputDialog>
#include <QThread>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QtDebug>


const QString VERSION= "3.1.1";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    showMaximized();
    setWindowTitle("ChiaNFTMaker_"+VERSION);

    ui->chinese->setChecked(true);
    iniSetForm();
    iniStyle();
    getSettings();
    setConnect();
    about = new DialogAbout(this);
    help = new DialogHelp(this);

}


void MainWindow::on_progress(const int & value,const int & max,const QString &message)
{
    ui->progressBar->setMaximum(max);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(value);
    ui->lineEditMessage->setText(message);

}


void MainWindow::setConnect()
{
    connect(formAdd,SIGNAL(progress(int,int,QString)),this,SLOT(on_progress(int,int,QString)));
    connect(formMint,SIGNAL(progress(int,int,QString)),this,SLOT(on_progress(int,int,QString)));
    connect(formOffer,SIGNAL(progress(int,int,QString)),this,SLOT(on_progress(int,int,QString)));
}

void MainWindow::iniSetForm()
/*设置初始layoutSet*/
{

    formAdd = new FormAdd(this);
    formMint = new FormMint(this);
    formOffer = new FormOffer(this);
    formMint->hide();
    formOffer->hide();
    layoutSet = new QHBoxLayout();
    layoutSet->setSpacing(1);
    layoutSet->setContentsMargins(1,1,1,1);
    layoutSet->addWidget(formAdd);
    layoutSet->addWidget(formMint);
    layoutSet->addWidget(formOffer);
    ui->frameSet->setLayout(layoutSet);

}

void MainWindow::getSettings()
{
    //语言选择
    if(settings.value("language")=="en"){
        ui->actionEN->setChecked(true);
        ui->chinese->setChecked(false);
    }else{
        ui->actionEN->setChecked(false);
        ui->chinese->setChecked(true);
    }

}


void MainWindow::iniStyle()
/*设置初始样式*/
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


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_toolButtonAdd_clicked()
/*点击了 合成 按钮*/
{
    ui->toolButtonAdd->setIcon(QIcon(":/add_on.png"));
    ui->toolButtonMint->setIcon(QIcon(":/mint_off.png"));
    ui->toolButtonOffer->setIcon(QIcon(":/offer_off.png"));
    formAdd->show();
    formMint->hide();
    formOffer->hide();
    ui->toolButtonStart->setText(tr("开始-A"));

}


void MainWindow::on_toolButtonMint_clicked()
/*点击了 铸造 按钮*/
{
    ui->toolButtonAdd->setIcon(QIcon(":/add_off.png"));
    ui->toolButtonMint->setIcon(QIcon(":/mint_on.png"));
    ui->toolButtonOffer->setIcon(QIcon(":/offer_off.png"));
    formMint->show();
    formAdd->hide();
    formOffer->hide();
    ui->toolButtonStart->setText(tr("开始-M"));

}


void MainWindow::on_toolButtonOffer_clicked()
/*点击了 offer 按钮*/
{
    ui->toolButtonAdd->setIcon(QIcon(":/add_off.png"));
    ui->toolButtonMint->setIcon(QIcon(":/mint_off.png"));
    ui->toolButtonOffer->setIcon(QIcon(":/offer_on.png"));
    formOffer->show();
    formMint->hide();
    formAdd->hide();
    ui->toolButtonStart->setText(tr("开始-O"));

}


void MainWindow::on_chinese_triggered()
{
    if(ui->chinese->isChecked()){
        ui->actionEN->setChecked(false);
        settings.setValue("language","cn");
        QMessageBox::information(this,tr("提示"),tr("请将软件重启后，方可生效。"));
        qApp->exit();
    }else{
        ui->chinese->setChecked(true);
    }
}




void MainWindow::on_actionEN_triggered()
{
    if(ui->actionEN->isChecked()){
        ui->chinese->setChecked(false);
        settings.setValue("language","en");
        QMessageBox::information(this,tr("Information")
                                 ,tr("Please restart the software before it takes effect."));
        qApp->exit();
    }else{
        ui->actionEN->setChecked(true);
    }


}


void MainWindow::add()
{

    QString message;
    QString describe;
    QString discord;
    QString website;
    QString twitter;
    QString uuid;
    QString name;
    //检测元数据信息
    if(!formAdd->checkSettings(message)){
        QMessageBox::information(this,tr("提醒")
                                 ,tr("合格的作品需要具备一些架构，请在设置页面认真填写如下信息：")
                                 +message+tr("若没有，请填写:none."));
        return;
    }else{//读取元信息
        //formAddDescribe
        describe = settings.value("formAddDescribe").toString();
        //formAddDiscord
        discord = settings.value("formAddDiscord").toString();
        //formAddWebsite
        website = settings.value("formAddWebsite").toString();
        //formAddTwitter
        twitter = settings.value("formAddTwitter").toString();
        //formAddUUID
        uuid = settings.value("formAddUUID").toString();
        //formAddName
        name = settings.value("formAddName").toString();
    }

    //检查表格信息
    QList<QStringList> modelContent;
    QStringList head;
    if(!formAdd->checkModel(modelContent,head,message)){
        QMessageBox::information(this,tr("提醒"),tr("您的设置需要完善：")+message);
        return;
    }

    //检查名称长度
    //获取名称长度和起始编号，并判断是否设置是否合规
    int nameLen,nameBegin;
    formAdd->getNameRules(nameLen,nameBegin);
    if(QString::number(modelContent.count()).length()>nameLen){
        QMessageBox::information(this,tr("提醒"),tr("您的设置需要完善：")+tr("名称长度设置过小"));
        return;
    }

    //获取运行的线程数
    bool ok;
    int threadNum = QInputDialog::getInt(this,tr("设置线程"),tr("请设置合成过程使用的线程数:"),1,1,10,1,&ok);
    if(!ok){
        qDebug()<<"取消";
        return;
    }

    //消耗xch
    Cryption cryption;
    if(!cryption.use()){
        QMessageBox::information(this,tr("提醒"),tr("余额不足，请充值（帮助-注册信息-充值）"));
        return;
    }
    dialogRegister.readRemainder();


    //获取线程控制数集
    int total = modelContent.count();
    QList<ThreadNumRule> threadNumRules;
    int startNum = nameBegin;
    for(int i=threadNum;i>0;--i){
        //指定单个线程的 规则数集
        int threadLen = total/i;
        ThreadNumRule tempRule;
        tempRule.nameLen = nameLen;
        tempRule.startNum = startNum;
        tempRule.finishNum = startNum + threadLen;
        tempRule.adjustNum = nameBegin;
        threadNumRules<<tempRule;
        //为下一个循环准备
        startNum += threadLen;
        total -= threadLen;
    }
    //在线程开始运行前，先打开文件夹，供用户查看合成过程
    QString josnSavePath = QDir::currentPath()+"/josnSave";
    QDir josnDir(josnSavePath);
    if(!josnDir.exists()){
        josnDir.mkdir(josnSavePath);
    }else{
        josnDir.removeRecursively();
        josnDir.mkdir(josnSavePath);
    }
    QString pngSavePath = QDir::currentPath()+"/pngSave";
    QDir pngDir(pngSavePath);
    if(!pngDir.exists()){
        pngDir.mkdir(pngSavePath);
    }else{
        pngDir.removeRecursively();
        pngDir.mkdir(pngSavePath);
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(josnSavePath));
    QDesktopServices::openUrl(QUrl::fromLocalFile(pngSavePath));
    //开启多个线程
    QList<QThread *> thosethread;
    for(int i=0;i<threadNumRules.count();++i){
        QThread * thread = new QThread();
        thosethread<<thread;
        ComposeThread *compose = new ComposeThread(name,describe,discord,website,twitter,uuid,modelContent,head);
        compose->moveToThread(thread);
        compose->setNameRules(threadNumRules.at(i));
        connect(compose,SIGNAL(exit()),thread,SLOT(quit()));
        connect(thread,SIGNAL(started()),compose,SLOT(run()));
        thread->start();
    }
    int waitNum = 1;
    while(waitNum != 0){
        waitNum = 0;
        for (int var = 0; var < thosethread.count(); ++var) {
            if(!thosethread.at(var)->isFinished()){
                waitNum++;
            }

        }
        QCoreApplication::processEvents(QEventLoop::AllEvents, 500);
    }
    qDebug()<<"waitNum"<<waitNum;


}

void MainWindow::mint()
{
    if(!formMint->isEnableRun()){
        return;
    }
    if(QMessageBox::information(this,tr("提醒")
                             ,tr("因为铸造及其耗时，请务必确认已关闭电脑休眠状态\n\n"
                                 "您确定要继续？"),
                             QMessageBox::Ok|QMessageBox::Cancel)
            ==QMessageBox::Cancel){
        return;
    }
    //消耗xch
    Cryption cryption;
    if(!cryption.use()){
        QMessageBox::information(this,tr("提醒"),tr("余额不足，请充值（帮助-注册信息-充值）"));
        return;
    }
    dialogRegister.readRemainder();
    formMint->runOrder();
}
void MainWindow::offer()
{
    if(!formOffer->isRunable()){
        return;
    }
    //消耗xch
    Cryption cryption;
    if(!cryption.use()){
        QMessageBox::information(this,tr("提醒"),tr("余额不足，请充值（帮助-注册信息-充值）"));
        return;
    }
    dialogRegister.readRemainder();
    formOffer->run();
}
void MainWindow::on_toolButtonStart_clicked()
{
    //首先判断当前页面是哪个
    if(!formAdd->isHidden()){//当前处在合成界面
        add();
    }else if(!formMint->isHidden()){//当前处在 Mint 页面
        mint();
    }else{//当前处在 offer 页面
        offer();
    }
}


void MainWindow::on_logon_triggered()
{
    dialogRegister.show();
}


void MainWindow::on_save_triggered()
{
    formAdd->saveSettings();
    formMint->saveSettings();
    formOffer->saveSettings();
}


void MainWindow::on_software_triggered()
{
    about->show();
}


void MainWindow::on_documentHelp_triggered()
{
    help->show();
}


void MainWindow::on_composeHelp_triggered()
{
    QDesktopServices::openUrl(QUrl(QString("https://www.bilibili.com/video/BV1Nd4y1z7Pe/")));
}


void MainWindow::on_mintHelp_triggered()
{
    QDesktopServices::openUrl(QUrl(QString("https://www.bilibili.com/video/BV1PD4y117Z6/")));
}



void MainWindow::on_offerHelp_triggered()
{
    QDesktopServices::openUrl(QUrl(QString("https://www.bilibili.com/video/BV1c14y1e7x5/")));
}

