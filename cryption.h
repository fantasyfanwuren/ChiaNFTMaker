#ifndef CRYPTION_H
#define CRYPTION_H

#include <QObject>


class Cryption : public QObject
{
    Q_OBJECT
public:
    explicit Cryption(QObject *parent = nullptr);
    const QString getIdentyCode();//得到本机可读标记
    const QString textencrypt(const QString &originaltext);//将可读加密为加密byte
    const QString textdecrypt(const QString &ciphertext);//将加密byte解密成可读字符串

    bool writeRegedit(const QString &ciphertext); //将加密信息写入注册表
    void readRegedit(QString &regeditText);//读取注册表内的信息
    void readRemainder(QString &remainderText);//读取注册表余额
    bool use();
    void getRechargeArg(double _num,double _fee,const QString &_program,const QString &_fingerprint);
    void sleepMsec(int msec);//延迟

public slots:
    void recharge();//充值
private:
    const QString encrypt(const QString &original);//加密byte
    const QString decrypt(const QString &ciphertext);//解密byete
    const QString stringToByte(const QString &text);////将字符串转化为byte
    const QString byteToString(const QString &byte);//将byte转化为字符串
private:

    double num;
    double fee;
    QString program;
    QString fingerprint;

signals:
    void wrongMessage(QString);
    void rightMessage(QString);
    void finished();



};

#endif // CRYPTION_H
