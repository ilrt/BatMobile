#ifndef PROCESSINGTHREAD_H
#define PROCESSINGTHREAD_H

#include <QThread>
#include <QObject>

class MyThread : public QObject, QThread
 {
    Q_OBJECT
 public:
     void run();
 signals:
     void percentageComplete(const int&);
 };

 void MyThread::run()
 {
    while(data.hasMoreItems())
    {
      doSomeProcessing(data.nextItem())
      emit( percentageComplete(10) );
    }
 }

 #endif // PROCESSINGTHREAD_H
