#include "iniadd.h"
#include <QTime>



IniAdd::IniAdd(QList<QStringList> &argTable,QObject *parent)
    : QObject{parent},partTable{argTable}
{

}

void IniAdd::getParts(const QList<QStringList> &arg)
{
    partTree = arg;
}

void IniAdd::listAll()
{
    for(int i=0;i<partTree.count();++i){
        addPositionToTable(partTree.at(i));
        emit prograss(i,partTree.count(),tr("正在计算所有可能性....."));
        sleep(500);
    }
    emit finished();
    deleteLater();
}

void IniAdd::addPositionToTable(const QStringList &parts)
{
    if(partTable.isEmpty()){//若一个部件都没有添加过
        for(int i=0;i<parts.count();++i){
            QStringList tempList;
            tempList<<parts.at(i);
            partTable<<tempList;
        }
    }else{//若之前添加过部件
        //首先，对原来的部件复制parts.count()次，在这个过程中需要发射信号write()
        int originalRowCount = partTable.count();//获取原本有多少行，以供复制使用
        //先复制
        int currentRow = originalRowCount;//当前要新生成的行初始化
        for(int i=1;i<parts.count();++i){//复制多少次，由新加入的parts数量决定
            for(int j=0;j<originalRowCount;++j){//每次复制多少行，复制哪些行，由originalRowCount控制
                QStringList tempList;
                tempList = partTable.at(j);//得到要复制的行的内容
                partTable<<tempList;
                currentRow++;//复制完成一行后，当前要新生成的行定位递增
            }
        }

        //再把新的部件依次添加进去
        currentRow = 0;
        //int currentColunm = partTable.at(0).count();
        for(int i=0;i<parts.count();++i){//要添加的部件是 parts.at(i)
            for(int j=0;j<originalRowCount;++j){//新部件要添加的次数是j，根据原有行数来确定数量
                QStringList tempList = partTable.at(currentRow);
                tempList<<parts.at(i);
                partTable.replace(currentRow,tempList);
                currentRow++;
            }
        }

    }


}

void IniAdd::sleep(unsigned int msec)
{
    QTime reachTime = QTime::currentTime().addMSecs(msec);
    while(QTime::currentTime() < reachTime);
}
