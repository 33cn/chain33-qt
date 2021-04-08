#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

/*
 * 关于界面
 */
#include "basejsonconnector.h"
#include <QDialog>
#include <QTextStream>
#include <QSettings>

namespace Ui {
    class AboutDialog;
}

class AboutDialog : public JsonConnectorDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

protected:
    virtual void requestFinished(const QVariant &result, const QString &error);

private:
    void initAboutUi();
    void readAboutConfigFile();
    void readValue(QSettings *lpconfig, const QString &key, QString &ret);

private slots:
    void on_buttonBox_accepted();

private:
    Ui::AboutDialog *ui;
    QString m_strIntro;
    QString m_strFrameName;
    QString m_strFrameUrl;
    QString m_strWebsite;
    QString m_strDocumentUrl;
    QString m_strBrowserUrl;
};

#endif // ABOUTDIALOG_H
