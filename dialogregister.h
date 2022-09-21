#ifndef DIALOGREGISTER_H
#define DIALOGREGISTER_H

#include <QDialog>

namespace Ui {
class DialogRegister;
}


class DialogRegister : public QDialog
{
    Q_OBJECT

public:
    explicit DialogRegister(QWidget *parent = nullptr);
    ~DialogRegister();
    void readRemainder();
private slots:
    void on_pushButtonRecharge_clicked();
    void on_wrongMessage(const QString &message);
    void on_rightMessage(const QString &message);
    void on_finished();

private:
    void readSettings();
    void iniStyle();

private:
    Ui::DialogRegister *ui;
};

#endif // DIALOGREGISTER_H
