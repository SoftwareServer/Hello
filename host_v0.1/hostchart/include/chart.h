#ifndef CHART_H
#define CHART_H

#include<qt5/QtWidgets/QMainWindow>
#include<qt5/QtCharts/QChartView>
#include<qt5/QtCharts/QChart>
#include<qt5/QtCharts/QLineSeries>
#include<qt5/QtCharts/QScatterSeries>
#include<QHBoxLayout>
#include<qt5/QtCharts/QBarSet>
#include<qt5/QtCharts/QBarCategoryAxis>
#include<qt5/QtCharts/QPercentBarSeries>
#include<QTimer>
#include<set>
#include<mutex>

QT_CHARTS_USE_NAMESPACE

struct host{            //similar to database
    std::string id;     //unique id for every row
    std::string time;
    std::string ip;     //local ip address
    std::string pid;    //process number
    std::string type;   //data type, such as int, float, double, string
    std::string value;
    std::string comments;
};

class Chart:public QMainWindow{
    Q_OBJECT

public:
    Chart(QWidget *parent=nullptr,std::string lineTitle="null",std::string barTitle="null");
    ~Chart();
    void timeToDate(struct timeval tv,char *time);
    void dataSave(int n,std::string comm);          //save data into vector<host>, as well as the same name method below
    void dataSave(float n,std::string comm);
    void getData(int interval);   //save every type data periodly by SIGNAL 'timerUpdate'

private slots:
    void update();                //SLOT for 'timerUpdate', update vector<host> by process()
    void result();                //SLOT for 'timer', update chart for display

private:
    QChart *chartline;
    QChart *chartbar;
    QChartView *chartViewline;
    QChartView *chartViewbar;
    QLineSeries *lineSeries=NULL;
    QScatterSeries *scatterSeries=NULL;
    QHBoxLayout *layout=NULL;
    QGridLayout *baseLayout=NULL;

    QBarSet *set0 = NULL;
    QBarSet *set1 = NULL;
    QPercentBarSeries *series=NULL;

    QTimer timer;
    QTimer timerUpdate;
    QList<float> data; // store data's list
    float sig;
    QList<int> dataInt; // store data's list
    int sigInt;
    std::string lineTitle;
    std::string barTitle;
    std::vector<host> host_data;
    std::set<std::string> dic;
    std::set<std::string> dicInt;
    std::mutex lock_s;
    int a;
    float b;
};
#endif
