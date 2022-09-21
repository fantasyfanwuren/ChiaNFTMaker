#ifndef OFFERTHREAD_H
#define OFFERTHREAD_H
#include "ChiaNFTMaker.h"
#include <QObject>

class OfferThread : public QObject
{
    Q_OBJECT
public:
    explicit OfferThread(OfferArg _theOfferArg,QObject *parent = nullptr);
    const QList<QStringList> & getTable();
    void setTable(const QList<QStringList> &_table);
public slots:
    void makeTable();
    void viewOrder();
    void run();
private:
    double getRandomDouble(int seed,double a,double b);
    void getProcessArg(int row,QStringList &arg);

signals:
    void progress(int,int,QString);
    void finishMakeTable();
    void finishViewOrder(QString);
    void writeTable(int,QString);
private:
    OfferArg theOfferArg;
    QList<QStringList> table;

};

#endif // OFFERTHREAD_H
