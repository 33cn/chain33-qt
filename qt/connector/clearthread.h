#ifndef CLEARTHREAD_H
#define CLEARTHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

class ClearThread :public QThread
{
public:
#if QT_VERSION >= 0x050000
    ClearThread(QObject *parent = Q_NULLPTR);
#else
    ClearThread(QObject *parent = NULL);
#endif
    ~ClearThread();
    void Stop();
    void Resume();
protected:
    virtual void run();
private:
    QMutex m_mutex;
    QWaitCondition m_cond;
    bool m_quit;
};

#endif // CLEARTHREAD_H
