#include "dialoghelp.h"
#include "ui_dialoghelp.h"

DialogHelp::DialogHelp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogHelp)
{
    ui->setupUi(this);
    setWindowTitle(tr("帮助"));
}

DialogHelp::~DialogHelp()
{
    delete ui;
}
