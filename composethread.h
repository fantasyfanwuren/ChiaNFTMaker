#ifndef COMPOSETHREAD_H
#define COMPOSETHREAD_H

#include "ChiaNFTMaker.h"

#include <QObject>

class ComposeThread : public QObject
{
    Q_OBJECT
public:
    explicit ComposeThread( QString &_name,
                            QString &_describe,
                            QString &_discord,
                            QString &_website,
                            QString &_twitter,
                            QString &_uuid,
                            QList<QStringList> &_modelContent,
                            QStringList &_head,
                            QObject *parent = nullptr);

    void setNameRules(const ThreadNumRule &_rule);
private:
    const QString getNameString(const int &nameNum,const int &nameLen,const QString &format);
    void makeLicense();


private:
     QString &name;
     QString &describe;
     QString &discord;
     QString &website;
     QString &twitter;
     QString &uuid;
     QList<QStringList> &modelContent;
     QStringList &head;
     ThreadNumRule rule;

public slots:
     void run();


signals:
     void exit();

};

#endif // COMPOSETHREAD_H
