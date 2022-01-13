#pragma once
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QString>
#include <QVersionNumber>
#include <QWidget>
#include "DialogWindow.hpp"
#include "UpdaterDialog.hpp"

class UpdaterGithub : public QObject
{
    Q_OBJECT
public:
    UpdaterGithub(QWidget * parent, const QString &repo_url, const QString &current_version);
    friend class UpdaterDialog;

public slots:
    void checkForUpdates(bool silent, float font_size);
    void downloadChangelog();
    void downloadUpdate();
    void installUpdate();

private slots:
    void _onUpdateCheckFinished();
    void _onNewDataDownloaded();
    void _onDownloadFinished();

private:
    float fontSize;
    QWidget* _parent;
    UpdaterDialog* _dia;
    QFile _downloadedBinaryFile;
    const QString _updatePageAddress;
    QNetworkAccessManager _networkManager;
    QNetworkReply* _reply;
    const QVersionNumber _currentVersion;
    QString _latestVersionString;
    bool _isSilent{true};
};
