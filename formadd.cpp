#include "formadd.h"
#include "qdebug.h"
#include "ui_formadd.h"
#include "iniadd.h"
#include "dialogsort.h"
#include "dialogcharts.h"


#include <QFile>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QSettings>
#include <QThread>
#include <QMessageBox>
#include <QtDebug>
#include <QTime>
#include <QDesktopServices>


#define DELAY_TIME 10



FormAdd::FormAdd(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormAdd)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground,true);

    model = new QStandardItemModel(this);
    ui->tableView->setModel(model);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    treeModel = new QStandardItemModel(this);
    treeModel->setHorizontalHeaderLabels(QStringList()<<tr("部位->部件"));
    ui->treeView->setModel(treeModel);
    readSettings();
}

FormAdd::~FormAdd()
{
    delete ui;
}
void FormAdd::saveSettings()
{
    //获取NFT名称
    QSettings settings = QSettings("The Bit Party","ChiaNFTMaker");
    QString name = ui->lineEditMetaName->text();
    settings.setValue("formAddName",name);
    //获取 Describe
    QString describe = ui->lineEditMetaDescribe->text();
    settings.setValue("formAddDescribe",describe);
    //获取Discord
    QString discord = ui->lineEditMetaDiscord->text();
    settings.setValue("formAddDiscord",discord);
    //获取Website
    QString website = ui->lineEditMetaOfficialWebsite->text();
    settings.setValue("formAddWebsite",website);
    //获取Twitter
    QString twitter = ui->lineEditMetaTwitter->text();
    settings.setValue("formAddTwitter",twitter);
    //获取Uuid
    QString uuid = ui->lineEditMetaUuid->text();
    settings.setValue("formAddUUID",uuid);
    //获取namelen
    int nameLen = ui->spinBoxMetaNameLen->value();
    settings.setValue("formAddNameLen",nameLen);
    //获取开始编号
    int nameBegin = ui->spinBoxMetaNameBegin->value();
    settings.setValue("formAddNameBegin",nameBegin);
}
void FormAdd::readSettings()
{
    //获取NFT名称
    QSettings settings = QSettings("The Bit Party","ChiaNFTMaker");
    QString name = settings.value("formAddName").toString();
    ui->lineEditMetaName->setText(name);
    //获取 Describe
    QString describe = settings.value("formAddDescribe").toString();
    ui->lineEditMetaDescribe->setText(describe);
    //获取Discord
    QString discord = settings.value("formAddDiscord").toString();
    ui->lineEditMetaDiscord->setText(discord);
    //获取Website
    QString website = settings.value("formAddWebsite").toString();
    ui->lineEditMetaOfficialWebsite->setText(website);
    //获取Twitter
    QString twitter = settings.value("formAddTwitter").toString();
    ui->lineEditMetaTwitter->setText(twitter);
    //获取Uuid
    QString uuid = settings.value("formAddUUID").toString();
    ui->lineEditMetaUuid->setText(uuid);
    //获取namelen
    int nameLen = settings.value("formAddNameLen").toInt();
    ui->spinBoxMetaNameLen->setValue(nameLen);
    //获取开始编号
    int nameBegin = settings.value("formAddNameBegin").toInt();
    ui->spinBoxMetaNameBegin->setValue(nameBegin);
}

void FormAdd::listDir(const QString &path)
{
    QDir dir(path);


    QStringList names = dir.entryList(QDir::Files);
    names.removeOne(".");
    names.removeOne("..");

    QStandardItem * item = new QStandardItem(dir.dirName());
    item->setData(path);
    treeModel->appendRow(item);
    for(int i=0;i<names.length();++i){
        QStandardItem * ChilidItem = new QStandardItem(names.at(i));
        item->appendRow(ChilidItem);
    }
    dir.cdUp();
    QSettings settings = QSettings("The Bit Party","ChiaNFTMaker");
    settings.setValue("formAddDir",dir.path());
}

void FormAdd::on_pushButtonMetaAddDir_clicked()
/*
 * ①通过对话框得到一个文件夹
 * ②获取字文件夹下的所有文件（png格式）
 * ③将其添加到列表尾部
 */
{
    QSettings settings = QSettings("The Bit Party","ChiaNFTMaker");

    QString iniDir = settings.value("formAddDir",".").toString();
    QString forAddDir = QFileDialog::getExistingDirectory(this,tr("选择文件夹"),iniDir);
    if(!forAddDir.isEmpty()){

        listDir(forAddDir);
    }
}


void FormAdd::on_pushButtonMetaClearDir_clicked()
{
    treeModel->removeRows(0,treeModel->rowCount());
}


void FormAdd::on_write(const int &row,const int &colnum,const QString & text)
{
    QStandardItem *tempItem = new QStandardItem(text);
    model->setItem(row,colnum,tempItem);
    ui->tableView->scrollTo(model->index(row,colnum));

}


void FormAdd::on_prograss(const int & value,const int & max,const QString &message)
{
    emit progress(value,max,message);
}


