#include "composethread.h"

#include <QDir>
#include <QPainter>
#include <QPixmap>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDesktopServices>

ComposeThread::ComposeThread(QString &_name
                             ,QString &_describe
                             , QString &_discord
                             , QString &_website
                             , QString &_twitter
                             , QString &_uuid
                             , QList<QStringList> &_modelContent
                             , QStringList &_head
                             ,QObject *parent
                             )
    : QObject{parent},name{_name},describe{_describe},discord{_discord},website{_website}
    ,twitter{_twitter},uuid{_uuid},modelContent{_modelContent},head{_head}
{

}


void ComposeThread::setNameRules(const ThreadNumRule &_rule)
{
    rule = _rule;
}


const QString ComposeThread::getNameString(const int &nameNum,const int &nameLen,const QString &format)
{
    QString resultName = QString::number(nameNum);
    int distance = nameLen-resultName.length();
    for(int i=0;i<distance;++i){
        resultName = "0"+resultName;
    }
    return resultName+format;
}

void ComposeThread::run()
{
    QString josnSavePath = QDir::currentPath()+"/josnSave";
    QString pngSavePath = QDir::currentPath()+"/pngSave";

    for(int i=rule.startNum;i<rule.finishNum;++i){

        //合成图片
        QStringList pngNames = modelContent.at(i-rule.adjustNum);
        QString beginImagePath = head.at(0)+"/"+pngNames.at(0);
        QPixmap beginImage(beginImagePath);
        QPainter tempPainter;
        tempPainter.begin(&beginImage);
        for(int j=1;j<head.count();++j){
            QString drawImagePath = head.at(j)+"/"+pngNames.at(j);
            QPixmap drawImage(drawImagePath);
            tempPainter.drawPixmap(0,0,drawImage);
        }
        tempPainter.end();
        QString savePath = pngSavePath+"/"+getNameString(i,rule.nameLen,".png");
        beginImage.save(savePath);

        //制作josn文件
        //①公有属性设置
        QJsonObject obj{
            {"format", "CHIP-0007"},
        };
        obj.insert("name",name+" #"+getNameString(i,rule.nameLen,""));
        obj.insert("description",describe);
        obj.insert("sensitive_content",false);

        QJsonObject collection;
        collection.insert("name",name);
        collection.insert("id",uuid);

        QJsonArray attributes{
                QJsonObject{
                    {"type","description"},
                    {"value",describe}
                }
        };
        QJsonObject websiteObj{
                    {"type","website"},
                    {"value",website}
        };
        attributes.append(websiteObj);
        QJsonObject discordObj{
                    {"type","discord"},
                    {"value",discord}
        };
        attributes.append(discordObj);
        QJsonObject twitterObj{
                    {"type","twitter"},
                    {"value",twitter}
        };
        attributes.append(twitterObj);
        collection.insert("attributes",attributes);
        obj.insert("collection",collection);

        //②私有属性设置
        QJsonArray theAttributes;
        for(int j=0;j<head.count();++j){
            QString trait_type = QDir(head.at(j)).dirName();
            QString value = pngNames.at(j).section(".",0,0);
            QJsonObject tempObj{
                {"trait_type",trait_type},
                {"value",value}
            };
            theAttributes.append(tempObj);
        }
        obj.insert("attributes",theAttributes);
        //③保存json文件
        QJsonDocument doc(obj);
        QByteArray docByte = doc.toJson();
        QFile fp(josnSavePath+"/"+getNameString(i,rule.nameLen,".json"));
        fp.open(QIODevice::WriteOnly);
        fp.write(docByte);
        fp.close();
    }
    emit exit();
    deleteLater();
}


void ComposeThread::makeLicense()
{
    QString licensePath = QDir::currentPath()+"/licenseSave";
    QDir licenseDir(licensePath);
    if(licenseDir.exists()){
        if(!licenseDir.removeRecursively()){
            return;
        }
    }
    //创建文件夹
    if(!licenseDir.mkdir(licensePath)){
        return;
    }
    //打开文件夹
    QDesktopServices::openUrl(QUrl::fromLocalFile(licensePath));
    QString licenseTextPath = licensePath +"/License.txt";
    QFile licensefile(licenseTextPath);
    QFile licensefileTemplate(":/License.txt");
    if(!licensefileTemplate.open(QIODevice::ReadOnly|QIODevice::Text)){
        return;
    }
    QString templateText = licensefileTemplate.readAll();
    licensefileTemplate.close();
    QString licenseText = templateText.replace("MyNFTNAME"
                                                   ,name);
    if(!licensefile.open(QIODevice::WriteOnly|QIODevice::Text)){
        return;
    }
    QTextStream tempStream(&licensefile);
    tempStream<<licenseText;
    licensefile.close();
}









