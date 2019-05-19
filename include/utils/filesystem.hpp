#pragma once
#include <windows.h>
#include <algorithm>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <cctype>
#include <cstdlib>
#include <cerrno>
#include <system_error>
#include <iostream>

namespace filesystem
{
class FilesystemError : public std::runtime_error
{
  public:
    explicit FilesystemError(const std::string &what_arg) : std::runtime_error(what_arg) {}
};
class Path;
std::vector<Path> directory_iterator(const Path &p);

class Path
{
  public:
    // Default Constructor
    Path() : _absolute(false) {}
    // Copy Constructor
    Path(const Path &other) : _actualPath(other._actualPath), _absolute(other._absolute) {}
    // Rvalue Constrcutor
    Path(Path &&other) : _actualPath(std::move(other._actualPath)), _absolute(other._absolute) {}
    // Move equal operator
    Path &operator=(const Path &other)
    {
        _actualPath = other._actualPath;
        _absolute = other._absolute;
        return *this;
    }

    // Move equal operator
    Path &operator=(Path &&other)
    {
        _actualPath = std::move(other._actualPath);
        _absolute = other._absolute;
        return *this;
    }

    // String Constructors
    Path(const char *Str) { _make_path(Str); }
    Path(const std::string &Str) { _make_path(Str); }
    // wstring contructors
    Path(const wchar_t *Str) { _make_path(Str); }
    Path(const std::wstring &Str) { _make_path(Str); }

    // Getters
    size_t size() const { return _actualPath.size(); }
    bool empty() const { return _actualPath.empty(); }
    bool absolute() const { return _absolute; }
    size_t charLength() const
    {
        size_t len = 0;
        for (const auto &part : _actualPath)
            // +1 because null character
            len += part.size();
        return len;
    }

    // Get String
    std::string str() const
    {
        if (this->empty())
            return "";
        std::ostringstream oss;
        // Windows requires a \\?\ prefix to handle paths longer than MAX_PATH
        if (_absolute && charLength() >= MAX_PATH_LEGACY)
        {
            oss << "\\\\?\\";
        }
        for (size_t i = 0; i < (this->size() - 1); ++i)
            oss << _actualPath[i] << '\\';
        oss << _actualPath.back();

        return oss.str();
    }

    // Get Wstring
    std::wstring wstr() const
    {
        std::string temp = this->str();
        int size = MultiByteToWideChar(CP_UTF8, 0, &temp[0], (int)temp.size(), NULL, 0);
        std::wstring result(size, 0);
        MultiByteToWideChar(CP_UTF8, 0, &temp[0], (int)temp.size(), &result[0], size);
        return result;
    }

    std::string name() const
    {
        if (this->empty())
            return "";
        return _actualPath.back();
    }

    std::string filename() const { return this->name();}

    std::string extension() const
    {
        if (this->empty())
            return "";
        std::string name = _actualPath.back();
        size_t pos = name.find_last_of(".");
        if (pos == std::string::npos)
            return "";
        return name.substr(pos + 1);
    }

    Path parent_path() const
    {
        Path Res(*this);

        if (this->absolute() && this->size()==1)
            return Res;

        if (!this->absolute() && this->empty())
            Res._actualPath.push_back("..");
        else
            Res._actualPath.pop_back();
        return Res;
    }

    bool exists() const
    {
        if (GetFileAttributesW(this->wstr().c_str()) != INVALID_FILE_ATTRIBUTES)
            return true;
        DWORD E = GetLastError();
        return (E != ERROR_FILE_NOT_FOUND && E != ERROR_PATH_NOT_FOUND);
    }

    void make_absolute()
    {
        if (this->empty())
            _actualPath.emplace_back(".");
        if (!this->exists() || _absolute )
            return;
        wchar_t Buffer[MAX_PATH_WINDOWS] = {'\0'};
        DWORD Len = GetFullPathNameW(this->wstr().c_str(), MAX_PATH_WINDOWS, Buffer, NULL);
        if (Len == 0)
            throw FilesystemError("Error with absolute Path");

        this->_make_path(Buffer);
    }

    bool is_directory() const
    {
        if (!this->exists())
            return false;
        DWORD result = GetFileAttributesW(this->wstr().c_str());
        return (result & FILE_ATTRIBUTE_DIRECTORY) != 0;
    }

    bool is_file() const
    {
        if (!this->exists())
            return false;
        return !this->is_directory();
    }

    Path operator/(const Path &other) const
    {
        if (other._absolute)
            throw FilesystemError("Expected a relative Path");

        Path result(*this);

        for (const auto &part : other._actualPath)
            result._actualPath.push_back(part);

        return result;
    }