void FormAdd::on_finished()
{
    ui->pushButtonMetaIni->setEnabled(true);
}


void FormAdd::on_pushButtonMetaIni_clicked()
{//初始化功能
    if(treeModel->rowCount()==0){
        QMessageBox::information(this,tr("提醒"),tr("未进行部件集合文件夹的添加设置,\n请先选择文件夹"));
        return;
    }

    //设置表头及创建treeString
    ui->pushButtonMetaIni->setEnabled(false);
    model->clear();
    treeString.clear();
    QStringList header;
    for(int i=0;i<treeModel->rowCount();++i){
        QStandardItem * tempItem = treeModel->item(i);
        QStringList tempList;
        tempList.clear();
        header<<tempItem->text();
        for(int j=0;j<tempItem->rowCount();++j){
            tempList<<tempItem->child(j)->text();
        }
        treeString<<tempList;
    }
    model->setHorizontalHeaderLabels(header);
    //开启一个线程，后台计算所有组合的可能行，并赋值给QList<QStringList> partTable
    QThread *thread = new QThread();
    QList<QStringList> partTable;
    IniAdd *iniAdd = new IniAdd(partTable);
    iniAdd->moveToThread(thread);
    connect(thread,SIGNAL(started()),iniAdd,SLOT(listAll()));
    connect(iniAdd,SIGNAL(prograss(int,int,QString))
            ,this,SLOT(on_prograss(int,int,QString)));
    connect(iniAdd,SIGNAL(finished()),thread,SLOT(quit()));
    iniAdd->getParts(treeString);
    thread->start();
    //非阻塞等待线程结束
    while (!thread->isFinished()) {
        delay(DELAY_TIME*10);
    }
    int totalRows = partTable.count();
    int totalCols = treeString.count();

    for(int i=0;i<totalRows;++i){
        for(int j=0;j<totalCols;++j){
            QStandardItem * tempItem = new QStandardItem(partTable.at(i).at(j));
            model->setItem(i,j,tempItem);
        }
        emit progress(i,totalRows-1,"正在准备表格....");
        if(i%100==0){
            delay(DELAY_TIME);
            ui->tableView->scrollTo(model->index(i,0));

        }
    }
    emit progress(100,100,"初始化完成！");
    ui->pushButtonMetaIni->setEnabled(true);
    //根据总量设置nft的名称长度
    QString tatalNum = QString::number(model->rowCount());
    int nameLen = tatalNum.length();
    if(ui->spinBoxMetaNameLen->value()<nameLen){
        ui->spinBoxMetaNameLen->setValue(nameLen);
    }
}

void FormAdd::delay(int ms)
{
   QCoreApplication::processEvents(QEventLoop::AllEvents, ms);
}


void FormAdd::on_pushButtonMetaDeleteChooseDir_clicked()
{//treeModel 删除选中
    if(treeModel->rowCount()!=0){
        QModelIndex childIndex = ui->treeView->selectionModel()->currentIndex();
        QStandardItem * tempItem = treeModel->itemFromIndex(childIndex);
        if(tempItem->parent()==nullptr){
            treeModel->removeRow(childIndex.row());
        }else{
            treeModel->removeRow(childIndex.row(),tempItem->parent()->index());
        }
    }else{
        QMessageBox::information(this,tr("提醒"),tr("未进行部件集合文件夹的添加设置,\n请先选择文件夹"));
    }
}


void FormAdd::on_pushButtonMetaDeletePngChoose_clicked()
{/*对所有组合删除选中行*/
    if(model->rowCount()!=0){
        QModelIndexList indexList = ui->tableView->selectionModel()->selectedIndexes();
        QList<int> deleteRow;
        //筛选出非重复的行
        for(int i=0;i<indexList.count();++i){
            if(!deleteRow.contains(indexList.at(i).row())
                    and !ui->tableView->isRowHidden(indexList.at(i).row())){
                deleteRow<<indexList.at(i).row();
            }
        }
        //执行删除行的命令
        for(int i=0;i<deleteRow.count();++i){
            model->removeRow(deleteRow.at(i)-i);
        }
    }else{
        QMessageBox::information(this,tr("提醒"),tr("未检测到任何组合可能,\n请先点击初始化"));
    }
}




void FormAdd::on_pushButtonMetaQuickAdjustment_clicked()
{/*筛选功能*/
    //判断是否进行了初始化
    if(model->rowCount()==0){
        QMessageBox::information(this,tr("提醒"),tr("未检测到任何组合可能,\n请先点击初始化"));
        return;
    }
    //对treeModel进行提取进入QList<QStringList> 中
    QList<QStringList> tempTreeString;
    for(int i=0;i<treeModel->rowCount();++i){
        QStringList tempString;
        tempString.clear();
        QStandardItem * tempItem = treeModel->item(i);
        tempString<<tempItem->text();
        for(int j=0;j<tempItem->rowCount();++j){
            tempString<<tempItem->child(j)->text();
        }
        tempTreeString<<tempString;
    }
    DialogSort dialogSort(tempTreeString,this);
    if(dialogSort.exec()==QDialog::Accepted){
        //先把隐藏的全部恢复
        showAll();
        hidden(dialogSort.getSortRule());
    }



}


