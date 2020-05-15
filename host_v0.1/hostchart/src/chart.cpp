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
    layout->setContentsMargins(2, 2, 2, 2);

    layout->addWidget(chartViewline);
    layout->addWidget(chartViewbar);
    layout->setStretchFactor(chartViewline,4);
    layout->setStretchFactor(chartViewbar,1);

    btn1=new QPushButton("Reset");
    btn2=new QPushButton("Pause");
    btn3=new QPushButton("Save");
    btn4=new QPushButton("Close");
    vlayout = new QHBoxLayout();
    vlayout->addWidget(btn1);
    vlayout->addWidget(btn2);
    vlayout->addWidget(btn3);
    vlayout->addWidget(btn4);

    QWidget *pCentralWidget = new QWidget(this);

    baseLayout = new QGridLayout(pCentralWidget);
    //baseLayout->addLayout(layout, 0, 0, 0, 0);
    //baseLayout->addLayout(vlayout, 0, 0, 0, 0);
    baseLayout->addLayout(vlayout, 0, 0);
    baseLayout->addLayout(layout, 1, 0);
    //baseLayout->addLayout(vlayout, 1, 0);

    setCentralWidget(pCentralWidget);

    connect(&timer, SIGNAL(timeout()), this, SLOT(result()));
    connect(btn1,SIGNAL(clicked()), this, SLOT(onBtnClicked1()));
    connect(btn2,SIGNAL(clicked()), this, SLOT(onBtnClicked2()));
    connect(btn3,SIGNAL(clicked()), this, SLOT(onBtnClicked3()));
    connect(btn4,SIGNAL(clicked()), this, SLOT(onBtnClicked4()));
    timer.start(2 * 1000);
    a=0;
    b=1.1;
    barData=0.0;
    barDataInt=0.0;
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
    if(NULL!=vlayout){
	delete vlayout;
	vlayout=NULL;
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
    if(NULL!=btn1){
	delete btn1;
	btn1=NULL;
    }
    if(NULL!=btn2){
	delete btn2;
	btn2=NULL;
    }
    if(NULL!=btn3){
	delete btn3;
	btn3=NULL;
    }
    if(NULL!=btn4){
	delete btn4;
	btn4=NULL;
    }
}
void Chart::update(){
    dataSave(++a,"robot num");
    dataSave(rand()%100,"packets");
    dataSave(rand()%1000,"data length");
    dataSave(++b,"speed");
    dataSave((float)(rand()%100+0.1),"signal","percent");
    dataSave((float)(rand()%100+0.3),"error rate");
    //qDebug()<<host_data.size();
}
void Chart::result(){
    if(dic.find(this->lineTitle)!=dic.end()){
	//qDebug()<<"find "<<this->lineTitle.c_str();
	// more than max, delete the early data
	while (lineData.size() > 31) {
	    lineData.removeFirst();
	}
	// hide
	float max=0;
	if(!lineData.empty()){
	    max=lineData.at(0);
	}
	if(isVisible()){
	    lineSeries->clear();
	    scatterSeries->clear();
	    int dx = 30 / (31-1);
	    int less = 31 - lineData.size();
	    for (int i = 0; i < lineData.size(); ++i) {
		if(lineData.at(i)>max){
		    max=lineData.at(i);
		}
		lineSeries->append(less*dx+i*dx, lineData.at(i));
		scatterSeries->append(less*dx+i*dx, lineData.at(i));
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
	while (lineDataInt.size() > 31) {
	    lineDataInt.removeFirst();
	}
	// hide
	int max=0;
	if(!lineDataInt.empty()){
	    max=lineDataInt.at(0);
	}
	if(isVisible()){
	    lineSeries->clear();
	    scatterSeries->clear();
	    int dx = 30 / (31-1);
	    int less = 31 - lineDataInt.size();
	    for (int i = 0; i < lineDataInt.size(); ++i) {
		if(lineDataInt.at(i)>max){
		    max=lineDataInt.at(i);
		}
		lineSeries->append(less*dx+i*dx, lineDataInt.at(i));
		scatterSeries->append(less*dx+i*dx, lineDataInt.at(i));
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
	    set0->replace(0,barData);
	    set1->replace(0,100.0-barData);
	    //qDebug()<<"barData : "<<barData;
	}
    }
    else if(dicInt.find(this->barTitle)!=dicInt.end()){
	//qDebug()<<"find "<<this->barTitle.c_str();
	if(isVisible()){
	    set0->replace(0,barDataInt);
	    set1->replace(0,100.0-barDataInt);
	    //qDebug()<<"barDataInt : "<<barDataInt;
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
void Chart::dataSave(int n,std::string attribute,std::string notes){
    char ip_num[20] = "";
    get_local_ip("eth0",ip_num);    //local network name, such as 'eth0', 'enpXsX' and so on
    struct timeval nowTus;
    gettimeofday(&nowTus,NULL);
    char time[25]="";
    timeToDate(nowTus,time);
    host temp;
    temp.id=to_string(host_data.size()+1);
    temp.nodeId=ip_num;
    temp.timetag=time;
    temp.attribute=attribute;
    temp.value=to_string(n);
    temp.notes=notes;
    lock_s.lock();
    dicInt.insert(attribute);
    if(this->lineTitle==attribute){
	lineDataInt<<n;
    }
    if(this->barTitle==attribute){
	barDataInt=n;
    }
    host_data.push_back(temp);       //save data to "database", vector<host>
    lock_s.unlock();
}
void Chart::dataSave(float n,std::string attribute,std::string notes){
    char ip_num[20] = "";
    get_local_ip("eth0",ip_num);    //local network name, such as 'eth0', 'enpXsX' and so on
    struct timeval nowTus;
    gettimeofday(&nowTus,NULL);
    char time[25]="";
    timeToDate(nowTus,time);
    host temp;
    temp.id=to_string(host_data.size()+1);
    temp.nodeId=ip_num;
    temp.timetag=time;
    temp.attribute=attribute;
    temp.value=to_string(n);
    temp.notes=notes;
    lock_s.lock();
    dic.insert(attribute);
    if(this->lineTitle==attribute){
	lineData<<n;
    }
    if(this->barTitle==attribute){
	barData=n;
    }
    host_data.push_back(temp);
    lock_s.unlock();
}
void Chart::getData(int interval){
    connect(&timerUpdate, SIGNAL(timeout()), this, SLOT(update()));
    timerUpdate.start(interval * 1000);
}
bool Chart::saveFile(const QString &fileName){
    if(!chartViewline){
	return false;
    }
    if(!chartViewbar){
	return false;
    }
    QPixmap map(chartViewline->width()+chartViewbar->width(),chartViewline->height());
    QPixmap pline;
    QPixmap pbar;
    pline=this->chartViewline->grab();
    pbar=this->chartViewbar->grab();
    QPainter p(&map);
    p.drawPixmap(0,0,pline);
    p.drawPixmap(pline.width(),0,pbar);
    QImage image=map.toImage();
    if(fileName.endsWith(".png")){
	image.save(fileName.mid(0,fileName.indexOf("."))+".png");
    }else{
	image.save(fileName+".png");
    }
    return true;
}
void Chart::onBtnClicked1(){
    if(!host_data.empty()){
	lock_s.lock();
	host_data.clear();
	lock_s.unlock();
    }
    if(!dic.empty()){
	lock_s.lock();
	dic.clear();
	lock_s.unlock();
    }
    if(!dicInt.empty()){
	lock_s.lock();
	dicInt.clear();
	lock_s.unlock();
    }
    if(!lineData.empty()){
	lock_s.lock();
	lineData.clear();
	lock_s.unlock();
    }
    if(!lineDataInt.empty()){
	lock_s.lock();
	lineDataInt.clear();
	lock_s.unlock();
    }
    lineSeries->clear();
    scatterSeries->clear();
    set0->replace(0,0);
    set1->replace(0,1);
    timer.start();
    if(pause){
	btn2->setText("Pause");
	pause=false;
    }
}
void Chart::onBtnClicked2(){
    if(pause){
	timer.start();
	btn2->setText("Pause");
	pause=false;
	//qDebug()<<"continue";
    }else{
	timer.stop();
	btn2->setText("Continue");
	pause=true;
	//qDebug()<<"pause";
    }
}
bool Chart::onBtnClicked3(){
    QString fileName = QFileDialog::getSaveFileName(this,
                               tr("Save Charts"), ".",
                               tr("PNG files (*.png)"));
    if (fileName.isEmpty()){
        return false;
    }
    return saveFile(fileName);
}
void Chart::onBtnClicked4(){
    this->close();
}
