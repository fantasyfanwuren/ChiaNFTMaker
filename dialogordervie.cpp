#include "dialogordervie.h"
#include "ui_dialogordervie.h"

DialogOrderVie::DialogOrderVie(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogOrderVie)
{
    ui->setupUi(this);
    setWindowTitle(tr("查看所有指令"));
}

DialogOrderVie::~DialogOrderVie()
{
    delete ui;
}


void DialogOrderVie::showOrders(const QString &orders)
{
    ui->plainTextEdit->clear();
    ui->plainTextEdit->setPlainText(orders);
}
