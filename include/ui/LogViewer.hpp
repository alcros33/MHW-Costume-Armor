#pragma once
#include <QHeaderView>
#include <QLabel>
#include <QMainWindow>
#include <QScrollBar>
#include <QStatusBar>
#include <QTableView>
#include <QThread>
#include <QTimer>
#include <QPushButton>
#include <QVBoxLayout>

#include "file_helpers.hpp"

class ThreadLogReader : public QThread
{
    Q_OBJECT
public:
    ThreadLogReader(const QString &file_name, int num_fields);

public slots:
    void run() override;
    void start();

signals:
    void resultReady(const QList<QStringList> &result);

private:
    QFile _file;
    int _currentRows = 0;
    int _numFields;
};

class TableLogModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    TableLogModel(QWidget *parent, const QStringList &fields,
                  const QList<int> &important_fields);

    QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    inline int rowCount(const QModelIndex &parent = QModelIndex()) const { return _log.count(); }
    inline int columnCount(const QModelIndex &parent = QModelIndex()) const { return _fields.count(); }

    inline QList<QStringList>& log() { return _log; }

public slots:
    void updateModel(const QList<QStringList> &data);

private:
    QStringList _fields;
    QList<QStringList> _log;
    QList<int> _importantFields;
    QList<QColor> _colors{QColor(30, 30, 30), QColor(50, 50, 50)};
    QColor _importantForeground = QColor(65, 190, 250);
    QColor _foreground = QColor(255, 255, 255);
};

class TableLogView: public QTableView
{
    Q_OBJECT
public:
    TableLogView(QWidget *parent, const QStringList &fields,
                 const QList<int> &important_fields);
    void setColumsHeaderWidths();
    inline TableLogModel* model() { return &_model; }

private:
    TableLogModel _model;
};

class LogWindow: public QMainWindow 
{
    Q_OBJECT
public:
    LogWindow(QWidget *parent, const QString &file_name, const QStringList &fields,
              const QList<int> &important_fields = {});

    inline TableLogView* tableView() { return &_tableView; }

public slots:
    void _rowDetails(const QModelIndex &idx);
private:
    ThreadLogReader _logReader;
    QWidget _centralWidget;
    QVBoxLayout _layout;
    TableLogView _tableView;
    QLabel _hintLabel;
    QTimer _timer;
};