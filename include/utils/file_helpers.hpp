#pragma once
#include <string>
#include <iostream>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QString>

inline QDir get_appdata_dir()
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}

inline std::ostream &operator<<(std::ostream &os, const QFile &file)
{
    os << file.fileName().toStdString();
    return os;
}

inline std::wostream &operator<<(std::wostream &os, const QFile &file)
{
    os << file.fileName().toStdWString();
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const QDir &dir)
{
    os << dir.absolutePath().toStdString();
    return os;
}

inline std::wostream &operator<<(std::wostream &os, const QDir &dir)
{
    os << dir.absolutePath().toStdWString();
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const QString &qstr)
{
    os << qstr.toStdString();
    return os;
}

inline std::wostream &operator<<(std::wostream &os, const QString &qstr)
{
    os << qstr.toStdWString();
    return os;
}