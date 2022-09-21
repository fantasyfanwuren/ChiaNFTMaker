#include "listwalletidthread.h"

#include <QProcess>
#include <QtDebug>
#include <QMetaType>

ListWalletIDThread::ListWalletIDThread(const QString &_chiaFilePath,const QString &_fingerprint,QObject *parent)
    : QObject{parent},chiaFilePath{_chiaFilePath},fingerprint{_fingerprint}
{
    //注册参数类型
    qRegisterMetaType<QList<QStringList> > ("QList<QStringList>");
}

void ListWalletIDThread::listWallet()
{//chia wallet show
    QProcess process;
    QStringList arg;
    arg<<"wallet"<<"show"<<"-f"<<fingerprint;
    process.start(chiaFilePath,arg);
    process.waitForStarted(300000);
    process.waitForFinished(300000);
    QString outPut = QString::fromLocal8Bit(process.readAllStandardOutput());
    if(!outPut.contains("Chia Wallet")){
        emit wrongMessage(tr("获取钱包列表失败"));
        emit finished();
        deleteLater();
        return;
    }
    //字符串提取 名称 类型 DID_ID 钱包ID
    // -Type: STANDARD_WALLET  DECENTRALIZED_ID NFT -DID ID:
    //核心，找到 -Wallet ID 作为查找的结束位和开始位
    int startNum = 0;
    for (int var = 0; var < 4; ++var) {
        startNum = outPut.indexOf("\n",startNum+1);
        qDebug()<<"startNum"<<startNum;
    }
    startNum+=1;
    qDebug()<<outPut.mid(startNum,5);

    while (true) {
        int walletIdNum = outPut.indexOf("-Wallet ID",startNum);
        if(walletIdNum==-1){
            break;
        }
        int walletIdFeedNum = outPut.indexOf("\n",walletIdNum);
        QString walletId = outPut.mid(walletIdNum+24,walletIdFeedNum-walletIdNum-24);
        walletId.remove("\r");
        qDebug()<<"walletId"<<walletId;

        int DIDIDNum = outPut.indexOf("-DID ID:",startNum);
        QString DIDId;
        if(DIDIDNum<walletIdNum){
            int DIDIdFeedNum = outPut.indexOf("\n",DIDIDNum);
            DIDId = outPut.mid(DIDIDNum+24,DIDIdFeedNum-DIDIDNum-24);
        }else{
            DIDId = "none";
        }
        DIDId.remove("\r");
        qDebug()<<"DIDId"<<DIDId;

        int typeNum = outPut.indexOf("-Type:",startNum);
        int typeFeedNum = outPut.indexOf("\n",typeNum);
        QString type = outPut.mid(typeNum+24,typeFeedNum-typeNum-24);
        type.remove("\r");
        qDebug()<<"type"<<type;

        int nameFeedNum = outPut.indexOf("\n",startNum);
        QString name = outPut.mid(startNum,nameFeedNum-startNum);
        name.remove(":");
        name.remove("\r");
        qDebug()<<"name"<<name;

        startNum = outPut.indexOf("\n",walletIdFeedNum+1)+1;
        qDebug()<<outPut.mid(startNum,5);

        QStringList temp;
        temp<<name<<walletId<<type<<DIDId;
        table<<temp;
    }
    qDebug()<<table;
    emit walletIdover(table);
    emit finished();
    deleteLater();
}

void ListWalletIDThread::getDIDName(const QString &_DIDName)
{
    DIDName = _DIDName;
}


void ListWalletIDThread::makeDID()
{//chia wallet did create -f -n -a
    QProcess process;
    QStringList arg;
    arg<<"wallet"<<"did"<<"create"<<"-f"<<fingerprint<<"-n"<<DIDName<<"-a"<<"9";
    process.start(chiaFilePath,arg);
    process.waitForStarted(300000);
    process.waitForFinished(300000);
    QString outPut = QString::fromLocal8Bit(process.readAllStandardOutput());
    if(!outPut.contains("Successfully created a DID")){
        emit wrongMessage(tr("创建DID钱包失败"));
        emit finished();
        deleteLater();
        return;
    }else{
        this->listWallet();
    }
}

void ListWalletIDThread::getNFTDi(const QString &_NFTDi)
{
    NFTDi = _NFTDi;
}


void ListWalletIDThread::makeNFT()
{//chia wallet nft create -f -di
    QProcess process;
    QStringList arg;
    arg<<"wallet"<<"nft"<<"create"<<"-f"<<fingerprint<<"-di"<<NFTDi;
    process.start(chiaFilePath,arg);
    process.waitForStarted(300000);
    process.waitForFinished(300000);
    QString outPut = QString::fromLocal8Bit(process.readAllStandardOutput());
    if(!outPut.contains("Successfully created an NFT")){
        emit wrongMessage(tr("创建NFT钱包失败"));
        emit finished();
        deleteLater();
        return;
    }else{
        this->listWallet();
    }

}

