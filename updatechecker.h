#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>

class UpdateChecker : public QObject
{
    Q_OBJECT
public:
    explicit UpdateChecker(QObject *parent = nullptr);

public slots:
    void checkForUpdates();
    void startDownload(const QUrl& url);

signals:
    void updateAvailable(const QString& version, const QString& releaseNotes, const QUrl& downloadUrl);
    void noUpdateAvailable();
    void errorOccurred(const QString& error);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished(const QString& savedPath);

private slots:
    void onCheckFinished(QNetworkReply* reply);
    void onDownloadFinished();

private:
    QNetworkAccessManager* networkManager;
    QNetworkReply* currentDownloadReply = nullptr;
};

#endif // UPDATECHECKER_H
