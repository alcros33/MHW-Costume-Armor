#include "LogViewer.hpp"

#include <QMessageBox>

// Begin ThreadLogReader member definition
ThreadLogReader::ThreadLogReader(const QString &file_name, int num_fields) : _file(file_name), _numFields(num_fields) {}
void ThreadLogReader::run()
{
    if (!_file.open(QIODevice::ReadOnly))
    {
        _file.close();
        emit resultReady(QList<QStringList>());
        return;
    }
    QString data = _file.readAll();
    _file.close();

    auto logEntrys = data.split("\n", QString::SkipEmptyParts);
    if (_currentRows == logEntrys.length())
        return;
    
    QList<QStringList> logs;
    for (const auto &entry : logEntrys.mid(_currentRows))
    {
        auto splitted = entry.split(" ", QString::SkipEmptyParts);
        QStringList joined = splitted.mid(0, _numFields - 1);
        joined.append(QStringList(splitted.mid(_numFields - 1)).join(" "));
        logs << joined;
    }
    _currentRows += logs.length();
    emit resultReady(logs);
}
void ThreadLogReader::start()
{
    QThread::start();
}
// End ThreadLogReader member definition

// Begin TableLogMoodel member definition
TableLogModel::TableLogModel(QWidget *parent, const QStringList &fields, const QList<int> &important_fields) : QAbstractTableModel(parent),
_fields(fields),
_importantFields(important_fields) {}

QVariant TableLogModel::data(const QModelIndex &idx, int role) const
{
    if (!idx.isValid())
        return QVariant();

    // setSpan
    // LogTableModel is instantiated

    if (role == Qt::BackgroundRole)
        return _colors[idx.row() % 2];

    if (role == Qt::TextAlignmentRole)
        return Qt::AlignBaseline;

    auto value = _log[idx.row()][idx.column()];

    if (role == Qt::EditRole || role == Qt::DisplayRole)
        return value;

    if (role == Qt::FontRole)
    {
        auto font = QFont();
        if(value.length() > 50)
            font.setPointSize(9);
        return font;
    }

    if (role == Qt::ForegroundRole)
    {
        if (_importantFields.contains(idx.column()))
            return _importantForeground;
        return _foreground;
    }
    return QVariant();
}

QVariant TableLogModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return _fields[section];
    if (role == Qt::ForegroundRole)
    {
        return _colors[0];
    }
    if (role == Qt::FontRole)
    {
        auto font = QFont();
        font.setBold(true);
        return font;
    }
    return QVariant();
}

void TableLogModel::updateModel(const QList<QStringList> &data)
{
    int old_len = _log.length();
    emit this->layoutAboutToBeChanged();
    _log += data;
    emit this->dataChanged(this->createIndex(old_len, 0),this->createIndex(_log.length(),columnCount()));
    emit this->layoutChanged();
    qobject_cast<TableLogView *>(this->parent())->scrollToBottom();
}

// End TableLogMoodel member definition

// Begin TableLogView member definition
TableLogView::TableLogView(QWidget *parent, const QStringList &fields,
                            const QList<int> &important_fields):
QTableView(parent),
_model(this, fields, important_fields)
{
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setWordWrap(false);
    this->setSortingEnabled(false);
    this->verticalScrollBar()->setDisabled(false);
    this->verticalHeader()->hide();
    this->setModel(&_model);
}

void TableLogView::setColumsHeaderWidths()
{
    auto header = this->horizontalHeader();

    for (int i = 0; i < header->count() - 1; ++i)
    {
        header->setSectionResizeMode(i, QHeaderView::ResizeToContents);
        header->setSectionResizeMode(i, QHeaderView::Interactive);
    }
    header->setSectionResizeMode(header->count() - 1, QHeaderView::Stretch);
}
// End TableLogView member definition

// Begin LogWindow member definition
LogWindow::LogWindow(QWidget *parent, const QString &file_name, const QStringList &fields,
              const QList<int> &important_fields):
QMainWindow(parent),
_logReader(file_name, fields.length()),
_centralWidget(this),
_layout(&_centralWidget),
_tableView(&_centralWidget, fields, important_fields),
_hintLabel("Double click on an entry to view details", &_centralWidget)
{
    this->setWindowTitle("Log Viewer");
    this->setGeometry(70, 150, 1200, 700);

    _tableView.setColumsHeaderWidths();
    _tableView.setAlternatingRowColors(true);
    _tableView.setShowGrid(false);
    _layout.addWidget(&_tableView);
    connect(&_tableView, _tableView.doubleClicked, this, _rowDetails);

    _hintLabel.setStyleSheet("font-style: italic;");
    this->statusBar()->addPermanentWidget(&_hintLabel);

    this->setCentralWidget(&_centralWidget);

    connect(&_logReader, _logReader.resultReady, _tableView.model(), _tableView.model()->updateModel);
    _logReader.run();

    connect(&_timer, _timer.timeout, &_logReader, _logReader.start);
    _timer.start(500);

    this->setAttribute(Qt::WA_DeleteOnClose, true);
}

void LogWindow::_rowDetails(const QModelIndex &idx)
{
    auto dia = new QMessageBox(this);
    dia->setIcon(QMessageBox::Information);
    dia->setText("Details");
    dia->setInformativeText(_tableView.model()->log()[idx.row()].last());
    dia->setStandardButtons(QMessageBox::Ok);
    dia->show();
}
// End LogWindow member definition