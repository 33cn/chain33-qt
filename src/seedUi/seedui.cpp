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

SeedUi* g_lpSeedUi = NULL;

SeedUi::SeedUi(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SeedUi)
{
    ui->setupUi(this);
    initUI();
    g_lpSeedUi = this;
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
    setWindowTitle(tr("%1钱包-正式版 %2").arg(CStyleConfig::GetInstance().GetAppName(), g_strVersion));
    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint); // 没有帮助按钮

    QTabWidget* tabWidget = new QTabWidget(this);
    tabWidget->addTab(new SwitchCreateSeedUi(tabWidget), tr("创建钱包"));
    tabWidget->addTab(new InputSeedUi(tabWidget, InputSeed_Tab), tr("导入钱包"));
    ui->seed_tab->addWidget(tabWidget);
}
