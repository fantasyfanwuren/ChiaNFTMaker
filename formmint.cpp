#include "formmint.h"
#include "ui_formmint.h"
#include "listwalletidthread.h"
#include "mintthread.h"
#include "dialogordervie.h"

#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QThread>
#include <QStandardItemModel>
#include <QInputDialog>
#include <QDesktopServices>
#include <QtDebug>
#include <QtConcurrent>


FormMint::FormMint(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormMint)
{
    ui->setupUi(this);
    makeConnect();
    readSettings();
    walletIdModel = new QStandardItemModel(this);
    royaltyModel = new QStandardItemModel(this);
    orderModel = new QStandardItemModel(this);
    //图片名称-图片哈希-图片链接-元数据名称-元数据哈希-元数据链接-版税地址-提示 赋予orderTable
    orderModel->setHorizontalHeaderLabels(QStringList()
                                          <<tr("图片名称")<<tr("图片哈希")<<tr("图片链接")
                                          <<tr("元数据名称")<<tr("元数据哈希")<<tr("元数据链接")
                                          <<tr("版税地址")<<tr("提示"));
    ui->tableViewWalletID->setModel(walletIdModel);
    ui->tableViewWalletID->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewMintRoyalty->setModel(royaltyModel);
    ui->tableViewMintOrder->setModel(orderModel);
    ui->tableViewMintOrder->setEditTriggers(QAbstractItemView::NoEditTriggers);
    orderViewDialog = new DialogOrderVie(this);

}

void FormMint::makeConnect()
{
    connect(ui->pushButtonMintOrderGenerate,SIGNAL(clicked()),this,SLOT(orderGenerate()));
}

FormMint::~FormMint()
{
    delete theMintThread;
    delete ui;

}

void FormMint::saveSettings()
{
    //chia.exe路径
    QSettings settings = QSettings("The Bit Party","ChiaNFTMaker");
    QString chiaFilePath = ui->lineEditMintChiaProgram->text();
    settings.setValue("program",chiaFilePath);
    //指纹
    QString fingerprint = ui->lineEditMintFingerprint->text();
    settings.setValue("fingerprint",fingerprint);
    //png路径
    QString pngDir = ui->lineEditMintPngDir->text();
    settings.setValue("formMintPngDir",pngDir);
    //元数据路径
    QString mataDir = ui->lineEditMintMetaDir->text();
    settings.setValue("formMintMetaDir",mataDir);
    //图片链接 settings.setValue("formMintPngLink",pngLink);
    QString pngLink = ui->lineEditMintPngLink->text();
    settings.setValue("formMintPngLink",pngLink);
    //元数据链接 settings.setValue("formMintMetaLink",metaLink);
    QString mataLink = ui->lineEditMintMetaLink->text();
    settings.setValue("formMintMetaLink",mataLink);
    //许可证链接 settings.setValue("formMintLicenceLink",licenceLink);
    QString licenceLink = ui->lineEditMintLicenceLink->text();
    settings.setValue("formMintLicenceLink",licenceLink);
    //设置NFTWalletId
    QString NFTWalletId = ui->lineEditMintNFTWalletID->text();
    settings.setValue("NFTWalletId",NFTWalletId);
    //设置目标地址 settings.setValue("formMintTargetAddress",targetAddress);
    QString targetAddress = ui->lineEditMintTargetAddress->text();
    settings.setValue("formMintTargetAddress",targetAddress);
    //版税
    int royalty = ui->spinBoxMintRoyaltyRate->value();
    settings.setValue("formMintRoyalty",royalty);
    //交易费
    int fee = ui->spinBoxMintTransactionFee->value();
    settings.setValue("formMintFee",fee);

    //版本号
    int currentVersion = ui->spinBoxMintCurrentVersion->value();
    settings.setValue("formMintCurrentVersion",currentVersion);
    //总版本号
    int totalVersion = ui->spinBoxMintTotalVersion->value();
    settings.setValue("formMintTotalVersion",totalVersion);
    //是否乱序 settings.setValue("formMintOrderly",theMintArg.isOrderly);
    bool isOrderly = ui->checkBoxNoOrderly->isChecked();
    settings.setValue("formMintOrderly",isOrderly);
}

