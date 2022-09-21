#include "dialogabout.h"
#include "ui_dialogabout.h"

DialogAbout::DialogAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAbout)
{
    ui->setupUi(this);
    setWindowTitle(tr("About Chia NFT Maker"));
}

DialogAbout::~DialogAbout()
{
    delete ui;
}
