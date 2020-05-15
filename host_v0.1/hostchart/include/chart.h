#ifndef CHART_H
#define CHART_H

#include<qt5/QtWidgets/QMainWindow>
#include<qt5/QtCharts/QChartView>
#include<qt5/QtCharts/QChart>
#include<qt5/QtCharts/QLineSeries>
#include<qt5/QtCharts/QScatterSeries>
#include<QScreen>
#include<QHBoxLayout>
#include<QPushButton>
#include<QFileDialog>
#include<qt5/QtCharts/QBarSet>
#include<qt5/QtCharts/QBarCategoryAxis>
#include<qt5/QtCharts/QPercentBarSeries>
#include<QTimer>
#include<set>
#include<mutex>

QT_CHARTS_USE_NAMESPACE

struct host{            //similar to database
    std::string id;     //unique id for every row
    std::string nodeId;
    std::string timetag;
    std::string attribute;
    std::string value;
    std::string notes;
};

class Chart:public QMainWindow{
    Q_OBJECT

public:
    Chart(QWidget *parent=nullptr,std::string lineTitle="null",std::string barTitle="null");
    ~Chart();
    void timeToDate(struct timeval tv,char *time);
    void dataSave(int n,std::string attribute,std::string notes="");          //save data into vector<host>, as well as the same name method below
    void dataSave(float n,std::string attribute,std::string note="");
    void getData(int interval);   //save every type data periodly by SIGNAL 'timerUpdate'

private slots:
    void update();                //SLOT for 'timerUpdate', update vector<host> by process()
    void result();                //SLOT for 'timer', update chart for display
    void onBtnClicked1();
    void onBtnClicked2();
    bool onBtnClicked3();
    void onBtnClicked4();

private:
    QChart *chartline=NULL;
    QChart *chartbar=NULL;
    QChartView *chartViewline=NULL;
    QChartView *chartViewbar=NULL;
    QLineSeries *lineSeries=NULL;
    QScatterSeries *scatterSeries=NULL;
    QHBoxLayout *layout=NULL;
    QHBoxLayout *vlayout=NULL;
    QGridLayout *baseLayout=NULL;

    QBarSet *set0 = NULL;
    QBarSet *set1 = NULL;
    QPercentBarSeries *series=NULL;

    QPushButton *btn1=NULL;
    QPushButton *btn2=NULL;
    QPushButton *btn3=NULL;
    QPushButton *btn4=NULL;
    bool pause=false;
    bool saveFile(const QString &fileName);

    QTimer timer;
    QTimer timerUpdate;
    QList<float> lineData; // store data's list
    float barData;
    QList<int> lineDataInt; // store data's list
    int barDataInt;
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
