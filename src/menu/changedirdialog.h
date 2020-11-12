#ifndef CHANGEDIRDIALOG_H
#define CHANGEDIRDIALOG_H

/*
 * 更改数据目录
 */

#include <QDialog>
#include <QThread>
#include <QMutex>
#include "basejsonconnector.h"

class CopyThread : public QThread
{
    Q_OBJECT
public:
    CopyThread();
    ~CopyThread(){}

    void SetDataDir(const QString& strDir);

signals:
    void FinishCopy();

protected:
    virtual void run();

private:
    QString m_dataDirStr;
    QMutex  m_mutex;
};

namespace Ui {
class ChangeDirDialog;
}

class ChangeDirDialog : public JsonConnectorDialog
{
    Q_OBJECT

public:
    explicit ChangeDirDialog(QWidget *parent = 0);
    ~ChangeDirDialog();

    void accept();

protected:
    virtual void requestFinished(const QVariant &result, const QString &error);

private slots:
    void on_ellipsisButton_clicked();

    void FinishCopy();

    void on_pushButton_Finish_clicked();

private:
    Ui::ChangeDirDialog *ui;
    CopyThread*         m_lpCopyThread;
};

#endif // CHANGEDIRDIALOG_H
