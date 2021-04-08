#include "seedui.h"
#include "ui_seedui.h"
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include "createseedui.h"
#include "inputseedui.h"
#include "switchcreateseedui.h"
#include "enumtype.h"
#include "mainui.h"
#include "basefuntion.h"

SeedUi::SeedUi(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SeedUi)
{
    ui->setupUi(this);
    initUI();
}

SeedUi::~SeedUi()
{
    delete ui;
}

void SeedUi::initUI()
{
#ifdef Q_OS_MAC
    resize(900, 495);
#else
    resize(920, 552);
#endif
    setWindowTitle(tr("%1钱包").arg(CStyleConfig::GetInstance().GetAppName()));
    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint); // 没有帮助按钮

    QTabWidget* tabWidget = new QTabWidget(this);
    tabWidget->addTab(new SwitchCreateSeedUi(tabWidget, this), tr("创建钱包"));
    tabWidget->addTab(new InputSeedUi(tabWidget, InputSeed_Tab, this), tr("导入钱包"));
    ui->seed_tab->addWidget(tabWidget);
}
