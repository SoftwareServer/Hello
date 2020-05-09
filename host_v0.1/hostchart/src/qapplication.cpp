/*
* API : init_dic, int-string(key-value) to describe the defination of data
* API : getData(int), param is interval to fresh the chart, in this method, SLOT update() processing the data that can assign to class's variables
*/
#include<QApplication>
#include"chart.cpp"

int qapp(int argc, char *argv[]){
    QApplication app(argc,argv);
    Chart chart(nullptr,"error rate","signal");//The line chart title is different from the comment if the chart has no points
    chart.getData(2);
    chart.resize(700, 400);
    chart.show();
    return app.exec();
}
