#pragma once
#include <QDialog>
#include <QPushButton>
#include "ui_UpdaterDialog.h"

namespace Ui {
class UpdaterDialog;
}

class UpdaterGithub;

class UpdaterDialog : public QDialog
{
    Q_OBJECT
public:
    UpdaterDialog(QWidget *parent, UpdaterGithub *updater);
    ~UpdaterDialog();

public slots:
    void close();
	void onUpdateAvailable(QString changelog);
	void downloadUpdate();
    
    void onUpdateDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onUpdateDownloadFinished();

private:
    Ui::UpdaterDialog *ui;
    UpdaterGithub *_updater;
};

