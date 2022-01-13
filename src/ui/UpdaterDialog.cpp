#include "UpdaterDialog.hpp"
#include "UpdaterGithub.hpp"

UpdaterDialog::UpdaterDialog(QWidget *parent, UpdaterGithub* updater,float font_size) :
QDialog(parent),
ui(new Ui::UpdaterDialog),
_updater(updater)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, this->close);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, this->downloadUpdate);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("Download");

    ui->progressBar->setMaximum(0);
    ui->progressBar->setValue(0);
    ui->lblPercentage->setVisible(false);
    ui->lblOperationInProgress->setVisible(false);
    ui->progressBar->setVisible(false);
    static const auto qssTemplate = QStringLiteral("QWidget{font-size:%1pt;}"
                                                   "QLabel#lblOperationInProgress{font-size : %2pt; }"
                                                   "QTextEdit{font-size : %3pt;}"
                                                   "QLabe#updateName{font-size : %4pt;}");
    this->setStyleSheet(qssTemplate.arg(font_size).arg(font_size * 1.25).arg(font_size * 1.5).arg(font_size * 1.75));
}

UpdaterDialog::~UpdaterDialog()
{
	delete ui;
}

void UpdaterDialog::close()
{
    if (_updater->_reply)
        _updater->_reply->close();
    if (_updater->_downloadedBinaryFile.isOpen())
        _updater->_downloadedBinaryFile.close();
    QDialog::close();
    this->deleteLater();
}

void UpdaterDialog::onUpdateAvailable(QString changelog)
{
    ui->changeLogViewer->setText(changelog);
    this->show();
}

void UpdaterDialog::downloadUpdate()
{
	ui->progressBar->setMaximum(100);
	ui->progressBar->setValue(0);
    ui->lblPercentage->setVisible(true);
    ui->lblOperationInProgress->setVisible(true);
    ui->progressBar->setVisible(true);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    _updater->downloadUpdate();
}

void UpdaterDialog::onUpdateDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    float pct = (bytesReceived < bytesTotal) ? bytesReceived * 100 / (float)bytesTotal : 100.0f;
    ui->progressBar->setValue((int)pct);
    ui->lblPercentage->setText(QString::number(pct, 'f', 2) + " %");
}

void UpdaterDialog::onUpdateDownloadFinished()
{
    ui->lblOperationInProgress->setText("Download finished");
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("Restart");
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(false);
    disconnect(ui->buttonBox, &QDialogButtonBox::accepted, this, this->downloadUpdate);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, _updater, _updater->installUpdate);
}
