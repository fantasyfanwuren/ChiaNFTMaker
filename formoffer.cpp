#include "formoffer.h"
#include "ui_formoffer.h"
#include "listwalletidthread.h"
#include "offerthread.h"
#include "dialogordervie.h"

#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include <QThread>
#include <QStandardItemModel>
#include <QtConcurrent>


FormOffer::FormOffer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormOffer)
{
    ui->setupUi(this);
    readSettings();
    walletIdModel = new QStandardItemModel(this);
    ui->tableViewWalletId->setModel(walletIdModel);
    ui->tableViewWalletId->setEditTriggers(QAbstractItemView::NoEditTriggers);
    orderTableModel = new QStandardItemModel(this);
    ui->tableViewOrderTable->setModel(orderTableModel);
    dialogOrderView = new DialogOrderVie(this);
}

FormOffer::~FormOffer()
{
    delete theOfferThread;
    delete ui;
}
void FormOffer::saveSettings()
{
    //chia path;
    QSettings settings= QSettings("The Bit Party","ChiaNFTMaker");
    QString chiaPath = ui->lineEditOfferChiaPath->text();
    settings.setValue("program",chiaPath);
    //offer保存路径
    QString offerPath = ui->lineEditOfferSaveDir->text();
    settings.setValue("offerSavePath",offerPath);
    //指纹
    QString fingerprint = ui->lineEditOfferFingerprint->text();
    settings.setValue("fingerprint",fingerprint);
    //设置NFTWalletId
    QString NFTWalletId = ui->lineEditOfferNFTWalletID->text();
    settings.setValue("NFTWalletId",NFTWalletId);
    //设置收款钱包ID
    QString receiveWalletId = ui->lineEditOfferReceiveWalletID->text();
    settings.setValue("formOfferReceiveWalletId",receiveWalletId);
    //交易费
    int fee = ui->spinBoxOfferTransactionFee->value();
    settings.setValue("formMintFee",fee);
    //设置价格
    double lowPrice = ui->doubleSpinBoxLowPrice->value();
    settings.setValue("formOfferLowPrice",lowPrice);
    double upPrice = ui->doubleSpinBoxUpPrice->value();
    settings.setValue("formOfferUpPrice",upPrice);

    //设置价格模式
    bool isRandomPrice = ui->radioButtonOfferRandomPrice->isChecked();
    settings.setValue("formOfferIsRandomPrice",isRandomPrice);
}
void FormOffer::readSettings()
{//chiaPath;offerPath;fingerprint;NFTWalletId;receiveWalletId;fee;lowPrice;upPrice;isRandomPrice
    //chia path;
    QSettings settings= QSettings("The Bit Party","ChiaNFTMaker");
    QString chiaPath = settings.value("program").toString();
    ui->lineEditOfferChiaPath->setText(chiaPath);
    //offer保存路径
    QString offerPath = settings.value("offerSavePath").toString();
    ui->lineEditOfferSaveDir->setText(offerPath);
    //指纹
    QString fingerprint = settings.value("fingerprint").toString();
    ui->lineEditOfferFingerprint->setText(fingerprint);
    //设置NFTWalletId
    QString NFTWalletId = settings.value("NFTWalletId").toString();
    ui->lineEditOfferNFTWalletID->setText(NFTWalletId);
    //设置收款钱包ID
    QString receiveWalletId = settings.value("formOfferReceiveWalletId","1").toString();//新
    ui->lineEditOfferReceiveWalletID->setText(receiveWalletId);
    //交易费
    int fee = settings.value("formMintFee").toInt();
    ui->spinBoxOfferTransactionFee->setValue(fee);
    //设置价格
    double lowPrice = settings.value("formOfferLowPrice",0.5).toDouble();//新
    ui->doubleSpinBoxLowPrice->setValue(lowPrice);
    ui->doubleSpinBoxUpPrice->setMinimum(lowPrice);
    double upPrice = settings.value("formOfferUpPrice",0.5).toDouble();//新
    ui->doubleSpinBoxUpPrice->setValue(upPrice);
    //设置价格模式
    bool isRandomPrice = settings.value("formOfferIsRandomPrice",false).toBool();//新
    if(isRandomPrice){
        ui->radioButtonOfferRandomPrice->click();
    }else{
        ui->radioButtonOfferUniformPrice->click();
    }


}

