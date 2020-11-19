#include "createseedui.h"
#include "ui_createseedui.h"
#include <QLabel>
#include <QSpacerItem>
#include "basefuntion.h"

CreateSeedUi::CreateSeedUi(QWidget *parent)
    : JsonConnectorWidget(parent)
    , ui(new Ui::CreateSeedUi)
    , m_nLang(1)
{
    ui->setupUi(this);

    ui->label_2->setWordWrap(true);
    ui->label_2->setAlignment(Qt::AlignTop);
    ui->label_3->setWordWrap(true);
    ui->label_3->setAlignment(Qt::AlignTop);

    ui->WarnLabel->setStyleSheet("QLabel{background: transparent; color: #ec5151;}");

    QString lang_territory = QString::fromStdString(QLocale::system().name().toStdString());
    if(lang_territory == "en") {
        m_nLang = 0;
    }

    if(m_nLang == 1) {
        QHBoxLayout* lpHBoxLayout = new QHBoxLayout(this);
        lpHBoxLayout->setSpacing(1);
        for(int i=0; i<15; ++i) {
            AddLabelList(lpHBoxLayout);

            if(m_nLang == 1 && i > 0 && (i+1)%3 == 0) {
                QSpacerItem* lpSpacerItem = new QSpacerItem(40,20,QSizePolicy::Expanding);
                lpHBoxLayout->addItem(lpSpacerItem);
            }
        }
        QSpacerItem* lpSpacerItem = new QSpacerItem(20,20,QSizePolicy::Expanding);
        lpHBoxLayout->addItem(lpSpacerItem);
        ui->SeedCodeLayout->addItem(lpHBoxLayout);
    } else {
        for(int j=0; j<2; ++j) {
            QHBoxLayout* lpHBoxLayout = new QHBoxLayout(this);
            lpHBoxLayout->setSpacing(4);
            for(int i=0; i<8-j; ++i) {
                AddLabelList(lpHBoxLayout);
            }
            QSpacerItem* lpSpacerItem = new QSpacerItem(20,20,QSizePolicy::Expanding);
            lpHBoxLayout->addItem(lpSpacerItem);
            ui->SeedCodeLayout->addItem(lpHBoxLayout);
        }
    }

    on_UpDataBtn_clicked();
}

CreateSeedUi::~CreateSeedUi()
{
    delete ui;
}

void CreateSeedUi::requestFinished(const QVariant &result, const QString &/*error*/)
{
    if(ID_GenSeed == m_nID) {
        QMap<QString, QVariant> resultMap = result.toMap();
        m_strSeedCode = resultMap["seed"].toString();
        QStringList strListSeedCode = m_strSeedCode.split(" ");

        if(strListSeedCode.size() >= 15) {
            for(int i=0; i<15; ++i) {
                m_listLabel[i]->setText(strListSeedCode[i]);
            }
        }
    }
}

void CreateSeedUi::on_UpDataBtn_clicked()
{
    QJsonObject jsonParms;
    jsonParms.insert("lang", m_nLang);
    QJsonArray params;
    params.insert(0, jsonParms);
    PostJsonMessage(ID_GenSeed, params);
}

void CreateSeedUi::AddLabelList(QHBoxLayout* lpHBoxLayout)
{
    QLabel* lpSeedLabel = new QLabel(this);
    m_listLabel.push_back(lpSeedLabel);
    lpSeedLabel->setText(" ");
    lpSeedLabel->setObjectName("SeedLabel");

    lpSeedLabel->setStyleSheet("QLabel { font: " + QString::number(GetBaseFontSize() + 2) + "pt; color: #ffba26; border-radius: 4px; padding-left: 8px; padding-right: 8px; padding-top: 13px;padding-bottom: 13px; background: #202020; } ");
    if(CStyleConfig::GetInstance().GetStyleType() == QSS_BLUE)
        lpSeedLabel->setStyleSheet("QLabel { font: " + QString::number(GetBaseFontSize() + 2) + "pt; color: #5282DB; border-radius: 4px; padding-left: 8px; padding-right: 8px; padding-top: 13px;padding-bottom: 13px; background: #ffffff; } ");

    lpHBoxLayout->addWidget(lpSeedLabel);
}

void CreateSeedUi::on_SeedNextBtn_clicked()
{
    emit on_NextBtn(m_strSeedCode.toStdString().c_str());
}
