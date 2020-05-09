#include"chart.h"
#include"get_ip.cpp"
#include <QDebug>

Chart::Chart(QWidget *parent,std::string lineTitle,std::string barTitle): QMainWindow(parent){
    this->lineTitle=lineTitle;
    this->barTitle=barTitle;

    lineSeries = new QLineSeries();
    scatterSeries = new QScatterSeries();
    scatterSeries->setMarkerSize(8);
    chartline = new QChart();
    chartline->addSeries(lineSeries);
    chartline->addSeries(scatterSeries);
    chartline->legend()->hide();
    chartline->setTitle(QString::fromStdString(this->lineTitle));
    chartline->createDefaultAxes();
    chartline->axisX()->setRange(0, 30);
    chartline->axisY()->setRange(0, 100);

    set0 = new QBarSet("Signal");
    set1 = new QBarSet("100-Signal");
    set0->append(0);
    set1->append(1);
    set1->setColor(Qt::gray);
    series = new QPercentBarSeries();
    series->append(set0);
    series->append(set1);
    chartbar = new QChart();
    chartbar->addSeries(series);
    chartbar->setTitle(QString::fromStdString(this->barTitle));
    chartbar->setAnimationOptions(QChart::SeriesAnimations);
    QStringList categories;
    categories << "";
    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(categories);
    chartbar->createDefaultAxes();
    chartbar->setAxisX(axis, series);
    chartbar->legend()->setVisible(false);
    chartbar->legend()->setAlignment(Qt::AlignBottom);

    chartViewline = new QChartView(chartline);
    chartViewbar = new QChartView(chartbar);

    chartViewline->setRenderHint(QPainter::Antialiasing);
    chartViewbar->setRenderHint(QPainter::Antialiasing);

    layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(chartViewline);
    layout->addWidget(chartViewbar);
    layout->setStretchFactor(chartViewline,4);
    layout->setStretchFactor(chartViewbar,1);

    QWidget *pCentralWidget = new QWidget(this);

    baseLayout = new QGridLayout(pCentralWidget);
    baseLayout->addLayout(layout, 0, 0, 0, 0);

    setCentralWidget(pCentralWidget);

    connect(&timer, SIGNAL(timeout()), this, SLOT(result()));
    timer.start(2 * 1000);
    a=0;
    b=1.1;
    sig=0.0;
    sigInt=0.0;
}
Chart::~Chart(){
    this->close();
    if(NULL!=chartline){
	delete chartline;
	chartline=NULL;
    }
    if(NULL!=chartbar){
	delete chartbar;
	chartbar=NULL;
    }
    if(NULL!=chartViewline){
	delete chartViewline;
	chartViewline=NULL;
    }
    if(NULL!=chartViewbar){
	delete chartViewbar;
	chartViewbar=NULL;
    }
    if(NULL!=lineSeries){
	delete lineSeries;
	lineSeries=NULL;
    }
    if(NULL!=scatterSeries){
	delete scatterSeries;
	scatterSeries=NULL;
    }
    if(NULL!=layout){
	delete layout;
	layout=NULL;
    }
    if(NULL!=baseLayout){
	delete baseLayout;
	baseLayout=NULL;
    }
    if(NULL!=set0){
	delete set0;
	set0=NULL;
    }
    if(NULL!=set1){
	delete set1;
	set1=NULL;
    }
}
void Chart::update(){
    dataSave(++a,"robot num");
    dataSave(rand()%100,"packets");
    dataSave(rand()%1000,"data length");
    dataSave(++b,"speed");
    dataSave((float)(rand()%100+0.1),"signal");
    dataSave((float)(rand()%100+0.3),"error rate");
    //qDebug()<<host_data.size();
}
void Chart::result(){
    if(dic.find(this->lineTitle)!=dic.end()){
	//qDebug()<<"find "<<this->lineTitle.c_str();
	// more than max, delete the early data
	while (data.size() > 31) {
	    data.removeFirst();
	}
	// hide
	float max=0;
	if(!data.empty()){
	    max=data.at(0);
	}
	if(isVisible()){
	    lineSeries->clear();
	    scatterSeries->clear();
	    int dx = 30 / (31-1);
	    int less = 31 - data.size();
	    for (int i = 0; i < data.size(); ++i) {
		if(data.at(i)>max){
		    max=data.at(i);
		}
		lineSeries->append(less*dx+i*dx, data.at(i));
		scatterSeries->append(less*dx+i*dx, data.at(i));
	    }
	}
	if(100<max){
	    chartline->axisY()->setRange(0, ((int)max+2));
	}
	else{
	    chartline->axisY()->setRange(0, 100);
	}
    }
    else if(dicInt.find(this->lineTitle)!=dicInt.end()){
	//qDebug()<<"find "<<this->lineTitle.c_str();
	// more than max, delete the early data
	while (dataInt.size() > 31) {
	    dataInt.removeFirst();
	}
	// hide
	int max=0;
	if(!data.empty()){
	    max=data.at(0);
	}
	if(isVisible()){
	    lineSeries->clear();
	    scatterSeries->clear();
	    int dx = 30 / (31-1);
	    int less = 31 - dataInt.size();
	    for (int i = 0; i < dataInt.size(); ++i) {
		if(dataInt.at(i)>max){
		    max=dataInt.at(i);
		}
		lineSeries->append(less*dx+i*dx, dataInt.at(i));
		scatterSeries->append(less*dx+i*dx, dataInt.at(i));
	    }
	}
	if(100<max){
	    chartline->axisY()->setRange(0, (max+2));
	}
	else{
	    chartline->axisY()->setRange(0, 100);
	}
    }
    else{
	if(!dic.empty() || !dicInt.empty()){
	    qDebug()<<"The title of the line chart looks like not exist, please check it";
	}
    }

    if(dic.find(this->barTitle)!=dic.end()){
	//qDebug()<<"find "<<this->barTitle.c_str();
	if(isVisible()){
	    set0->replace(0,sig);
	    set1->replace(0,100.0-sig);
	    //qDebug()<<"sig : "<<sig;
	}
    }
    else if(dicInt.find(this->barTitle)!=dicInt.end()){
	//qDebug()<<"find "<<this->barTitle.c_str();
	if(isVisible()){
	    set0->replace(0,sigInt);
	    set1->replace(0,100.0-sigInt);
	    //qDebug()<<"sigInt : "<<sigInt;
	}
    }
    else{
	if(!dic.empty() || !dicInt.empty()){
	    qDebug()<<"The title of the bar chart looks like not exist, please check it";
	}
    }
}
void Chart::timeToDate(struct timeval tv,char *time){
    struct tm tv_temp;
    localtime_r(&tv.tv_sec,&tv_temp);
    snprintf(time,25,"%04d-%02d-%02d %02d:%02d:%02d.%03ld",tv_temp.tm_year+1900,tv_temp.tm_mon+1,tv_temp.tm_mday,tv_temp.tm_hour,tv_temp.tm_min,tv_temp.tm_sec,tv.tv_usec/1000);
}
void Chart::dataSave(int n,std::string comm){
    char ip_num[20] = "";
    get_local_ip("eth0",ip_num);    //local network name, such as 'eth0', 'enpXsX' and so on
    struct timeval nowTus;
    gettimeofday(&nowTus,NULL);
    char time[25]="";
    timeToDate(nowTus,time);
    host temp;
    temp.id=to_string(host_data.size()+1);
    temp.time=time;
    temp.ip=ip_num;
    temp.pid=to_string(getpid());
    temp.type="int";
    temp.value=to_string(n);
    temp.comments=comm;
    lock_s.lock();
    dicInt.insert(comm);
    if(this->lineTitle==comm){
	dataInt<<n;
    }
    if(this->barTitle==comm){
	sigInt=n;
    }
    host_data.push_back(temp);       //save data to "database", vector<host>
    lock_s.unlock();
}
void Chart::dataSave(float n,std::string comm){
    char ip_num[20] = "";
    get_local_ip("eth0",ip_num);    //local network name, such as 'eth0', 'enpXsX' and so on
    struct timeval nowTus;
    gettimeofday(&nowTus,NULL);
    char time[25]="";
    timeToDate(nowTus,time);
    host temp;
    temp.id=to_string(host_data.size()+1);
    temp.time=time;
    temp.ip=ip_num;
    temp.pid=to_string(getpid());
    temp.type="float";
    temp.value=to_string(n);
    temp.comments=comm;
    lock_s.lock();
    dic.insert(comm);
    if(this->lineTitle==comm){
	data<<n;
    }
    if(this->barTitle==comm){
	sig=n;
    }
    host_data.push_back(temp);
    lock_s.unlock();
}
void Chart::getData(int interval){
    connect(&timerUpdate, SIGNAL(timeout()), this, SLOT(update()));
    timerUpdate.start(interval * 1000);
}
