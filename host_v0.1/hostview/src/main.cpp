#include<QApplication>
#include<boost/thread/thread.hpp>
#include<boost/bind.hpp>
#include"table.cpp"

int test(int argc,char *argv[]){
    QApplication app(argc,argv);

    Table view;
    view.resize(650,950);
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
int main(int argc, char *argv[])
{
    boost::thread_group thrd_grp;
    thrd_grp.create_thread(boost::bind(*test,argc,argv));
    thrd_grp.join_all();

    return 0;
}
