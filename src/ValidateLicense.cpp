#include "ValidateLicense.h"

#include <QtWidgets>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>


// A real application should not hard code the account id and license key like this.
// Should instead read them from configuration or ask the user in a dialog.
// 测试用户id
constexpr auto g_accountId = "1fddcec8-8dd3-4d8d-9b16-215cac0f9b52";
//
constexpr auto g_licenseKey = "3EF6D1-CBB672-2A94EB-04EA73-02B873-V3";

CValidateLicense::CValidateLicense()
{
    m_strUserId = g_accountId;

    // 当提示 qt.network.ssl: QSslSocket::connectToHostEncrypted: TLS initialization failed 时 ，需要确定qt构建时所用的ssl版本，然后下载二进制复制到输出目录即可，或者自行编译对应版本
    // 用用于确定 ssl的版本
    qDebug() << "SSL 库构建版本:" << QSslSocket::sslLibraryBuildVersionString();
    qDebug() << "当前环境 SSL 支持情况:" << QSslSocket::supportsSsl();
}

CValidateLicense::~CValidateLicense()
{
}

void CValidateLicense::SetUserId(const QString& strUserID)
{
    m_strUserId = strUserID;
}

CValidateLicense::stuResult::stuResult()
{
    bVisable = false;
    strErrorTitle = "";   // 错误标题
    strErrorDetail = "";  // 错误详情
}

void CValidateLicense::ValidateLicense(const QString& strProductID)
{
    QString strID = strProductID;
    if (strID.isEmpty())
    {
        strID = g_licenseKey;
    }

    auto manager = new QNetworkAccessManager(this);

    QObject::connect(manager, &QNetworkAccessManager::finished, this, &CValidateLicense::acceptFun);

    // 1. 使用QJsonDocument安全地构造JSON数据
    QJsonObject metaObject;
    metaObject["key"] = strID; // 假设g_licenseKey是QString

    QJsonObject rootObject;
    rootObject["meta"] = metaObject;

    QJsonDocument jsonDoc(rootObject);
    // 使用Compact模式去除不必要的空格，更节省流量
    QByteArray postBytes = jsonDoc.toJson(QJsonDocument::Compact);


    //QString postString = QString("{\"meta\":{\"key\":\"%1\"}}").arg(g_licenseKey);
   // QByteArray postBytes = postString.toUtf8();
    QNetworkRequest request = QNetworkRequest(QUrl(
      QString("https://api.keygen.sh/v1/accounts/%1/licenses/actions/validate-key").arg(m_strUserId)));

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/vnd.api+json");
    request.setRawHeader("Accept", "application/vnd.api+json");

    // 立刻返回, 不会等待回复.
    manager->post(request, postBytes);
}
const CValidateLicense::stuResult* CValidateLicense::GetResult() const
{
    return &m_dataResult;
}

void CValidateLicense::OnEndGetResult()
{

}

void CValidateLicense::acceptFun(QNetworkReply *reply)
{
    if (nullptr == reply)
        return;

    if (reply->error() == QNetworkReply::NoError)
    {
        QString answer = reply->readAll();
        QJsonDocument& jsonResponse = m_dataResult.json;
        jsonResponse = QJsonDocument::fromJson(answer.toUtf8());



        if (jsonResponse.object()["meta"].toObject()["valid"].toBool())
        {
            m_dataResult.bVisable = true;
        }
        else
        {
          QString title, detail;

          if (jsonResponse.object()["errors"].toArray().count() > 0) {
            auto err = jsonResponse.object()["errors"].toArray()[0].toObject();
            title = err["title"].toString();
            detail = err["detail"].toString();
          }

          m_dataResult.strErrorTitle = title;
          m_dataResult.strErrorDetail = detail;
          m_dataResult.bVisable = false;
        }
    }
    else
    {
        qDebug() << "网络错误,如果提示TLS错误说明没有复制openSSL相关的dll到输出目录:" << reply->errorString();
    }

    OnEndGetResult();
}
