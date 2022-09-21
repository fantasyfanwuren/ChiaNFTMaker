#ifndef CHIANFTMAKER_H
#define CHIANFTMAKER_H
#include <QCoreApplication>


struct ThreadNumRule
{
    int nameLen;
    int startNum;
    int finishNum;
    int adjustNum;
};

struct MintArg
{
    MintArg() {}
    QString chiaPath;
    QString fingerPrint;
    QString NFTWalletId;
    QList<QStringList> royaltyAddress;
    QString targetAddress;
    int royalty;
    int fee;
    int currentVersion;
    int totalVersion;
    QString pngDir;
    QString pngLink;
    QString metaDir;
    QString metaLink;
    QString licenceLink;
    QString licenseHash;
    bool isOrderly;
};

//chiaPath;offerPath;fingerprint;NFTWalletId;receiveWalletId;fee;lowPrice;upPrice;isRandomPrice
struct OfferArg
{
    OfferArg() {}
    QString chiaPath; QString offerPath; QString fingerprint;
    QString NFTWalletId; QString receiveWalletId; int fee;
    double lowPrice; double upPrice; bool isRandomPrice;
};




#endif // CHIANFTMAKER_H