void FormOffer::on_pushButtonOfferDeamonDirChoose_clicked()
{
    QSettings settings= QSettings("The Bit Party","ChiaNFTMaker");
    QString iniPath =settings.value("program",".").toString();
    QString chiaPath = QFileDialog::getOpenFileName(this,tr("请选择chia"),iniPath);
    if(!chiaPath.isEmpty()){
        ui->lineEditOfferChiaPath->setText(chiaPath);
        settings.setValue("program",chiaPath);
    }

}

void FormOffer::on_pushButtonOfferSaveDirChoose_clicked()
{
    QSettings settings= QSettings("The Bit Party","ChiaNFTMaker");
    QString iniPath =settings.value("offerSavePath",".").toString();
    QString offerPath = QFileDialog::getExistingDirectory(this,tr("请选择chia"),iniPath);
    if(!offerPath.isEmpty()){
        ui->lineEditOfferSaveDir->setText(offerPath);
        settings.setValue("offerSavePath",offerPath);
    }

}


void FormOffer::on_pushButtonOfferWalletListView_clicked()
{
    QSettings settings = QSettings("The Bit Party","ChiaNFTMaker");
    QString chiaFilePath = ui->lineEditOfferChiaPath->text();
    if(chiaFilePath.isEmpty()){
        QMessageBox::information(this,tr("提醒"),tr("请填写chia程序位置"));
        return;
    }else{
        settings.setValue("program",chiaFilePath);
    }
    QString fingerPrint = ui->lineEditOfferFingerprint->text();
    if(fingerPrint.isEmpty()){
        QMessageBox::information(this,tr("提醒"),tr("请填写钱包指纹"));
        return;
    }else{
        settings.setValue("fingerprint",fingerPrint);
    }

    ui->pushButtonOfferWalletListView->setEnabled(false);

    ListWalletIDThread * getWallet = new ListWalletIDThread(chiaFilePath,fingerPrint);
    QThread *theThread = new QThread;
    getWallet->moveToThread(theThread);
    connect(theThread,SIGNAL(started()),getWallet,SLOT(listWallet()));
    connect(getWallet,SIGNAL(finished()),theThread,SLOT(quit()));
    connect(getWallet,SIGNAL(wrongMessage(QString)),this,SLOT(on_getWalletWrong(QString)));
    connect(getWallet,SIGNAL(walletIdover(QList<QStringList>)),this,SLOT(on_walletIdover(QList<QStringList>)));
    theThread->start();
}

void FormOffer::on_getWalletWrong(QString message)
{
    QMessageBox::information(this,tr("提醒"),message);
    ui->pushButtonOfferWalletListView->setEnabled(true);

}

void FormOffer::on_walletIdover(QList<QStringList> table)
{
    //temp<<name<<walletId<<type<<DIDId;
        walletIdModel->clear();
        walletIdModel->setHorizontalHeaderLabels(QStringList()<<tr("名称")<<tr("钱包ID")<<tr("钱包类型")<<tr("身份ID"));
        for(int i=0;i<table.count();++i){
            for(int j=0;j<table.at(i).count();++j){
                QStandardItem *tempItem = new QStandardItem(table.at(i).at(j));
                walletIdModel->setItem(i,j,tempItem);
            }
        }
        ui->tableViewWalletId->resizeColumnToContents(3);
        ui->pushButtonOfferWalletListView->setEnabled(true);

}

void FormOffer::on_radioButtonOfferUniformPrice_toggled(bool checked)
{
    //点选统一价格：
    if(checked){
        ui->labelrLowPrice->setText(tr("统一价格"));
        ui->labelUpPrice->hide();
        ui->doubleSpinBoxUpPrice->hide();
        ui->linePrice->hide();
    }else{
        ui->labelrLowPrice->setText(tr("价格下限"));
        ui->doubleSpinBoxUpPrice->show();
        ui->labelUpPrice->show();
        ui->linePrice->show();
    }


}

