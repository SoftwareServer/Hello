/*
* API : getData(int), param is interval to fresh the table, in this method, SLOT updata() proceesing the data that can assign to class's variables
*/
#include<QApplication>
#include"table.cpp"

int test(int argc,char *argv[]){
    QApplication app(argc,argv);

    Table view;
    view.resize(750,950);
    //resize column width for 'id', 'time', 'pid', 'type' to adapt context
    view.setColumnWidth(0,60);
    view.setColumnWidth(1,180);
    view.setColumnWidth(3,80);
    view.setColumnWidth(4,60);
    view.show();
    view.setEditTriggers(QAbstractItemView::NoEditTriggers);
    view.getData(2);
    return app.exec();
}