void FormMint::readSettings()
{
    //chia.exe路径
    QSettings settings = QSettings("The Bit Party","ChiaNFTMaker");
    QString chiaFilePath = settings.value("program").toString();
    ui->lineEditMintChiaProgram->setText(chiaFilePath);
    //指纹
    QString fingerprint = settings.value("fingerprint").toString();
    ui->lineEditMintFingerprint->setText(fingerprint);
    //png路径
    QString pngDir = settings.value("formMintPngDir").toString();
    ui->lineEditMintPngDir->setText(pngDir);
    //元数据路径
    QString mataDir = settings.value("formMintMetaDir").toString();
    ui->lineEditMintMetaDir->setText(mataDir);
    //图片链接 settings.setValue("formMintPngLink",pngLink);
    QString pngLink = settings.value("formMintPngLink").toString();
    ui->lineEditMintPngLink->setText(pngLink);
    //元数据链接 settings.setValue("formMintMetaLink",metaLink);
    QString mataLink = settings.value("formMintMetaLink").toString();
    ui->lineEditMintMetaLink->setText(mataLink);
    //许可证链接 settings.setValue("formMintLicenceLink",licenceLink);
    QString licenceLink = settings.value("formMintLicenceLink").toString();
    ui->lineEditMintLicenceLink->setText(licenceLink);
    //设置NFTWalletId
    QString NFTWalletId = settings.value("NFTWalletId").toString();
    ui->lineEditMintNFTWalletID->setText(NFTWalletId);
    //设置目标地址 settings.setValue("formMintTargetAddress",targetAddress);
    QString targetAddress = settings.value("formMintTargetAddress").toString();
    ui->lineEditMintTargetAddress->setText(targetAddress);
    //版税
    int royalty = settings.value("formMintRoyalty").toInt();
    ui->spinBoxMintRoyaltyRate->setValue(royalty);
    //交易费
    int fee = settings.value("formMintFee").toInt();
    ui->spinBoxMintTransactionFee->setValue(fee);
    //版本号
    int currentVersion = settings.value("formMintCurrentVersion").toInt();
    ui->spinBoxMintCurrentVersion->setValue(currentVersion);
    //总版本号
    int totalVersion = settings.value("formMintTotalVersion").toInt();
    ui->spinBoxMintTotalVersion->setValue(totalVersion);
    //是否乱序 settings.setValue("formMintOrderly",theMintArg.isOrderly);
    bool isOrderly = settings.value("formMintOrderly").toBool();
    ui->checkBoxNoOrderly->setChecked(!isOrderly);

}

void FormMint::on_pushButtonMintChooseChia_clicked()
{//选择chia.exe的文件位置
    QSettings settings = QSettings("The Bit Party","ChiaNFTMaker");
    QString iniDir = settings.value("program",".").toString();
    QString chiaFilePath = QFileDialog::getOpenFileName(this,tr("选择chia程序"),iniDir);
    if(!chiaFilePath.isEmpty()){
        ui->lineEditMintChiaProgram->setText(chiaFilePath);
    }
}

