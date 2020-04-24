#ifndef TABLE_H
#define TABLE_H

#include<QTableView>
#include<QTimer>
#include<QVector>
#include<QStandardItemModel>

#include<mutex>

struct host{            //similar to database
    std::string id;     //unique id for every row
    std::string time;
    std::string ip;     //local ip address
    std::string pid;    //process number
    std::string type;   //data type, such as int, float, double, string
    std::string value;
};

class Table:public QTableView{
    Q_OBJECT

public:
    Table(QWidget *parent=NULL);
    ~Table();
    void timeToDate(struct timeval tv,char *time);
    void process(int n);          //save data into vector<host>, as well as the same name method below
    void process(float n);
    void process(double n);
    void process(std::string n);
    void getData(int interval);   //save every type data periodly by SIGNAL 'timerUpdate'

private slots:
    void update();                //SLOT for 'timerUpdate', update vector<host> by process()
    void result();                //SLOT for 'timer', update model's data for display

private:
    QStandardItemModel *model;
    QTimer timer;
    QTimer timerUpdate;
    std::vector<host> host_data;
    std::mutex lock_s;
    int a;
    float b;
    double c;
    std::string d;
};
#endif
