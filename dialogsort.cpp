#include "dialogsort.h"
#include "ui_dialogsort.h"

#include <QGroupBox>
#include <QLayout>
#include <QCheckBox>

DialogSort::DialogSort(const QList<QStringList> &ss,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSort),partsTree(ss)
{
    ui->setupUi(this);
    setWidget();

}


void DialogSort::setWidget()
{
    //窗口尺寸
    int size_y = 100;
    int size_x = 200;
    theWidget = new QWidget(this);
    gridLayout = new QGridLayout();
    theWidget->setLayout(gridLayout);
    for(int i=0;i<partsTree.count();++i){
        QStringList tempString = partsTree.at(i);
        //创建一个装载要筛选的内容
        QGroupBox * tempGroupBox = new QGroupBox();
        //tempGroupBox->setMinimumSize(QSize(200,300));

        //设置组合框标题
        tempGroupBox->setTitle(tempString.at(0));
        //设置组合框为垂直布局
        QVBoxLayout *groupBoxLayout = new QVBoxLayout();
        QMargins margins = groupBoxLayout->contentsMargins();
        margins.setTop(16);
        groupBoxLayout->setContentsMargins(margins);
        tempGroupBox->setLayout(groupBoxLayout);
        for(int j=1;j<tempString.count();++j){
            QCheckBox * tempCheckBox = new QCheckBox();
            tempCheckBox->setText(tempString.at(j));
            tempCheckBox->setChecked(true);
            groupBoxLayout->addWidget(tempCheckBox);
        }
        //将 tempGroupBox 添加到原始的网格布局中

        gridLayout->addWidget(tempGroupBox,i/3,i%3);
    }
    ui->scrollArea->setWidget(theWidget);
    resize(size_x*3, size_y*3);
    setWindowTitle(tr("筛选"));
}


QList<QStringList> DialogSort::getSortRule()
{
    QList<QStringList> result;
    result.clear();
    QList<QGroupBox *> groupBoxs = theWidget->findChildren<QGroupBox *>();
    for(int i=0;i<groupBoxs.count();++i){
        QStringList tempList;
        tempList.clear();
        tempList<<groupBoxs.at(i)->title();
        QGroupBox * tempGroupBox = groupBoxs.at(i);
        QList<QCheckBox *> checkBoxs= tempGroupBox->findChildren<QCheckBox *>();
        for(int j=0;j<checkBoxs.count();++j){
            if(checkBoxs.at(j)->isChecked()){
                tempList<<checkBoxs.at(j)->text();
            }
        }
        result<<tempList;
    }
    return result;
}

DialogSort::~DialogSort()
{
    delete ui;
}

