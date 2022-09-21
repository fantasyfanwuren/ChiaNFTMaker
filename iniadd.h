#ifndef INIADD_H
#define INIADD_H

#include <QObject>

class IniAdd : public QObject
{
    Q_OBJECT
public:
    explicit IniAdd(QList<QStringList> &argTable,QObject *parent = nullptr);
    void getParts(const QList<QStringList> &arg);

private:
    void addPositionToTable(const QStringList &parts);
    void sleep(unsigned int msec);

public slots:
    void listAll();


private:
    QList<QStringList> partTree;
    QList<QStringList> &partTable;


signals:
    void write(int,int,QString);
    void prograss(int,int,QString);
    void finished();

};

#endif // INIADD_H
