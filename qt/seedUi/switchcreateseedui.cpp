#include "switchcreateseedui.h"
#include "ui_switchcreateseedui.h"
#include "createseedui.h"
#include "inputseedui.h"

SwitchCreateSeedUi::SwitchCreateSeedUi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SwitchCreateSeedUi)
{
    ui->setupUi(this);

    m_lpcentralWidget = new QStackedWidget(this);

    m_lpCreateSeedUi = new CreateSeedUi(m_lpcentralWidget);
    m_lpInputSeedUi = new InputSeedUi(m_lpcentralWidget);

    m_lpcentralWidget->addWidget(m_lpCreateSeedUi);
    m_lpcentralWidget->addWidget(m_lpInputSeedUi);
    m_lpcentralWidget->setCurrentWidget(m_lpCreateSeedUi);

    connect(m_lpCreateSeedUi, SIGNAL(on_NextBtn(const char*)), this, SLOT(on_NextBtn(const char*)));
    connect(m_lpInputSeedUi, SIGNAL(on_PrevBtn()), SLOT(on_PrevBtn()));
}

SwitchCreateSeedUi::~SwitchCreateSeedUi()
{
    delete ui;
}

void SwitchCreateSeedUi::on_NextBtn(const char* lpcSeedCode)
{
    m_lpcentralWidget->setCurrentWidget(m_lpInputSeedUi);
    m_lpInputSeedUi->SetSeedCodeShow(lpcSeedCode);
}

void SwitchCreateSeedUi::on_PrevBtn()
{
    m_lpcentralWidget->setCurrentWidget(m_lpCreateSeedUi);
}
