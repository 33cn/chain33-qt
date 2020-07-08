#ifndef CREATESEEDUI_H
#define CREATESEEDUI_H

#include <QWidget>
#include <QList>
#include <QLabel>
#include <QHBoxLayout>
#include "basejsonconnector.h"

namespace Ui {
class CreateSeedUi;
}

class CreateSeedUi : public JsonConnectorWidget
{
    Q_OBJECT

public:
    explicit CreateSeedUi(QWidget *parent = 0);
    ~CreateSeedUi();

    virtual void requestFinished(const QVariant &result, const QString &error);

signals:
    void on_NextBtn(const char *lpcSeedCode);

private slots:
    void on_UpDataBtn_clicked();

    void on_SeedNextBtn_clicked();

private:
    void AddLabelList(QHBoxLayout* lpHBoxLayout);

private:
    Ui::CreateSeedUi *ui;
    QString             m_strSeedCode;
    QList<QLabel*>      m_listLabel;
    int                 m_nLang;
//    QStringList         m_strListSeedCode;
};

#endif // CREATESEEDUI_H
