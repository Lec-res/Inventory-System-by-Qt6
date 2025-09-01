#include "updatechecker.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QFile>
#include <QMessageBox>

// 【重要】请将这里的用户名和仓库名替换为您自己的
const QString GITHUB_USER = "Lec-res";
const QString GITHUB_REPO = "Inventory-system-by-Qt6";



// 在代码中定义当前版本
const QString CURRENT_VERSION = "v3.1.0";


UpdateChecker::UpdateChecker(QObject *parent) : QObject(parent)
{
    networkManager = new QNetworkAccessManager(this);
}

void UpdateChecker::checkForUpdates()
{
    QUrl url("https://api.github.com/repos/" + GITHUB_USER + "/" + GITHUB_REPO + "/releases/latest");
    QNetworkRequest request(url);
    networkManager->get(request);
    connect(networkManager, &QNetworkAccessManager::finished, this, &UpdateChecker::onCheckFinished);
}

void UpdateChecker::onCheckFinished(QNetworkReply *reply)
{
    disconnect(networkManager, &QNetworkAccessManager::finished, this, &UpdateChecker::onCheckFinished);
    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred("网络错误：" + reply->errorString());
        reply->deleteLater();
        return;
    }
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QJsonObject releaseInfo = doc.object();
    QString latestVersion = releaseInfo["tag_name"].toString();
    if (latestVersion.isEmpty()) {
        emit errorOccurred("无法解析版本信息，请检查GitHub仓库和Release设置。");
        reply->deleteLater();
        return;
    }

    // 简单的版本号比较
    if (latestVersion.compare(CURRENT_VERSION) > 0) {
        QString releaseNotes = releaseInfo["body"].toString();
        QJsonArray assets = releaseInfo["assets"].toArray();
        QUrl downloadUrl;

        // 【修改】根据版本号动态构建文件名
        QString expectedAssetName = QString("Inventory-System-%1.zip").arg(latestVersion);

        for (const QJsonValue& asset : assets) {
            QString assetName = asset.toObject()["name"].toString();
            if (assetName == expectedAssetName) {
                downloadUrl = QUrl(asset.toObject()["browser_download_url"].toString());
                break;
            }
        }

        if (downloadUrl.isValid()) {
            emit updateAvailable(latestVersion, releaseNotes, downloadUrl);
        } else {
            emit errorOccurred("找到新版本，但未找到指定的下载文件：" + expectedAssetName);
        }
    } else {
        emit noUpdateAvailable();
    }
    reply->deleteLater();
}

void UpdateChecker::startDownload(const QUrl &url)
{
    if (currentDownloadReply) {
        return; // 已经在下载中
    }
    QNetworkRequest request(url);
    currentDownloadReply = networkManager->get(request);
    connect(currentDownloadReply, &QNetworkReply::downloadProgress, this, &UpdateChecker::downloadProgress);
    connect(currentDownloadReply, &QNetworkReply::finished, this, &UpdateChecker::onDownloadFinished);
}

void UpdateChecker::onDownloadFinished()
{
    if (currentDownloadReply->error() != QNetworkReply::NoError) {
        emit errorOccurred("下载失败: " + currentDownloadReply->errorString());
    } else {
        QString downloadDir = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);

        // 【修改】从回复的URL中获取实际文件名，或者使用动态生成的名称
        QString fileName = currentDownloadReply->url().fileName();
        if (fileName.isEmpty()) {
            // 如果无法从URL获取文件名，则使用默认格式
            fileName = QString("Inventory-system-latest.zip");
        }

        QString savePath = downloadDir + "/" + fileName;
        QFile file(savePath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(currentDownloadReply->readAll());
            file.close();
            emit downloadFinished(savePath);
        } else {
            emit errorOccurred("无法保存文件到目录: " + downloadDir);
        }
    }
    currentDownloadReply->deleteLater();
    currentDownloadReply = nullptr;
}
