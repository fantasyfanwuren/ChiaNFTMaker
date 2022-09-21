#include "cryption.h"

#include <QStandardPaths>
#include <QByteArray>

#include <QHostInfo>
#include <QTime>
#include <QRandomGenerator>
#include <QProcess>
#include <QCoreApplication>
#include <QSettings>



const QString PAYEE_ADDRESS = "xch1ghj5wv5lxccp3t6wfhcl0qzpay3zu3hxygpf302cf460rupveuns6mlday";

const QList<int> password = {123,135,351,354,853,878,232,146,379,323,219,
                             223,145,456,723,3,2,1,5,7,4,2,
                             3,5,7,4,2,311,423,5,6,7,8,
                             5,3,4,6,8,932,134,2,3,4,3,
                             4,5,6,5,2,334,413,5,6,7,2,
                             2,3,4,5,6,734,178,2,3,4,5,
                             2,3,4,5,6,812,256,3,4,5,6,
                             7,2,3,4,5,612,745,8,9,3,4,
                             2,1,3,1,4,734,213,3,5,7,8,
                             2,4,5,2,1,412,234,5,6,7,8,
                             2,1,4,5,2,334,435,5,6,7,8,
                             3,4,5,2,7,445,256,3,7,9,5,
                             0,1,2,3,4,675,678,7,3,4,2,
                             2,3,4,5,6,237,356,2,1,3,4,
                             2,4,5,6,7,845,319,4,2,2,6};

Cryption::Cryption(QObject *parent)
    : QObject{parent}
{

}

const QString Cryption::getIdentyCode()
{
    //获取用户名
    QString userName = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    userName = userName.section("/",-1,-1);
    //获取本机名
    QString localHostName = QHostInfo::localHostName();
    //获取识别码
    QString orgStrong = userName+localHostName;
    qDebug()<<orgStrong;
    return stringToByte(orgStrong);
}

const QString Cryption::encrypt(const QString &original)
{//加密byte
    QStringList originalList = original.split("|");
    int beginNum = QRandomGenerator::system()->generate()%password.count();
    QString result;
    int j = beginNum;
    for(int i=0;i<originalList.count();++i){
        int temp = originalList.at(i).toInt();
        temp+=password.at(j);
        j++;
        if(j==password.count()){
            j=0;
        }
        result+=QString::number(temp)+"|";
    }
    return result +QString::number(beginNum);
}


const QString Cryption::decrypt(const QString &ciphertext)
{//解密byete
    QStringList cipherList = ciphertext.split("|");
    int beginNum = cipherList.last().toInt();
    int j = beginNum;
    QString result;
    for(int i=0;i<cipherList.count()-1;++i){
        int temp = cipherList.at(i).toInt();
        temp-=password.at(j);
        j++;
        if(j==password.count()){
            j=0;
        }
        if(i==cipherList.count()-2){
            result+=QString::number(temp);
        }else{
            result+=QString::number(temp)+"|";
        }
    }
    return result;
}


const QString Cryption::stringToByte(const QString &text)
{//将字符串转化为byte
    QByteArray orgByte = text.toLocal8Bit();
    QString result;
    for(int i=0;i<orgByte.count();++i){
        if(i == orgByte.count()-1){
            result+=QString::number((int)orgByte.at(i));
        }else{
            result+=QString::number((int)orgByte.at(i))+"|";
        }
    }
    return result;
}


const QString Cryption::byteToString(const QString &byte)
{//将byte转化为字符串
    QStringList byteList  = byte.split("|");
    QByteArray byteArray;
    for(int i=0;i<byteList.count();++i){
        int temp = byteList.at(i).toInt();
        byteArray.append(temp);
    }
    return QString::fromLocal8Bit(byteArray);
}


const QString Cryption::textencrypt(const QString &originaltext)
{////将字符串加密为byte
    QString byteText = stringToByte(originaltext);
    QString enByteText = encrypt(byteText);
    return enByteText;
}
const QString Cryption::textdecrypt(const QString &ciphertext)
{////将byte解密成字符串
    QString deByteText = decrypt(ciphertext);
    QString orgText = byteToString(deByteText);
    return orgText;
}


