#ifndef SWITCHCREATESEEDUI_H
#define SWITCHCREATESEEDUI_H

#include <QWidget>
#include <QStackedWidget>
class CreateSeedUi;
class InputSeedUi;

namespace Ui {
class SwitchCreateSeedUi;
}

class SwitchCreateSeedUi : public QWidget
{
    Q_OBJECT

public:
    explicit SwitchCreateSeedUi(QWidget *parent = 0, QWidget *seedUi = 0);
    ~SwitchCreateSeedUi();    

public slots:
    void on_NextBtn(const char* lpcSeedCode);
    void on_PrevBtn();

private:
    Ui::SwitchCreateSeedUi *ui;
    QStackedWidget *m_lpcentralWidget;

    CreateSeedUi* m_lpCreateSeedUi;
    InputSeedUi* m_lpInputSeedUi;
};

#endif // SWITCHCREATESEEDUI_H