void FormMint::listWalletId()
{

    QSettings settings = QSettings("The Bit Party","ChiaNFTMaker");
    QString chiaFilePath = ui->lineEditMintChiaProgram->text();
    if(chiaFilePath.isEmpty()){
        QMessageBox::information(this,tr("提醒"),tr("请填写chia程序位置"));
        return;
    }else{
        settings.setValue("program",chiaFilePath);
    }
    QString fingerPrint = ui->lineEditMintFingerprint->text();
    if(fingerPrint.isEmpty()){
        QMessageBox::information(this,tr("提醒"),tr("请填写钱包指纹"));
        return;
    }else{
        settings.setValue("fingerprint",fingerPrint);
    }

    ui->pushButtonMintWalletListView->setEnabled(false);
    ui->pushButtonMintDIDWalletCreate->setEnabled(false);
    ui->pushButtonMintNFTWalletCreate->setEnabled(false);

    ListWalletIDThread * getWallet = new ListWalletIDThread(chiaFilePath,fingerPrint);
    QThread *theThread = new QThread;
    getWallet->moveToThread(theThread);
    connect(theThread,SIGNAL(started()),getWallet,SLOT(listWallet()));
    connect(getWallet,SIGNAL(finished()),theThread,SLOT(quit()));
    connect(getWallet,SIGNAL(wrongMessage(QString)),this,SLOT(on_getWalletWrong(QString)));
    connect(getWallet,SIGNAL(walletIdover(QList<QStringList>)),this,SLOT(on_walletIdover(QList<QStringList>)));
    theThread->start();
}


void FormMint::on_getWalletWrong(const QString &message)
{
    QMessageBox::information(this,tr("提醒"),message);
    ui->pushButtonMintWalletListView->setEnabled(true);
    ui->pushButtonMintDIDWalletCreate->setEnabled(true);
    ui->pushButtonMintNFTWalletCreate->setEnabled(true);
}


void FormMint::on_pushButtonMintWalletListView_clicked()
{
    this->listWalletId();

}

void FormMint::on_walletIdover(const QList<QStringList> &table)
{//temp<<name<<walletId<<type<<DIDId;
    walletIdModel->clear();
    walletIdModel->setHorizontalHeaderLabels(QStringList()<<tr("名称")<<tr("钱包ID")<<tr("钱包类型")<<tr("身份ID"));
    for(int i=0;i<table.count();++i){
        for(int j=0;j<table.at(i).count();++j){
            QStandardItem *tempItem = new QStandardItem(table.at(i).at(j));
            walletIdModel->setItem(i,j,tempItem);
        }
    }
    ui->tableViewWalletID->resizeColumnToContents(3);
    ui->pushButtonMintWalletListView->setEnabled(true);
    ui->pushButtonMintDIDWalletCreate->setEnabled(true);
    ui->pushButtonMintNFTWalletCreate->setEnabled(true);

}

void FormMint::makeDIDWallet()
{
    QSettings settings = QSettings("The Bit Party","ChiaNFTMaker");
    QString chiaFilePath = ui->lineEditMintChiaProgram->text();
    if(chiaFilePath.isEmpty()){
        QMessageBox::information(this,tr("提醒"),tr("请填写chia程序位置"));
        return;
    }else{
        settings.setValue("program",chiaFilePath);
    }
    QString fingerPrint = ui->lineEditMintFingerprint->text();
    if(fingerPrint.isEmpty()){
        QMessageBox::information(this,tr("提醒"),tr("请填写钱包指纹"));
        return;
    }else{
        settings.setValue("fingerprint",fingerPrint);
    }


    QString DIDName = QInputDialog::getText(this,tr("DID名称"),tr("请给DID钱包起个名字："));
    if(DIDName.isEmpty()){
        return;
    }
    if(DIDName.contains(" ")){
        QMessageBox::information(this,tr("提醒"),tr("DID钱包名称不能含有空格"));
        return;
    }
    ui->pushButtonMintWalletListView->setEnabled(false);
    ui->pushButtonMintDIDWalletCreate->setEnabled(false);
    ui->pushButtonMintNFTWalletCreate->setEnabled(false);
    ListWalletIDThread * makeDIDWallet = new ListWalletIDThread(chiaFilePath,fingerPrint);
    makeDIDWallet->getDIDName(DIDName);
    QThread *theThread = new QThread;
    makeDIDWallet->moveToThread(theThread);
    connect(theThread,SIGNAL(started()),makeDIDWallet,SLOT(makeDID()));
    connect(makeDIDWallet,SIGNAL(finished()),theThread,SLOT(quit()));
    connect(makeDIDWallet,SIGNAL(wrongMessage(QString)),this,SLOT(on_getWalletWrong(QString)));
    connect(makeDIDWallet,SIGNAL(walletIdover(QList<QStringList>)),this,SLOT(on_walletIdover(QList<QStringList>)));
    theThread->start();


}

