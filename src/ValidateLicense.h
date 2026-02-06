#pragma once
class QNetworkReply;
#include <QObject>
#include <QString>
#include <QJsonDocument>
class CValidateLicense : public QObject
{
    Q_OBJECT
public:
    CValidateLicense();
    ~CValidateLicense();

    ///
    /// \brief SetUserId 设置用户id （setting中的id）
    /// \param strUserID 传入用户id
    ///
    void SetUserId(const QString& strUserID);

    ///
    /// \brief 验证结果
    ///
    struct stuResult
    {
        bool bVisable;           // 是否有效
        QString strErrorTitle;   // 错误标题
        QString strErrorDetail;  // 错误详情

        QJsonDocument json;      // 返回的json结果

        stuResult();
    };


    ///
    /// \brief 验证产品id是否有效
    /// \param strProductID 传入产品id
    ///
    void ValidateLicense(const QString& strProductID = "");

    ///
    /// \brief 返回值
    /// \return 返回结果指针
    ///
    const stuResult* GetResult() const;
    ///
    /// \brief 结束接收
    ///
    void OnEndGetResult();

public slots:
    ///
    /// \brief 接受 从网络返回的参数
    /// \param reply 传入网络返回值
    ///
    void acceptFun(QNetworkReply *reply);
private:
    QString m_strUserId;            // 用户id

    stuResult m_dataResult;         // 验证后的结果
};
