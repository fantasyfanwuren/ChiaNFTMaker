#include "mintthread.h"
#include <QMetaType>
#include <QDir>
#include <QtDebug>
#include <QCryptographicHash>
#include <random>
#include <QProcess>
#include <QTimer>


MintThread::MintThread(const MintArg &thearg,QObject *parent)
    : QObject{parent},theMintArg{thearg}
{
    qRegisterMetaType<QList<QStringList> > ("QList<QStringList> ");

}


void MintThread::refreshMintArg(const MintArg & thearg)
{
    theMintArg = thearg;
}

void MintThread::makeOrder()
{
    QDir pngDir(theMintArg.pngDir);
    QStringList pngNames = pngDir.entryList(QDir::Files);
    pngNames.removeOne(".");
    pngNames.removeOne("..");
    QDir metaDir(theMintArg.metaDir);
    QStringList metaNames = metaDir.entryList(QDir::Files);
    metaNames.removeOne(",");
    metaNames.removeOne("..");
    //获取mint数量
    int maxNum = metaNames.count()<pngNames.count() ? metaNames.count() : pngNames.count();
    //获取QStringList类型的 royaltyAddress
    QStringList royaltyAddress;
    royaltyAddress.clear();
    this->getRandomroyaltyAdress(royaltyAddress,maxNum);
    //图片名称-图片哈希-图片链接-元数据名称-元数据哈希-元数据链接-版税地址-提示 赋予orderTable
    for(int i=0;i<maxNum;++i){
        QStringList tempList;
        tempList.clear();
        QString pngHash = getSha256(theMintArg.pngDir+"/"+pngNames.at(i));
        QString pngLink = theMintArg.pngLink+"/"+pngNames.at(i);
        QString metaHash = getSha256(theMintArg.metaDir+"/"+metaNames.at(i));
        QString metaLink = theMintArg.metaLink+"/"+metaNames.at(i);
        tempList<<pngNames.at(i)<<pngHash<<pngLink
               <<metaNames.at(i)<<metaHash<<metaLink
              <<royaltyAddress.at(i);
        orderTable<<tempList;
        if(i%50==0){
            emit progress(i,maxNum-1,tr("正在计算哈希和分配版税地址..."));
        }
        if(i==maxNum-1){
            emit progress(i,maxNum-1,tr("哈希计算及版税地址分配完成"));
        }
    }
    qDebug()<<theMintArg.licenseHash;
    emit writeTable(orderTable);

}

void MintThread::getRandomroyaltyAdress(QStringList &result,int maxNum) const
{
    QList<double> mix;//比例
    QList<double> ratio;//比值
    //提取比值序列,计算分母total
    double total = 0;
    for(int i=0;i<theMintArg.royaltyAddress.count();++i){
        ratio<<theMintArg.royaltyAddress.at(i).at(1).toDouble();
        total+=ratio.at(i);
    }
    //计算mix序列（比例）
    for(int i=0;i<ratio.count();++i){
        mix<<ratio.at(i)/total;
    }
    //计算分配数量
    QList<int> distribution;
    for(int i=0;i<mix.count();++i){
        distribution<<(int)(maxNum*mix.at(i));
    }
    //核对分配数量
    int sum = 0;
    for(int i=0;i<distribution.count();++i){
        sum+=distribution.at(i);
    }
    int correctionValue = sum-maxNum;//纠偏值，若为正数，表示地址多了，需要减少correctionValue个
    int lastNumber = distribution.at(distribution.count()-1);
    distribution.replace(distribution.count()-1,lastNumber-correctionValue);
    //开始进行分配
    for(int i=0;i<distribution.count();++i){
        for(int j=0;j<distribution.at(i);++j){
            result<<theMintArg.royaltyAddress.at(i).at(0);
        }
    }
    //打乱顺序
    if(!theMintArg.isOrderly){
        std::random_shuffle(result.begin(),result.end());
    }
}

const QString MintThread::getSha256(const QString &path) const
{
    /*
     * 一个计算哈希的小函数
     */
    QFile file(path);
    file.open(QIODevice::ReadOnly);
    QByteArray byteArray = file.readAll();
    file.close();
    //计算
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(byteArray);  // 添加数据到加密哈希值
    QByteArray result = hash.result();  // 返回最终的哈希值
    QString strSha256 = result.toHex();
    return strSha256;
}


