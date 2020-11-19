#ifndef INPUTSEEDUI_H
#define INPUTSEEDUI_H

#include <QWidget>
#include <QPushButton>
#include "basejsonconnector.h"

namespace Ui {
class InputSeedUi;
}

class InputSeedUi : public JsonConnectorWidget
{
    Q_OBJECT

public:
    explicit InputSeedUi(QWidget *parent = 0, int Tab = 0, QWidget *seedUi = 0);
    ~InputSeedUi();

    void Init();

    void SetSeedCodeShow(const char* lpcSeedCode);

    virtual void requestFinished(const QVariant &result, const QString &error);

    virtual bool eventFilter(QObject *target, QEvent *event);

signals:
    void on_PrevBtn();

private slots:
    void on_SeedPrevBtn_clicked();

    void on_SeedCreateWalletBtn_clicked();

    void on_SeedItemBtn_clicked();

    void on_SeedClearBtn_clicked();

private:
    Ui::InputSeedUi *ui;
    QString         m_strSeed;
    int             m_nTab;
    QList<QPushButton*>      m_listBtn;

    QString         m_strSeedCode;
    int             m_nCount;   // 记录创建地址的个数

    QWidget *m_seedUi;
};

#endif // INPUTSEEDUI_H
