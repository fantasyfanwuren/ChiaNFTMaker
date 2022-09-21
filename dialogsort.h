#ifndef DIALOGSORT_H
#define DIALOGSORT_H

#include <QDialog>

class QGridLayout;

namespace Ui {
class DialogSort;
}

class DialogSort : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSort(const QList<QStringList> &ss,QWidget *parent = nullptr);
    ~DialogSort();
    QList<QStringList> getSortRule();
private:
    void setWidget();

private:
    Ui::DialogSort *ui;
    QList<QStringList> partsTree;
    QGridLayout * gridLayout;
    QWidget * theWidget;
};

#endif // DIALOGSORT_H