bool Cryption::writeRegedit(const QString &ciphertext)
{
    QSettings settings = QSettings("The Bit Party","ChiaNFTMaker");
    QString orgText = textdecrypt(ciphertext);
    QString ID = byteToString(getIdentyCode());
    if(orgText.contains(ID)){
        settings.setValue("cryption",ciphertext);
        return true;
    }else{
        return false;
    }
}


void Cryption::readRegedit(QString &regeditText)
{
    QSettings settings = QSettings("The Bit Party","ChiaNFTMaker");
    regeditText = settings.value("cryption","none").toString();
}

void Cryption::readRemainder(QString &remainderText)
{
    QString regeditText;
    readRegedit(regeditText);
    remainderText = textdecrypt(regeditText);
    QString ID = byteToString(getIdentyCode());
    remainderText.remove(ID);
    if(remainderText!="无限"){
        remainderText = QString::number(remainderText.toDouble());
    }
}
void Cryption::getRechargeArg(double _num,double _fee,const QString &_program,const QString &_fingerprint)
{
    num = _num;
    fee = _fee;
    program = _program;
    fingerprint = _fingerprint;
}


void Cryption::recharge()
{
    //得到当前余额
    QString remainderText;
    readRemainder(remainderText);
    if(remainderText=="无限"){
        emit wrongMessage(tr("您拥有无限使用权，无需充值！"));
        emit finished();
        return;
    }
    //打款
    QProcess process;
    QStringList arg;
    arg<<"wallet"<<"send";
    //要发送的xch数量
    arg<<"-a"<<QString::number(num);
    //指纹
    arg<<"-f"<<fingerprint;
    //交易费
    arg<<"-m"<<QString::number(fee);
    //收款地址-t 收款地址
    arg<<"-t"<<PAYEE_ADDRESS;
    //开始进行打款操作
    qDebug()<<"打款arg"<<arg;
    process.start(program,arg);
    process.waitForStarted(300000);
    process.waitForFinished(300000);
    QString outPut = QString::fromLocal8Bit(process.readAllStandardOutput());
    qDebug()<<"outPut"<<outPut;
    if(outPut.contains("chia wallet get_transaction")){
        emit rightMessage(tr("支付成功，正在等待确认..."));
    }else{
        emit wrongMessage(tr("支付失败"));
        emit finished();
        return;
    }
    //循环等待区块链确认
    int startNum = outPut.indexOf("wallet get_transaction");
    int finishNum = outPut.indexOf("to get status")-3;
    int len = finishNum-startNum+1;
    QString outPutUse = outPut.mid(startNum,len);
    QStringList outPutUseList = outPutUse.split(" ");
    qDebug()<<"outPutUseList"<<outPutUseList;
    while(true){
        process.start(program,outPutUseList);
        process.waitForStarted(300000);
        process.waitForFinished(300000);
        outPut = QString::fromLocal8Bit(process.readAllStandardOutput());
        qDebug()<<"outPut"<<outPut;
        if(outPut.contains("Confirmed")){
            //写注册表
            double remainder = remainderText.toDouble();
            remainder+=num;
            QString ID = byteToString(getIdentyCode());
            remainderText = ID+QString::number(remainder);
            //加密新的余额信息到注册表
            QString ciphertext = textencrypt(remainderText);
            writeRegedit(ciphertext);

            emit rightMessage(tr("已同步，充值成功！"));
            break;
        }else{
            sleepMsec(30000);
        }
    }
    emit finished();
    deleteLater();
}


void Cryption::sleepMsec(int msec)
{
    QTime curTimer = QTime::currentTime().addMSecs(msec);
    while (QTime::currentTime() < curTimer)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

bool Cryption::use()
{
    //得到当前余额
    QString remainderText;
    readRemainder(remainderText);
    if(remainderText=="无限"){
        return true;
    }
    //计算消耗0.01个币后的剩余币的数量
    double remainder = remainderText.toDouble()-0.01;
    if(remainder<0){
        return false;
    }
    //修改注册表
    QString ID = byteToString(getIdentyCode());
    remainderText = ID+QString::number(remainder);
    //加密新的余额信息到注册表
    QString ciphertext = textencrypt(remainderText);
    writeRegedit(ciphertext);
    return true;
}



