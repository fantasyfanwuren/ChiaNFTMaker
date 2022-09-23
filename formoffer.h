#ifndef FORMOFFER_H
#define FORMOFFER_H
#include "ChiaNFTMaker.h"
#include <QWidget>

namespace Ui {
class FormOffer;
}
class QStandardItemModel;
class OfferThread;
class DialogOrderVie;

class FormOffer : public QWidget
{
    Q_OBJECT

public:
    explicit FormOffer(QWidget *parent = nullptr);
    ~FormOffer();
    void run();
    bool isRunable();
    void saveSettings();
private:
    void readSettings();
    bool getArgs();
    void refreshOfferThreadTable();

signals:
    void progress(int,int,QString);

private slots:
    void on_pushButtonOfferDeamonDirChoose_clicked();

    void on_pushButtonOfferSaveDirChoose_clicked();

    void on_pushButtonOfferWalletListView_clicked();

    void on_getWalletWrong(QString);

    void on_walletIdover(QList<QStringList>);

    void on_radioButtonOfferUniformPrice_toggled(bool checked);

    void on_doubleSpinBoxLowPrice_valueChanged(double arg1);

    void on_pushButtonOfferLayout_clicked();

    void on_progress(int,int,QString);
    void on_finishMakeTable();
    void on_writeTable(int,QString);
    void on_pushButtonOfferOrderView_clicked();
    void on_finishViewOrder(QString orderText);

    void on_pushButtonOfferLayoutDelete_clicked();

private:
    Ui::FormOffer *ui;
    QStandardItemModel *walletIdModel;
    QStandardItemModel *orderTableModel;
    OfferArg theOfferArg;
    OfferThread *theOfferThread;
    DialogOrderVie *dialogOrderView;
};

#endif // FORMOFFER_H