void FormMint::on_pushButtonMintDIDWalletCreate_clicked()
{
    this->makeDIDWallet();
}


void FormMint::makeNFTWallet()
{
    QSettings settings = QSettings("The Bit Party","ChiaNFTMaker");
    QString chiaFilePath = ui->lineEditMintChiaProgram->text();
    if(chiaFilePath.isEmpty()){
        QMessageBox::information(this,tr("提醒"),tr("请填写chia程序位置"));
        return;
    }else{
        settings.setValue("program",chiaFilePath);
    }
    QString fingerPrint = ui->lineEditMintFingerprint->text();
    if(fingerPrint.isEmpty()){
        QMessageBox::information(this,tr("提醒"),tr("请填写钱包指纹"));
        return;
    }else{
        settings.setValue("fingerprint",fingerPrint);
    }


    QString NFTId = QInputDialog::getText(this
                                          ,tr("NFTId")
                                          ,tr("请输入要创建的NFT钱包对应的DID钱包的身份ID\n"
                                              "获取方法：\n"
                                              "①点击\"查看列表\"\n"
                                              "②选中类型为\"DECENTRALIZED_ID\"的钱包对应的身份ID\n"
                                              "③复制到剪切板（ctrl+c）"));
    if(NFTId.isEmpty()){
        return;
    }
    ui->pushButtonMintWalletListView->setEnabled(false);
    ui->pushButtonMintDIDWalletCreate->setEnabled(false);
    ui->pushButtonMintNFTWalletCreate->setEnabled(false);
    ListWalletIDThread * makeNFTWallet = new ListWalletIDThread(chiaFilePath,fingerPrint);
    makeNFTWallet->getNFTDi(NFTId);
    QThread *theThread = new QThread;
    makeNFTWallet->moveToThread(theThread);
    connect(theThread,SIGNAL(started()),makeNFTWallet,SLOT(makeNFT()));
    connect(makeNFTWallet,SIGNAL(finished()),theThread,SLOT(quit()));
    connect(makeNFTWallet,SIGNAL(wrongMessage(QString)),this,SLOT(on_getWalletWrong(QString)));
    connect(makeNFTWallet,SIGNAL(walletIdover(QList<QStringList>)),this,SLOT(on_walletIdover(QList<QStringList>)));
    theThread->start();
}



void FormMint::on_pushButtonMintNFTWalletCreate_clicked()
{
    this->makeNFTWallet();
}


void FormMint::on_pushButtonMintRoyaltyAdd_clicked()
{
    QString royaltyAdress = QInputDialog::getText(this,tr("版税地址"),tr("请输入要添加的版税地址："));
    if(royaltyAdress.isEmpty()){
        return;
    }
    if(royaltyAdress.count()!=62){
        QMessageBox::information(this,tr("提醒"),tr("所填写的版税地址有误"));
        return;
    }
    royaltyModel->setHorizontalHeaderLabels(QStringList()<<tr("版税地址")<<tr("比例值"));
    int currentRow = royaltyModel->rowCount();
    QStandardItem *tempItem = new QStandardItem(royaltyAdress);
    royaltyModel->setItem(currentRow,0,tempItem);
    tempItem = new QStandardItem("1.0");
    royaltyModel->setItem(currentRow,1,tempItem);
}