    Path &operator/=(const Path &other)
    {
        if (other._absolute)
            throw FilesystemError("Expected a relative Path");

        for (const auto &part : other._actualPath)
            _actualPath.push_back(part);
        return *this;
    }

    Path append(const Path &other)
    {
        if (other._absolute)
            throw FilesystemError("Expected a relative Path");

        for (const auto &part : other._actualPath)
            _actualPath.push_back(part);
        return *this;
    }

    bool operator==(const Path &other) const { return other._actualPath == _actualPath; }
    bool operator!=(const Path &other) const { return !((*this)==other); }

    std::vector<Path> ls() const 
    {
        return directory_iterator(*this);
    }

    friend std::ostream &operator<<(std::ostream &os, const Path &path)
    {
        os << path.str();
        return os;
    }

    static const size_t MAX_PATH_WINDOWS = 32767;
    static const size_t MAX_PATH_LEGACY = 260;

  private:
    void _make_path(const std::string &Str)
    {
        std::string tmp = Str;
        // Remove prefix just in case
        if (tmp.size() >= 4 && tmp.substr(0, 4) == "\\\\?\\")
            tmp.erase(0, 4);
        _tokenize(tmp, "/\\");
        _absolute = tmp.size() >= 2 && std::isalpha(tmp[0]) && tmp[1] == ':';
    }
    void _make_path(const std::wstring &WStr)
    {
        if (WStr.empty())
        {
            _make_path(std::string());
            return;
        }

        size_t size = WideCharToMultiByte(CP_UTF8, 0, WStr.c_str(), (int)WStr.size(), NULL, 0, NULL, NULL);
        std::string Str(size, ' ');
        WideCharToMultiByte(CP_UTF8, 0, WStr.c_str(), (int)WStr.size(), (char *)Str.c_str(), size, NULL, NULL);
        _make_path(Str);
    }
    void _tokenize(const std::string &Str, const std::string &delim)
    {
        size_t lastPos = 0;
        size_t pos = Str.find_first_of(delim);
        _actualPath.clear();

        while (lastPos != std::string::npos)
        {
            if (pos != lastPos)
                _actualPath.push_back(Str.substr(lastPos, pos - lastPos));
            lastPos = pos;
            if (lastPos == std::string::npos || lastPos + 1 == Str.size())
                break;
            pos = Str.find_first_of(delim, ++lastPos);
        }
    }

    std::vector<std::string> _actualPath;
    bool _absolute;
}; //End of class Path

inline Path current_path()
{
    wchar_t Buffer[Path::MAX_PATH_WINDOWS] = {'\0'};
    if (!_wgetcwd(Buffer, Path::MAX_PATH_WINDOWS))
        throw FilesystemError("Error in getcwd(): " + std::to_string(GetLastError()));
    return Buffer;
}

inline bool create_directory(const Path &p)
{
    if (p.exists())
        return true;
    
    int status = CreateDirectoryW(p.wstr().c_str(), NULL);
    if (status==0)
        throw FilesystemError("Error in CreateDir(): " + std::to_string(GetLastError()));
    return true;
}

inline bool copy_file(const Path &src, const Path &dest, bool overwrite=false)
{
    if (!src.exists())
        return false;
    
    if (dest.exists()&&(!overwrite))
        return false;

    int status = CopyFileW(src.wstr().c_str(), dest.wstr().c_str(), !overwrite);
    if (status == 0)
        throw FilesystemError("Error in CopyFile(): " + std::to_string(GetLastError()));
    return true;
}

inline std::vector<Path> directory_iterator(const Path &p)
{
    if (!p.is_directory())
        throw FilesystemError("Error in directory_iterator, path is not a directory");

    WIN32_FIND_DATAW FindFileData;
    HANDLE hFind;
    hFind = FindFirstFileW((p/"*").wstr().c_str(), &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        auto Err = GetLastError();
        if ( Err != ERROR_NO_MORE_FILES)
            throw FilesystemError("Error in FindFirstFileW :" + std::to_string(Err));
        return std::vector<Path>();
    }

    std::vector<Path> Result;
    do
    {
        Result.push_back(p/FindFileData.cFileName);
    }
    while (FindNextFileW(hFind, &FindFileData) != 0);

    auto Err = GetLastError();
    if (Err != ERROR_NO_MORE_FILES)
        throw FilesystemError("Error in FindNextFileW :" + std::to_string(Err));
    
    FindClose(hFind);
    return Result;
}

} // namespace filesystem