void MintThread::viewOrder()
{
    double fee = (double)(theMintArg.fee/1000000000000);
    QString theOredrs;
    QStringList orderList;
    int rowNum = orderTable.count();
    for(int i=0;i<rowNum;++i){
        orderList.clear();
        orderList<<"chia"<<"wallet"<<"nft"<<"mint"
                 <<"-f"<<theMintArg.fingerPrint
                 <<"-i"<<theMintArg.NFTWalletId
                 <<"-ra"<<orderTable.at(i).at(6)
                 <<"-ta"<<theMintArg.targetAddress
                 <<"-nh"<<orderTable.at(i).at(1)
                 <<"-u"<<orderTable.at(i).at(2)
                 <<"-mh"<<orderTable.at(i).at(4)
                 <<"-mu"<<orderTable.at(i).at(5)
                 <<"-lh"<<theMintArg.licenseHash
                 <<"-lu"<<theMintArg.licenceLink
                 <<"-et"<<QString::number(theMintArg.totalVersion)
                 <<"-en"<<QString::number(theMintArg.currentVersion)
                 <<"-m"<<QString::number(fee)
                 <<"-rp"<<QString::number(theMintArg.royalty);
        for(int j=0;j<orderList.count();++j){
            theOredrs+=(orderList.at(j)+" ");
        }
        theOredrs+="\n\n";
        if(i%50==0){
            emit progress(i,rowNum-1,tr("正在生成命令..."));
        }
        if(i==rowNum-1){
            emit progress(i,rowNum-1,tr("命令已后台生成"));
        }
    }
    emit finishViewOrders(theOredrs);
}



const QList<QStringList> &MintThread::getOrderTable()
{
    return orderTable;
}


void MintThread::runOrder()
{//在执行此函数之前需要运行 refreshMintArg函数
    double fee = (double)(theMintArg.fee/1000000000000);
    QStringList arg;
    QProcess process;
    QString outPut;
    int rowNum = orderTable.count();
    for(int i=0;i<rowNum;++i){
        //首先检测钱包是否处于同步状态，若未同步，则一直等待
        arg.clear();
        arg<<"wallet"<<"show"<<"-f"<<theMintArg.fingerPrint;
        do{
            process.start(theMintArg.chiaPath,arg);
            process.waitForStarted(300000);
            process.waitForFinished(300000);
            outPut = QString::fromLocal8Bit(process.readAllStandardOutput());
            if(outPut.contains("Synced")){
                break;
            }else{
                emit progress(i,rowNum-1,tr("等待钱包同步..."));
                emit writeTable(i,tr("等待钱包同步"));
            }
            sleep(10000);
        }while(true);
        //首先检测是否之前铸造过，根据图片哈希进行比较
        //chia wallet nft list -f -i
        arg.clear();
        arg<<"wallet"<<"nft"<<"list"
            <<"-f"<<theMintArg.fingerPrint
           <<"-i"<<theMintArg.NFTWalletId;
        process.start(theMintArg.chiaPath,arg);
        process.waitForStarted(300000);
        process.waitForFinished(300000);
        outPut = QString::fromLocal8Bit(process.readAllStandardOutput());
        if(outPut.contains(orderTable.at(i).at(1))){
            emit writeTable(i,tr("之前铸造过"));
            outPut.clear();
            continue;
        }
        //进行铸造
        arg.clear();
        arg<<"wallet"<<"nft"<<"mint"
                 <<"-f"<<theMintArg.fingerPrint
                 <<"-i"<<theMintArg.NFTWalletId
                 <<"-ra"<<orderTable.at(i).at(6)
                 <<"-ta"<<theMintArg.targetAddress
                 <<"-nh"<<orderTable.at(i).at(1)
                 <<"-u"<<orderTable.at(i).at(2)
                 <<"-mh"<<orderTable.at(i).at(4)
                 <<"-mu"<<orderTable.at(i).at(5)
                 <<"-lh"<<theMintArg.licenseHash
                 <<"-lu"<<theMintArg.licenceLink
                 <<"-et"<<QString::number(theMintArg.totalVersion)
                 <<"-en"<<QString::number(theMintArg.currentVersion)
                 <<"-m"<<QString::number(fee)
                 <<"-rp"<<QString::number(theMintArg.royalty);
        process.start(theMintArg.chiaPath,arg);
        process.waitForStarted(300000);
        process.waitForFinished(300000);
        outPut = QString::fromLocal8Bit(process.readAllStandardOutput());
        qDebug()<<outPut;
        if(outPut.contains("Successfully")){
            outPut.clear();
            emit progress(i,rowNum,orderTable.at(i).at(0)+tr("铸造成功,等待同步中.."));
            emit writeTable(i,tr("同步中.."));
        }else{
            emit progress(i,rowNum,orderTable.at(i).at(0)+tr("铸造失败"));
            emit writeTable(i,tr("铸造失败"));
            continue;
        }
        //开始等待同步完成
        do{
            sleep(10000);
            arg.clear();
            arg<<"wallet"<<"nft"<<"list"
                <<"-f"<<theMintArg.fingerPrint
               <<"-i"<<theMintArg.NFTWalletId;
            process.start(theMintArg.chiaPath,arg);
            process.waitForStarted(300000);
            process.waitForFinished(300000);
            outPut = QString::fromLocal8Bit(process.readAllStandardOutput());
            qDebug()<<"延时等待同步中。。。";
        }while(!outPut.contains(orderTable.at(i).at(1)));
        emit progress(i,rowNum,orderTable.at(i).at(0)+tr("铸造成功"));
        emit writeTable(i,tr("铸造成功"));
    }
    emit progress(100,100,tr("全部铸造完成"));
}

void MintThread::sleep(int ms)
{
    QEventLoop loop;
    QTimer::singleShot(ms,&loop,SLOT(quit()));
    loop.exec();
}