void FormMint::on_pushButtonMintRoyaltyDelete_clicked()
{
    if(royaltyModel->rowCount()!=0){
        QModelIndexList indexList = ui->tableViewMintRoyalty->selectionModel()->selectedIndexes();
        QList<int> deleteRow;
        //筛选出非重复的行
        for(int i=0;i<indexList.count();++i){
            if(!deleteRow.contains(indexList.at(i).row())){
                deleteRow<<indexList.at(i).row();
            }
        }
        //执行删除行的命令
        for(int i=0;i<deleteRow.count();++i){
            royaltyModel->removeRow(deleteRow.at(i)-i);
        }
    }else{
        QMessageBox::information(this,tr("提醒"),tr("未添加任何版税地址"));
    }
}


void FormMint::on_pushButtonMintRoyaltyClear_clicked()
{
    royaltyModel->removeRows(0,royaltyModel->rowCount());
}


void FormMint::on_pushButtonViewPngLink_clicked()
{
    QString adress = ui->lineEditMintPngLink->text();
    if(adress.isEmpty()){
        adress = "https://nft.storage/";
    }
    QDesktopServices::openUrl(QUrl(adress));
}





void FormMint::on_pushButtonViewMetaLink_clicked()
{
    QString adress = ui->lineEditMintMetaLink->text();
    if(adress.isEmpty()){
        adress = "https://nft.storage/";
    }
    QDesktopServices::openUrl(QUrl(adress));
}



void FormMint::on_pushButtonViewLicenceLink_clicked()
{
    QString adress = ui->lineEditMintLicenceLink->text();
    if(adress.isEmpty()){
        adress = "https://nft.storage/";
    }
    QDesktopServices::openUrl(QUrl(adress));
}


void FormMint::on_pushButtonMintPngDirChoose_clicked()
{
    QSettings settings = QSettings("The Bit Party","ChiaNFTMaker");
    QString iniDir = settings.value("formMintPngDir",".").toString();
    QString pngDir = QFileDialog::getExistingDirectory(this,tr("选择文件夹"),iniDir);
    if(!pngDir.isEmpty()){
        ui->lineEditMintPngDir->setText(pngDir);
        settings.setValue("formMintPngDir",pngDir);
    }
}


void FormMint::on_pushButtonMintMetaDirChoose_clicked()
{
    QSettings settings = QSettings("The Bit Party","ChiaNFTMaker");
    QString iniDir = settings.value("formMintMetaDir",".").toString();
    QString metaDir = QFileDialog::getExistingDirectory(this,tr("选择文件夹"),iniDir);
    if(!metaDir.isEmpty()){
        ui->lineEditMintMetaDir->setText(metaDir);
        settings.setValue("formMintMetaDir",metaDir);
    }
}




void FormMint::orderGenerate()
{

    QString message;
    if(!getMintArgs(message)){
        QMessageBox::information(this,tr("提醒"),message);
        return;
    }
    downLoadLicnse();

}
void FormMint::downLoadLicnse()
{
    QUrl newUrl = QUrl(theMintArg.licenceLink);
    qDebug()<<newUrl;
    if(newUrl.isValid()){
        reply = networkManager.get(QNetworkRequest(newUrl));
        connect(reply,SIGNAL(finished()),this,SLOT(on_downloadFinished()));
        connect(reply,SIGNAL(downloadProgress(qint64, qint64)),this,SLOT(on_downloadProgress(qint64,qint64)));
    }else{
        qDebug()<<"链接无效";
    }
}


void FormMint::on_downloadFinished()
{
    emit progress(100,100,tr("许可证下载完成"));
    //计算许可证的哈希
    QByteArray array = reply->readAll();
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(array);  // 添加数据到加密哈希值
    QByteArray result = hash.result();  // 返回最终的哈希值
    theMintArg.licenseHash = result.toHex();
    theMintThread = new MintThread(theMintArg);
    QtConcurrent::run(theMintThread,&MintThread::makeOrder);
    connect(theMintThread,SIGNAL(writeTable(QList<QStringList>)),this,SLOT(on_writeTable(QList<QStringList>)));
    connect(theMintThread,SIGNAL(progress(int,int,QString)),this,SLOT(on_progress(int,int,QString)));
}

