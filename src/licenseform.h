#ifndef WIDGET1_H
#define WIDGET1_H

#include <QWidget>
#include "ValidateLicense.h"
QT_BEGIN_NAMESPACE
namespace Ui { class licenseform; }
QT_END_NAMESPACE

class licenseform : public QWidget
{
    Q_OBJECT

public:
    explicit licenseform(QWidget *parent = nullptr);
    ~licenseform();

private slots:
    void on_buttonValidationLicense_clicked();
    void onEndValidateLicense();

private:
    Ui::licenseform *ui;

    CValidateLicense m_validateLicense;
};

#endif // WIDGET1_H