void FormOffer::on_doubleSpinBoxLowPrice_valueChanged(double arg1)
{
    ui->doubleSpinBoxUpPrice->setMinimum(arg1);
}


void FormOffer::on_pushButtonOfferLayout_clicked()
{
    if(!getArgs()){
        return;
    }
    theOfferThread = new OfferThread(theOfferArg);
    QtConcurrent::run(theOfferThread,&OfferThread::makeTable);
    connect(theOfferThread,SIGNAL(progress(int,int,QString)),this,SLOT(on_progress(int,int,QString)));
    connect(theOfferThread,SIGNAL(finishMakeTable()),this,SLOT(on_finishMakeTable()));
}

void FormOffer::on_finishMakeTable()
{
    orderTableModel->clear();
    orderTableModel->setHorizontalHeaderLabels(QStringList()<<tr("NFT链接")<<tr("NFTID")<<tr("价格")<<tr("提示"));

    const QList<QStringList> &table = theOfferThread->getTable();
    int rowCount = table.count();
    int colCount = table.at(0).count();
    for(int i=0;i<rowCount;++i){
        for(int j=0;j<colCount;++j){
            QStandardItem *temp = new QStandardItem(table.at(i).at(j));
            orderTableModel->setItem(i,j,temp);
        }
        if(i%50==0){
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
            emit progress(i,rowCount-1,tr("正在准备表格..."));
            ui->tableViewOrderTable->scrollTo(orderTableModel->index(i,0));
        }
        if(i==rowCount-1){
            emit progress(i,rowCount-1,tr("准备表格完成"));
            ui->tableViewOrderTable->scrollTo(orderTableModel->index(i,0));
            ui->tableViewOrderTable->resizeColumnToContents(0);
        }
    }

}

void FormOffer::on_progress(int value,int max,QString message)
{
    emit progress(value,max,message);
}

bool FormOffer::getArgs()
{
    QSettings settings= QSettings("The Bit Party","ChiaNFTMaker");
    //chiaPath;
    QString chiaPath = ui->lineEditOfferChiaPath->text();
    if(chiaPath.isEmpty()){
        QMessageBox::information(this,tr("提醒"),tr("请填写chia路径"));
        return false;
    }else{
        theOfferArg.chiaPath = chiaPath;
    //program
        settings.setValue("program",chiaPath);
    }
    //offerPath;
    QString offerPath = ui->lineEditOfferSaveDir->text();
    if(offerPath.isEmpty()){
        QMessageBox::information(this,tr("提醒"),tr("请填写保存目录"));
        return false;
    }else{
        theOfferArg.offerPath = offerPath;
    // offerSavePath
        settings.setValue("offerSavePath",offerPath);
    }
    //fingerprint;
    QString fingerprint = ui->lineEditOfferFingerprint->text();
    if(fingerprint.isEmpty()){
        QMessageBox::information(this,tr("提醒"),tr("请填写指纹"));
        return false;
    }else{
        theOfferArg.fingerprint = fingerprint;
     //fingerprint
        settings.setValue("fingerprint",fingerprint);
    }
    //NFTWalletId;
    QString NFTWalletId = ui->lineEditOfferNFTWalletID->text();
    if(NFTWalletId.isEmpty()){
        QMessageBox::information(this,tr("提醒"),tr("请填写NFT钱包ID"));
        return false;
    }else{
        theOfferArg.NFTWalletId = NFTWalletId;
    // NFTWalletId
        settings.setValue("NFTWalletId",NFTWalletId);
    }
    //receiveWalletId;
    QString receiveWalletId = ui->lineEditOfferReceiveWalletID->text();
    if(receiveWalletId.isEmpty()){
        QMessageBox::information(this,tr("提醒"),tr("请填写接收钱包ID"));
        return false;
    }else{
        theOfferArg.receiveWalletId = receiveWalletId;
    //formOfferReceiveWalletId
        settings.setValue("formOfferReceiveWalletId",receiveWalletId);
    }
    //fee;
    int fee = ui->spinBoxOfferTransactionFee->value();
    theOfferArg.fee = fee;
    settings.setValue("formMintFee",fee);
    //lowPrice;|//formOfferLowPrice
    double lowPrice = ui->doubleSpinBoxLowPrice->value();
    theOfferArg.lowPrice = lowPrice;
    settings.setValue("formOfferLowPrice",lowPrice);
    //upPrice;formOfferUpPrice
    double upPrice = ui->doubleSpinBoxUpPrice->value();
    theOfferArg.upPrice = upPrice;
    settings.setValue("formOfferUpPrice",upPrice);
    //isRandomPrice formOfferIsRandomPrice
    bool isRandomPrice= ui->radioButtonOfferRandomPrice->isChecked();
    theOfferArg.isRandomPrice = isRandomPrice;
    settings.setValue("formOfferIsRandomPrice",isRandomPrice);
    return true;
}

