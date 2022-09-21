#ifndef FORMMINT_H
#define FORMMINT_H

#include "ChiaNFTMaker.h"
#include <QWidget>
#include <QtNetwork>

namespace Ui {
class FormMint;
}

class QStandardItemModel;
class MintThread;
class DialogOrderVie;

class FormMint : public QWidget
{
    Q_OBJECT

public:
    explicit FormMint(QWidget *parent = nullptr);
    ~FormMint();
    void runOrder();
    bool isEnableRun();
    void saveSettings();
private:
    void makeConnect();
    void readSettings();
    void listWalletId();
    void makeDIDWallet();
    void makeNFTWallet();
    bool getMintArgs(QString &message);
    void downLoadLicnse();

signals:
    void progress(int,int,QString);

private slots:
    void on_pushButtonMintChooseChia_clicked();

    void on_pushButtonMintWalletListView_clicked();

    void on_getWalletWrong(const QString &message);
    void on_walletIdover(const QList<QStringList> &table);

    void on_pushButtonMintDIDWalletCreate_clicked();

    void on_pushButtonMintNFTWalletCreate_clicked();

    void on_pushButtonMintRoyaltyAdd_clicked();

    void on_pushButtonMintRoyaltyDelete_clicked();

    void on_pushButtonMintRoyaltyClear_clicked();

    void on_pushButtonViewPngLink_clicked();

    void on_pushButtonViewMetaLink_clicked();

    void on_pushButtonViewLicenceLink_clicked();

    void on_pushButtonMintPngDirChoose_clicked();

    void on_pushButtonMintMetaDirChoose_clicked();

    void orderGenerate();

    void on_writeTable(QList<QStringList> tabel);

    void on_progress(int value,int max,QString message);

    void on_pushButtonMintOrderView_clicked();

    void on_finishViewOrders(QString orders);
    void on_downloadFinished();
    void on_downloadProgress(qint64,qint64);
    void on_writeTable(int,QString);


private:
    Ui::FormMint *ui;
    QStandardItemModel *walletIdModel;
    QStandardItemModel *royaltyModel;
    QStandardItemModel *orderModel;
    MintThread * theMintThread;
    MintArg theMintArg;
    QNetworkReply * reply;
    QNetworkAccessManager networkManager;
    DialogOrderVie * orderViewDialog;
};

#endif // FORMMINT_H
