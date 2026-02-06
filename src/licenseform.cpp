#include "licenseform.h"
#include "./ui_licenseform.h"
licenseform::licenseform(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::licenseform)
{
    ui->setupUi(this);

    QObject::connect(&m_validateLicense, &CValidateLicense::OnEndGetResult, this, &licenseform::onEndValidateLicense);
}

licenseform::~licenseform()
{
    delete ui;
}

void licenseform::on_buttonValidationLicense_clicked()
{
    m_validateLicense.ValidateLicense();
}

void licenseform::onEndValidateLicense()
{
    if (m_validateLicense.GetResult()->bVisable)
    {
        ui->textEditResult->setText("验证成功");
    }
    else
    {
        ui->textEditResult->setText("验证失败");
    }
}