void FormOffer::run()
{
    QtConcurrent::run(theOfferThread,&OfferThread::run);
    connect(theOfferThread,SIGNAL(progress(int,int,QString))
            ,this,SLOT(on_progress(int,int,QString)));
    //writeTable(int,QString)
    connect(theOfferThread,SIGNAL(writeTable(int,QString)),this,SLOT(on_writeTable(int,QString)));
}

bool FormOffer::isRunable()
{
    if(orderTableModel->rowCount()==0){
        QMessageBox::information(this,tr("提醒"),tr("请先进行初始布局"));
        return false;
    }else{
        refreshOfferThreadTable();
    }
    if(!getArgs()){
        return false;
    }
    return true;
}

void FormOffer::on_writeTable(int row,QString message)
{
    QStandardItem * item = new QStandardItem(message);
    orderTableModel->setItem(row,3,item);
    ui->tableViewOrderTable->scrollTo(orderTableModel->index(row,3));
}
void FormOffer::refreshOfferThreadTable()
{
    QList<QStringList> table;
    for(int i=0;i<orderTableModel->rowCount();++i){
        QStringList tempList;
        tempList.clear();
        for(int j=0;j<orderTableModel->columnCount()-1;++j){
            QStandardItem *tempItem = orderTableModel->item(i,j);
            tempList<<tempItem->text();
        }
        table<<tempList;
        if(i%50==0){
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }
    }
    theOfferThread->setTable(table);
}


void FormOffer::on_pushButtonOfferOrderView_clicked()
{
    if(orderTableModel->rowCount()==0){
        QMessageBox::information(this,tr("提醒"),tr("请先进行初始布局"));
        return;
    }else{
        refreshOfferThreadTable();
    }
    if(!getArgs()){
        return;
    }
    QtConcurrent::run(theOfferThread,&OfferThread::viewOrder);
    connect(theOfferThread,SIGNAL(progress(int,int,QString))
            ,this,SLOT(on_progress(int,int,QString)));
    connect(theOfferThread,SIGNAL(finishViewOrder(QString)),this,SLOT(on_finishViewOrder(QString)));
}

void FormOffer::on_finishViewOrder(QString orderText)
{
    dialogOrderView->showOrders(orderText);
    dialogOrderView->show();

}

void FormOffer::on_pushButtonOfferLayoutDelete_clicked()
{
    if(orderTableModel->rowCount()!=0){
        QModelIndexList indexList = ui->tableViewOrderTable->selectionModel()->selectedIndexes();
        QList<int> deleteRow;
        //筛选出非重复的行
        for(int i=0;i<indexList.count();++i){
            if(!deleteRow.contains(indexList.at(i).row())){
                deleteRow<<indexList.at(i).row();
            }
        }
        //执行删除行的命令
        for(int i=0;i<deleteRow.count();++i){
            orderTableModel->removeRow(deleteRow.at(i)-i);
        }
    }else{
        QMessageBox::information(this,tr("提醒"),tr("未进行初始化布局"));
    }
}

