#ifndef SEEDUI_H
#define SEEDUI_H

#include <QDialog>

namespace Ui {
class SeedUi;
}

class SeedUi : public QDialog
{
    Q_OBJECT

public:
    explicit SeedUi(QWidget *parent = 0);

    ~SeedUi();

    void initUI();

private:
    Ui::SeedUi *ui;
};

#endif // SEEDUI_H
