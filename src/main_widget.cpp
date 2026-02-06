#include "main_widget.h"
#include <QtWidgets>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>


// A real application should not hard code the account id and license key like this.
// Should instead read them from configuration or ask the user in a dialog.
constexpr auto g_accountId = "1fddcec8-8dd3-4d8d-9b16-215cac0f9b52";
constexpr auto g_licenseKey = "3EF6D1-CBB672-2A94EB-04EA73-02B873-V3";


static void _AfterLicenseValidationSuccess(MainWidget *pMainWidget) {
  // Remove all children from pMainWidget.
  {
    QWidget dummyWidget;
    dummyWidget.setLayout(pMainWidget->layout());
  }

  QVBoxLayout *mainLayout = new QVBoxLayout();
  mainLayout->addWidget(new QPushButton("Do some work"));
  pMainWidget->setLayout(mainLayout);
}


static void _AfterLicenseValidationFailure(MainWidget *pMainWidget, QString title, QString detail) {
  // Remove all children from pMainWidget.
  {
    QWidget dummyWidget;
    dummyWidget.setLayout(pMainWidget->layout());
  }

  QVBoxLayout *mainLayout = new QVBoxLayout();
  mainLayout->addWidget(new QLabel(
    QString("License validation failed!\n\nTitle: %1\n\nDetail: %2").arg(title, detail)));
  pMainWidget->setLayout(mainLayout);
}


static void _ValidateLicense(MainWidget *pMainWidget, QVBoxLayout *pLayout) {
  auto manager = new QNetworkAccessManager(pMainWidget);

  QObject::connect(
    manager,
    &QNetworkAccessManager::finished,
    [pMainWidget](QNetworkReply *reply) {
      if (reply->error() == QNetworkReply::NoError)
      {
          QString answer = reply->readAll();
          QJsonDocument jsonResponse = QJsonDocument::fromJson(answer.toUtf8());

          if (jsonResponse.object()["meta"].toObject()["valid"].toBool()) {
            _AfterLicenseValidationSuccess(pMainWidget);
          } else {
            QString title, detail;

            if (jsonResponse.object()["errors"].toArray().count() > 0) {
              auto err = jsonResponse.object()["errors"].toArray()[0].toObject();
              title = err["title"].toString();
              detail = err["detail"].toString();
            }

            _AfterLicenseValidationFailure(pMainWidget, title, detail);
          }
      }
      else
      {
          qDebug() << "网络错误,如果提示TLS错误说明没有复制openSSL相关的dll到输出目录:" << reply->errorString();
      }
    }
  );

  // 1. 使用QJsonDocument安全地构造JSON数据
  QJsonObject metaObject;
  metaObject["key"] = g_licenseKey; // 假设g_licenseKey是QString

  QJsonObject rootObject;
  rootObject["meta"] = metaObject;

  QJsonDocument jsonDoc(rootObject);
  // 使用Compact模式去除不必要的空格，更节省流量
  QByteArray postBytes = jsonDoc.toJson(QJsonDocument::Compact);


  //QString postString = QString("{\"meta\":{\"key\":\"%1\"}}").arg(g_licenseKey);
 // QByteArray postBytes = postString.toUtf8();
  QNetworkRequest request = QNetworkRequest(QUrl(
    QString("https://api.keygen.sh/v1/accounts/%1/licenses/actions/validate-key").arg(g_accountId)));

  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/vnd.api+json");
  request.setRawHeader("Accept", "application/vnd.api+json");

  // Returns immediately, does not wait for the reply.
  manager->post(request, postBytes);
}


MainWidget::MainWidget() {

    // 当提示 qt.network.ssl: QSslSocket::connectToHostEncrypted: TLS initialization failed 时 ，需要确定qt构建时所用的ssl版本，然后下载二进制复制到输出目录即可，或者自行编译对应版本
    // 用用于确定 ssl的版本
    qDebug() << "SSL 库构建版本:" << QSslSocket::sslLibraryBuildVersionString();
    qDebug() << "当前环境 SSL 支持情况:" << QSslSocket::supportsSsl();


  QVBoxLayout *mainLayout = new QVBoxLayout();
  mainLayout->addWidget(new QLabel("Validating license..."));

  setLayout(mainLayout);
  setWindowTitle("Keygen example");
  setMinimumSize(400, 200);

  // Async function, returns immediately.
  _ValidateLicense(this, mainLayout);
}
