#ifndef FORMADD_H
#define FORMADD_H

#include <QWidget>


class QStandardItemModel;
class QStandardItem;



namespace Ui {
class FormAdd;
}

class FormAdd : public QWidget
{
    Q_OBJECT
private:
    Ui::FormAdd *ui;
    QStandardItemModel * model;
    QStandardItemModel * treeModel;
    QList<QStringList> treeString;

private:
    void listDir(const QString &path);

    void readSettings();

    void hidden(const QList<QStringList> &list);

    void showAll();

    void delay(int ms);

public:
    explicit FormAdd(QWidget *parent = nullptr);

    ~FormAdd();

    bool checkSettings(QString &result);

    bool checkModel(QList<QStringList> &result,QStringList &head,QString &message);

    void getNameRules(int &nameLen,int &nameBegin);
    void saveSettings();

signals:
    void progress(int,int,QString);

private slots:
    void on_pushButtonMetaAddDir_clicked();

    void on_pushButtonMetaClearDir_clicked();

    void on_pushButtonMetaIni_clicked();

    void on_write(const int &row,const int &colnum,const QString &text);

    void on_prograss(const int & value,const int & max,const QString &message);

    void on_finished();

    void on_pushButtonMetaDeleteChooseDir_clicked();

    void on_pushButtonMetaDeletePngChoose_clicked();

    void on_pushButtonMetaQuickAdjustment_clicked();

    void on_pushButtonMetaShowAll_clicked();

    void on_pushButtonMetaPieChart_clicked();

    void on_pushButtonMetaUuidGet_clicked();
};

#endif // FORMADD_H