void FormMint::on_downloadProgress(qint64 value,qint64 total)
{
    emit progress(value,total,tr("正在下载许可证..."));
}

void FormMint::on_writeTable(QList<QStringList> tabel)
{//设置表格，注意中间有点延迟
    //切断线程
    //图片名称-图片哈希-图片链接-元数据名称-元数据哈希-元数据链接-版税地址 赋予orderTable
    for(int i=0;i<tabel.count();++i){//行
        for(int j=0;j<tabel.at(i).count();++j){//列
            QStandardItem *theItem = new QStandardItem(tabel.at(i).at(j));
            orderModel->setItem(i,j,theItem);
        }
        if(i%100==0){
            emit progress(i,tabel.count()-1,tr("正在准备表格..."));
            //delay(DELAY_TIME);
            //ui->tableView->scrollTo(model->index(i,0));
            ui->tableViewMintOrder->scrollTo(orderModel->index(i,0));
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }
        if(i==tabel.count()-1){
            emit progress(i,tabel.count()-1,tr("表格准备完成"));
        }
    }

}

void FormMint::on_progress(int value,int max,QString message)
{//设置进度条及信息
    emit progress(value,max,message);
}


bool FormMint::getMintArgs(QString &message)
{
    QSettings settings = QSettings("The Bit Party","ChiaNFTMaker");
    //获取图片文件位置
    QString pngDir = ui->lineEditMintPngDir->text();
    if(pngDir.isEmpty()){
        message = tr("请选择图片文件");
        return false;
    }else{
        theMintArg.pngDir = pngDir;
        settings.setValue("formMintPngDir",pngDir);
    }
    //获取图片链接
    QString pngLink = ui->lineEditMintPngLink->text();
    if(pngLink.isEmpty()){
        message = tr("请填写图片链接");
        return false;
    }else{
        theMintArg.pngLink = pngLink;
        settings.setValue("formMintPngLink",pngLink);
    }
    //获取元文件位置
    QString metaDir = ui->lineEditMintMetaDir->text();
    if(metaDir.isEmpty()){
        message = tr("请填写元文件");
        return false;
    }else{
        theMintArg.metaDir = metaDir;
        settings.setValue("formMintMetaDir",metaDir);
    }
    //获取元文件链接
    QString metaLink = ui->lineEditMintMetaLink->text();
    if(metaLink.isEmpty()){
        message = tr("请填写元文件链接");
        return false;
    }else{
        theMintArg.metaLink = metaLink;
        settings.setValue("formMintMetaLink",metaLink);
    }
    //获取许可证链接
    QString licenceLink = ui->lineEditMintLicenceLink->text();
    if(licenceLink.isEmpty()){
        message = tr("请填写许可证链接");
        return false;
    }else{
        theMintArg.licenceLink = licenceLink;
        settings.setValue("formMintLicenceLink",licenceLink);
    }
    //获取chia目录
    QString chiaPath = ui->lineEditMintChiaProgram->text();
    if(chiaPath.isEmpty()){
        message = tr("请填写chia路径");
        return false;
    }else{
        theMintArg.chiaPath = chiaPath;
        settings.setValue("program",chiaPath);
    }
    //获取指纹
    QString fingerPrint = ui->lineEditMintFingerprint->text();
    if(fingerPrint.isEmpty()){
        message = tr("请填写指纹");
        return false;
    }else{
        //settings.value("fingerprint").toString();
        theMintArg.fingerPrint = fingerPrint;
        settings.setValue("fingerprint",fingerPrint);
    }
    //NFT钱包ID
    QString NFTWalletId = ui->lineEditMintNFTWalletID->text();
    if(NFTWalletId.isEmpty()){
        message = tr("请填写NFT钱包ID");
        return false;
    }else{
        theMintArg.NFTWalletId = NFTWalletId;
        settings.setValue("NFTWalletId",NFTWalletId);
    }
    //版税地址集合
    if(royaltyModel->rowCount()==0){
        message = tr("请添加版税地址");
        return false;
    }else{
        for(int row=0;row<royaltyModel->rowCount();++row){
            QStringList tempList;
            tempList.clear();
            for(int column=0;column<royaltyModel->columnCount();++column){
                QStandardItem *tempItem = royaltyModel->item(row,column);
                tempList<<tempItem->text();
            }
            theMintArg.royaltyAddress<<tempList;
        }
    }

    //目标地址
    QString targetAddress = ui->lineEditMintTargetAddress->text();
    if(targetAddress.isEmpty()){
        message = tr("请填写目标地址");
        return false;
    }else{
        theMintArg.targetAddress = targetAddress;
        settings.setValue("formMintTargetAddress",targetAddress);
    }
    //版税
    int royalty = ui->spinBoxMintRoyaltyRate->value();
    settings.setValue("formMintRoyalty",royalty);
    theMintArg.royalty = royalty;
    //交易费
    int fee = ui->spinBoxMintTransactionFee->value();
    settings.setValue("formMintFee",fee);
    theMintArg.fee = fee;
    //版本号
    int currentVersion = ui->spinBoxMintCurrentVersion->value();
    settings.setValue("formMintCurrentVersion",currentVersion);
    theMintArg.currentVersion = currentVersion;
    //总版本号
    int totalVersion = ui->spinBoxMintTotalVersion->value();
    settings.setValue("formMintTotalVersion",totalVersion);
    theMintArg.totalVersion = totalVersion;

    //是否乱序
    theMintArg.isOrderly=!ui->checkBoxNoOrderly->isChecked();
    settings.setValue("formMintOrderly",theMintArg.isOrderly);
    return true;
}

