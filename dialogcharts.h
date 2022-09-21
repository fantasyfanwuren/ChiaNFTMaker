#ifndef DIALOGCHARTS_H
#define DIALOGCHARTS_H

#include <QDialog>

namespace Ui {
class DialogCharts;
}

class DialogCharts : public QDialog
{
    Q_OBJECT

public:
    explicit DialogCharts(const QList<QHash<QString,int>> &countArg
                          ,const QStringList &headArg
                          ,QWidget *parent = nullptr);
    ~DialogCharts();

private:
    Ui::DialogCharts *ui;
    QList<QHash<QString,int>> counts;
    QStringList head;
private slots:
    void on_pieHighLight(bool);

};

#endif // DIALOGCHARTS_H
