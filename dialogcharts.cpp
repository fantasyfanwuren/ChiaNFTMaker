#include "dialogcharts.h"
#include "ui_dialogcharts.h"


#include <QtCharts/QPieSeries>
#include <QRect>
#include <QtCharts/QChartView>


DialogCharts::DialogCharts(const QList<QHash<QString,int>> &countArg
                           ,const QStringList &headArg
                           ,QWidget *parent) :
   QDialog(parent),
    ui(new Ui::DialogCharts),counts(countArg),head(headArg)
{
    ui->setupUi(this);
    //设置布局
    QWidget *theWideget = new QWidget;
    ui->scrollArea->setWidget(theWideget);
    QGridLayout * gridLayout = new QGridLayout;
    theWideget->setLayout(gridLayout);
    //添加空间
    for(int i=0;i<head.count();i++){
        //创建并添加数据
        QtCharts::QChartView * theChartsView = new QtCharts::QChartView;
        theChartsView->setMinimumHeight(300);
        QtCharts::QChart * theChart = theChartsView->chart();
        gridLayout->addWidget(theChartsView,i/2,i%2);
        QtCharts::QPieSeries *pieSeries = new QtCharts::QPieSeries;
        QHash <QString,int> currentCount = counts.at(i);
        QHash<QString, int>::iterator j;
        for(j=currentCount.begin();j!=currentCount.end();++j){
            pieSeries->append(j.key(),j.value());
        }
        //设置标签
        for(int k=0;k<pieSeries->slices().count();++k){
            QtCharts::QPieSlice *slice = pieSeries->slices().at(k);
            slice->setLabel(slice->label()
                            +QString::asprintf(":%.1f%%",slice->percentage()*100));
            connect(slice,SIGNAL(hovered(bool)),this,SLOT(on_pieHighLight(bool)));
            slice->setLabelPosition(QtCharts::QPieSlice::LabelOutside);
        }

        pieSeries->setLabelsVisible(true);
        theChart->setTitle(head.at(i));
        theChart->addSeries(pieSeries);
        theChart->legend()->setVisible(true);
        theChart->legend()->setAlignment(Qt::AlignLeft);

    }
    setWindowTitle(tr("饼状图分析"));
    showMaximized();

}


void DialogCharts::on_pieHighLight(bool show)
{
    QtCharts::QPieSlice *slice = (QtCharts::QPieSlice *)sender();
    slice->setExploded(show);
}

DialogCharts::~DialogCharts()
{
    delete ui;
}
