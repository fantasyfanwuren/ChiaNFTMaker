#ifndef DIALOGORDERVIE_H
#define DIALOGORDERVIE_H

#include <QDialog>

namespace Ui {
class DialogOrderVie;
}

class DialogOrderVie : public QDialog
{
    Q_OBJECT

public:
    explicit DialogOrderVie(QWidget *parent = nullptr);
    ~DialogOrderVie();
    void showOrders(const QString &orders);

private:
    Ui::DialogOrderVie *ui;
};

#endif // DIALOGORDERVIE_H
