#ifndef LISTWALLETIDTHREAD_H
#define LISTWALLETIDTHREAD_H

#include <QObject>

class ListWalletIDThread : public QObject
{
    Q_OBJECT
public:
    explicit ListWalletIDThread(const QString &_chiaFilePath,const QString &_fingerprint,QObject *parent = nullptr);
    void getDIDName(const QString &_DIDName);
    void getNFTDi(const QString &_NFTDi);


private:
    QString chiaFilePath;
    QString fingerprint;
    QString DIDName;
    QString NFTDi;
    QList<QStringList> table;

public slots:
    void listWallet();
    void makeDID();
    void makeNFT();

signals:
    void finished();
    void walletIdover(QList<QStringList>);
    void wrongMessage(QString);

};

#endif // LISTWALLETIDTHREAD_H
