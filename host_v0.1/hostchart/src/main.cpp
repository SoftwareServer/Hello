/* user main()
* API : init_dic, int-string(key-value) to describe the defination of data
* API : getData(int), param is interval to fresh the chart, in this method, SLOT update() processing the data that can assign to class's variables
*/
#include<QApplication>
#include"chart.cpp"

int main(int argc, char *argv[])
{
    QApplication app(argc,argv);
    init_dic();
    Chart chart;
    chart.getData(2);
    chart.resize(700, 400);
    chart.show();
    return app.exec();
}
