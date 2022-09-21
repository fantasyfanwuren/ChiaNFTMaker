#ifndef MINTTHREAD_H
#define MINTTHREAD_H

#include "ChiaNFTMaker.h"
#include <QObject>




class MintThread : public QObject
{
    Q_OBJECT
public:
    explicit MintThread(const MintArg &thearg,QObject *parent = nullptr);
    void refreshMintArg(const MintArg & thearg);
    const QList<QStringList> &getOrderTable();

private:
    const QString getSha256(const QString &name) const;
    void getRandomroyaltyAdress(QStringList &result,int maxNum) const;

public slots:
    void makeOrder();
    void runOrder();
    void viewOrder();

private:

    MintArg theMintArg;
    QList<QStringList> orderTable;
    bool isViewMode;
    void sleep(int ms);

signals:
    void writeTable(QList<QStringList>);
    void progress(int,int,QString);
    void finishViewOrders(QString);
    void writeTable(int,QString);

};

#endif // MINTTHREAD_H
