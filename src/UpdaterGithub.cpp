#include <assert.h>
#include <QCoreApplication>
#include <QProcess>
#include "UpdaterGithub.hpp"
#include "file_helpers.hpp"
#include "logger.hpp"

UpdaterGithub::UpdaterGithub(QWidget* parent, const QString &repo_url, const QString &current_version) :
    _parent(parent),
    _dia(nullptr),
	_updatePageAddress(repo_url + "/releases/%1"),
	_currentVersion(QVersionNumber::fromString(current_version)) {}

void UpdaterGithub::checkForUpdates(bool silent)
{
    _isSilent = silent;
    
    auto req = QNetworkRequest(QUrl(_updatePageAddress.arg("latest")));
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::SameOriginRedirectPolicy);
    req.setSslConfiguration(QSslConfiguration::defaultConfiguration()); // HTTPS
    this->_reply = _networkManager.get(req);
	if (!this->_reply)
	{
        LOG_ENTRY(ERROR, "Network request rejected due to: " << this->_reply->errorString());
        return;
	}

    connect(this->_reply, _reply->finished, this, _onUpdateCheckFinished,
            Qt::UniqueConnection);
}

void UpdaterGithub::_onUpdateCheckFinished()
{
    this->_reply->deleteLater();

    if (this->_reply->error() != QNetworkReply::NoError)
    {
        LOG_ENTRY(ERROR, "QNetwork returned errror " << this->_reply->errorString());
        if (_isSilent)
            return;
        auto dia = new DialogWindow(this->_parent, "Update",
                                    "Error while searching for updates", Status::ERROR0);
        dia->show();
        return;
    }

    if (this->_reply->bytesAvailable() <= 0)
    {
        LOG_ENTRY(ERROR, "No data downloaded");
        if (_isSilent)
            return;
        auto dia = new DialogWindow(this->_parent, "Update",
                                    "Error while searching for updates", Status::ERROR0);
        dia->show();
        return;
    }

    _latestVersionString = this->_reply->url().url().split("/", QString::SkipEmptyParts).last();
    if (_latestVersionString.contains("major"))
    {
        auto dia = new DialogWindow(this->_parent, "Update",
                                    "There's an update available, but i'ts major release, please re-download the entire folder for the application again", Status::WARNING);
        dia->show();
        return;
    }

    if (QVersionNumber::fromString(_latestVersionString) <= _currentVersion)
    {
        if (_isSilent)
            return;
        auto dia = new DialogWindow(this->_parent, "Update",
                                    "Application is up-to-date", Status::SUCCESS);
        dia->show();
        return;
    }

    const QString patternL = QStringLiteral("<div class=\"markdown-body\">\n");
    const QString patternR = QStringLiteral("<h3>IMPORTANT NOTE:");

    const auto text = QString(this->_reply->readAll());

    int left = text.indexOf(patternL);
    _dia = new UpdaterDialog(this->_parent, this);
    if (left == -1)
    {
        LOG_ENTRY(ERROR, "Could not retrieve changelog");
        _dia->onUpdateAvailable("");
        return;
    }
    int right = text.indexOf(patternR, left + patternL.length());
    int lenght = right - (left + patternL.length());
    _dia->onUpdateAvailable(text.mid(left + patternL.length(), lenght));
}

void UpdaterGithub::downloadChangelog()
{
    QFile changelog(QCoreApplication::applicationDirPath()+"/CHANGELOG.txt");
    if (!changelog.open(QFile::WriteOnly))
    {
        LOG_ENTRY(ERROR, "Failed to open file " << changelog);
        return;
    }
    auto url = _updatePageAddress.arg("CHANGELOG.md").replace("releases", _latestVersionString).replace("github", "raw.githubusercontent");
    auto req = QNetworkRequest(QUrl(url));
    req.setSslConfiguration(QSslConfiguration::defaultConfiguration()); // HTTPS

    auto reply = _networkManager.get(req);
    if (!reply)
    {
        LOG_ENTRY(ERROR, "Network request rejected.");
        return;
    }
    while (reply->isRunning())
        QCoreApplication::processEvents();
    changelog.write(reply->readAll());
    changelog.close();
    reply->deleteLater();
}

void UpdaterGithub::downloadUpdate()
{
	assert(!_downloadedBinaryFile.isOpen());
    _downloadedBinaryFile.setFileName(QCoreApplication::applicationFilePath() + ".NEW");
    if (!_downloadedBinaryFile.open(QFile::WriteOnly))
	{
		LOG_ENTRY(ERROR, "Failed to open temporary file " << _downloadedBinaryFile);
		return;
	}
    downloadChangelog();

    auto url = _updatePageAddress.arg("download/%1/LatestUpdate.exe").arg(_latestVersionString);
    auto req = QNetworkRequest(QUrl(url));
    req.setSslConfiguration(QSslConfiguration::defaultConfiguration()); // HTTPS
	req.setMaximumRedirectsAllowed(5);
	req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

    this->_reply = _networkManager.get(req);
    if (!this->_reply)
	{
        LOG_ENTRY(ERROR, "Network request rejected.");
        return;
	}

	connect(this->_reply, &QNetworkReply::readyRead, this, this->_onNewDataDownloaded);
    connect(this->_reply, &QNetworkReply::downloadProgress, _dia, _dia->onUpdateDownloadProgress);
    connect(this->_reply, &QNetworkReply::finished, this, this->_onDownloadFinished, Qt::UniqueConnection);
}

void UpdaterGithub::_onNewDataDownloaded()
{
    if (!this->_reply)
        return;
    
    _downloadedBinaryFile.write(this->_reply->readAll());
}

void UpdaterGithub::_onDownloadFinished()
{
    _downloadedBinaryFile.close();

    this->_reply->deleteLater();

    if (this->_reply->error() != QNetworkReply::NoError)
    {
        LOG_ENTRY(ERROR,"Error while downloading the update " << this->_reply->errorString());
        auto dia = new DialogWindow(this->_parent, "Update",
                                    "Error while downloading the update", Status::ERROR0);
        dia->show();
        _dia->close();
        return;
    }

    _dia->onUpdateDownloadFinished();
}

void UpdaterGithub::installUpdate()
{
    QFile currentExe = QCoreApplication::applicationFilePath();
    QFile dest = currentExe.fileName() + ".OLD";
    if (dest.exists())
        dest.remove();
    currentExe.rename(currentExe.fileName() + ".OLD");
    
    _downloadedBinaryFile.rename(QCoreApplication::applicationFilePath());

    if (!QProcess::startDetached('\"' + _downloadedBinaryFile.fileName() + '\"'))
    {
        LOG_ENTRY(ERROR, "Failed to launch the downloaded update.");
        return;
    }
    QCoreApplication::exit();
}