void FormAdd::hidden(const QList<QStringList> &list)
{
    for(int i=0;i<list.count();++i){
        QStringList hideRuleString = list.at(i);
        QString head = hideRuleString.at(0);
        //定位列
        int currentCol=0;
        for(int k=0;k<list.count();++k){
            if(model->horizontalHeaderItem(k)->text()==head){
                currentCol = k;
                break;
            }
        }
        qDebug()<<currentCol;
        //开始隐藏

        for(int row=0;row<model->rowCount();++row){
            QStandardItem *tempItem = model->item(row,currentCol);
            if(!hideRuleString.contains(tempItem->text())){
                ui->tableView->setRowHidden(row,true);
            }
        }
    }
}


void FormAdd::on_pushButtonMetaShowAll_clicked()
{
    if(model->rowCount()==0){
        QMessageBox::information(this,tr("提醒"),tr("未检测到任何组合可能,\n请先点击初始化"));
        return;
    }
    showAll();
}

void FormAdd::showAll()
{
    for(int i=0;i<model->rowCount();++i){
        if(ui->tableView->isRowHidden(i)){
            ui->tableView->setRowHidden(i,false);
        }
    }
}


void FormAdd::on_pushButtonMetaPieChart_clicked()
{
    if(model->rowCount()==0){
        QMessageBox::information(this,tr("提醒"),tr("未检测到任何组合可能,\n请先点击初始化"));
        return;
    }

    QStringList head;
    head.clear();
    QList<QHash<QString,int>> count;
    count.clear();
    //逐列查找
    for(int i=0;i<model->columnCount();++i){
        QHash<QString,int> tempHash;
        QStringList toFindStrings = treeString.at(i);
        for(int j=0;j<toFindStrings.count();++j){
            QList<QStandardItem *> items = model->findItems(toFindStrings.at(j)
                                                            ,Qt::MatchExactly,i);
            tempHash[toFindStrings.at(j)] = items.count();
        }
        count<<tempHash;
        head<<model->horizontalHeaderItem(i)->text();
    }
    DialogCharts dialogCharts = DialogCharts(count,head,this);
    dialogCharts.exec();
}

bool FormAdd::checkSettings(QString &result)
{
    QSettings settings = QSettings("The Bit Party","ChiaNFTMaker");
    if(ui->lineEditMetaDescribe->text().isEmpty()){
        result+=tr("描述、");
    }else{
        settings.setValue("formAddDescribe",ui->lineEditMetaDescribe->text());
    }

    if(ui->lineEditMetaDiscord->text().isEmpty()){
        result+=tr("Discord、");
    }else{
        settings.setValue("formAddDiscord",ui->lineEditMetaDiscord->text());
    }

    if(ui->lineEditMetaOfficialWebsite->text().isEmpty()){
        result+=tr("官网、");
    }else{
        settings.setValue("formAddWebsite",ui->lineEditMetaOfficialWebsite->text());
    }

    if(ui->lineEditMetaTwitter->text().isEmpty()){
        result+=tr("推特、");
    }else{
        settings.setValue("formAddTwitter",ui->lineEditMetaTwitter->text());
    }

    if(ui->lineEditMetaUuid->text().isEmpty()){
        result+=tr("UUID、");
    }else{
        settings.setValue("formAddUUID",ui->lineEditMetaUuid->text());
    }

    if(ui->lineEditMetaName->text().isEmpty()){
        result+=tr("NFT集合名称、");
    }else{
        settings.setValue("formAddName",ui->lineEditMetaName->text());
    }

    if(result.isEmpty()){
        return true;
    }else{
        return false;
    }
}


bool FormAdd::checkModel(QList<QStringList> &result,QStringList &head,QString &message)
{

    if(treeModel->rowCount()==0){
        message = tr("未设置保存部件图片的文件夹");
        return false;
    }
    if(model->rowCount()==0){
        message = tr("未初始化");
        return false;
    }


    //获取表头信息
    for(int i=0;i<treeModel->rowCount();++i){
        head<<treeModel->item(i)->data().toString();
    }

    //逐行获取表格内容
    for(int i=0;i<model->rowCount();++i){
        QStringList rowList;
        rowList.clear();
        for(int j=0;j<model->columnCount();++j){
            rowList<<model->item(i,j)->text();
        }
        result<<rowList;
    }
    return true;

}

void FormAdd::getNameRules(int &nameLen,int &nameBegin)
{
    QSettings settings = QSettings("The Bit Party","ChiaNFTMaker");
    nameLen = ui->spinBoxMetaNameLen->value();
    settings.setValue("formAddNameLen",nameLen);
    nameBegin = ui->spinBoxMetaNameBegin->value();
    settings.setValue("formAddNameBegin",nameBegin);

}


void FormAdd::on_pushButtonMetaUuidGet_clicked()
{
    QDesktopServices::openUrl(QUrl(QString("https://www.uuidgenerator.net/")));
}

