#include "offerthread.h"
#include <QProcess>
#include <ctime>
#include <random>

OfferThread::OfferThread(OfferArg _theOfferArg,QObject *parent)
    : QObject{parent},theOfferArg{_theOfferArg}
{

}

void OfferThread::makeTable()
{
    QProcess process;
    QStringList arg;//chia wallet nft list -i -f
    arg<<"wallet"<<"nft"<<"list"
      <<"-i"<<theOfferArg.NFTWalletId
     <<"-f"<<theOfferArg.fingerprint;
    process.start(theOfferArg.chiaPath,arg);
    emit progress(1,100,tr("正在获取NFT列表..."));
    process.waitForStarted(300000);
    process.waitForFinished(300000);
    QString outPut = QString::fromLocal8Bit(process.readAllStandardOutput());
    if(outPut.contains("NFT identifier:")){
        emit progress(100,100,tr("获取NFT列表成功"));
    }else{
        emit progress(100,100,tr("获取NFT列表失败"));
    }
    srand(time(0));
    int i =0;
    QStringList tempList;
    while(true){
        tempList.clear();
        QString NFTid;
        QString NFTLink;
        int seed = rand();
        //获取NFTid
        i=outPut.indexOf("NFT identifier:",i);
        if(i==-1){
            break;
        }else{
            int j = outPut.indexOf("\n",i);
            i+=27;
            NFTid = outPut.mid(i,j-i).remove("\r");
        }
        //获取NFT链接
         i = outPut.indexOf("Metadata URIs:",i);
         if(i==-1){
             break;
         }else{
             i = outPut.indexOf("\n",i)+3;
             int j = outPut.indexOf("\n",i);
             NFTLink =  outPut.mid(i,j-i).remove("\r");
         }
        //获取定价
         QString price;
         if(theOfferArg.isRandomPrice and theOfferArg.lowPrice!=theOfferArg.upPrice){
             price = QString::number(getRandomDouble(seed,theOfferArg.lowPrice,theOfferArg.upPrice));
         }else{
             price = QString::number(theOfferArg.lowPrice);
         }
         tempList<<NFTLink<<NFTid<<price;
         table<<tempList;
    }
    emit finishMakeTable();
}

double OfferThread::getRandomDouble(int seed,double a,double b)
{
    std::default_random_engine e(seed);
    std::uniform_real_distribution<double> u(a,b);
    return u(e);
}

const QList<QStringList> & OfferThread::getTable()
{
    return table;
}

void OfferThread::viewOrder()
{
    QStringList arg;
    QString orders;
    orders.clear();
    int maxNum = table.count();
    for(int i=0;i<maxNum;++i){
        getProcessArg(i,arg);
        orders +="chia";
        orders +=" ";
        for(int j=0;j<arg.count();++j){
            orders += arg.at(j);
            orders += " ";
        }
        orders +="\n\n";
        if(i%100==0){
            emit progress(i,maxNum-1,tr("正在生成指令..."));
        }
        if(i == maxNum-1){
            emit progress(i,maxNum-1,tr("生成指令完成"));
            emit finishViewOrder(orders);
        }
    }

}

void OfferThread::run()
{
    QProcess process;
    QStringList arg;
    int maxNum = table.count();
    //0 链接，1 nftID,2价格 3 状态
    for(int i=0;i<maxNum;++i){
        //获得参数

        getProcessArg(i,arg);
        //执行chia命令
        process.start(theOfferArg.chiaPath,arg);
        process.waitForStarted(300000);
        process.waitForReadyRead(300000);
        QString outPut =QString::fromLocal8Bit(process.readAllStandardOutput());
        if(outPut.contains("Offers for NFTs will have royalties automatically added")){
            QString reply = "y\n";
            process.write(reply.toLocal8Bit());
            process.waitForReadyRead(300000);
            process.write(reply.toLocal8Bit());
            //等待彻底结束
            process.waitForReadyRead(300000);
            while(!process.waitForFinished(300000)){

            }
            emit writeTable(i,tr("生成成功"));
            emit progress(i,maxNum-1,tr("正在创建Offer...."));
        }else{
            emit writeTable(i,tr("生成失败"));
        }
    }
    emit progress(100,100,tr("生成offer完成"));
}

void OfferThread::setTable(const QList<QStringList> &_table)
{
    table = _table;
}

void OfferThread::getProcessArg(int row,QStringList &arg)
{
    //tr("NFT链接")<<tr("NFTID")<<tr("价格")
    arg.clear();
    arg<<"wallet"<<"make_offer"
      <<"-o"<<table.at(row).at(1)+":1"
     <<"-f"<<theOfferArg.fingerprint
    <<"-r"<<theOfferArg.receiveWalletId+":"+table.at(row).at(2);
    QString name = "1_"+table.at(row).at(1)+"_x_"+table.at(row).at(2)+"XCH.offer";
    arg<<"-p"<<theOfferArg.offerPath+"/"+name;
}