void FormMint::on_pushButtonMintOrderView_clicked()
{//查看指令
    QString message;
    if(!this->getMintArgs(message)){
        QMessageBox::information(this,tr("提醒"),message);
        return;
    }
    if(orderModel->rowCount()==0){
        QMessageBox::information(this,tr("提醒"),tr("请先点击生成指令"));
        return;
    }

    theMintThread->refreshMintArg(theMintArg);
    QtConcurrent::run(theMintThread,&MintThread::viewOrder);
    connect(theMintThread,SIGNAL(finishViewOrders(QString)),this,SLOT(on_finishViewOrders(QString)));
    connect(theMintThread,SIGNAL(progress(int,int,QString)),this,SLOT(on_progress(int,int,QString)));
}

void FormMint::on_finishViewOrders(QString orders)
{

    orderViewDialog->showOrders(orders);
    orderViewDialog->show();
}

void FormMint::runOrder()
{
    ui->pushButtonMintOrderGenerate->setEnabled(false);
    ui->pushButtonMintOrderView->setEnabled(false);
    QtConcurrent::run(theMintThread,&MintThread::runOrder);
    connect(theMintThread,SIGNAL(progress(int,int,QString)),this,SLOT(on_progress(int,int,QString)));
    connect(theMintThread,SIGNAL(writeTable(int,QString)),this,SLOT(on_writeTable(int,QString)));
}

void FormMint::on_writeTable(int row,QString text)
{
    QStandardItem *theItem = new QStandardItem(text);
    orderModel->setItem(row,7,theItem);
    ui->tableViewMintOrder->scrollTo(orderModel->index(row,7));
}

bool FormMint::isEnableRun()
{
    if(orderModel->rowCount()==0){
        QMessageBox::information(this,tr("提醒"),tr("请先点击生成命令"));
        return false;
    }else{
        return true;
    }
}














