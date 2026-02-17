#include "FileIO.h"
#include <string>
#include <fstream>
#include <stdexcept>
#include <algorithm>

using std::string;
using std::ios;
using std::ifstream;
using std::streamsize;
using std::remove;

namespace MinigameEngine
{
    string FileIO::ReadFile(const string& path)
    {
        ifstream file(path, ios::binary | ios::ate);
        if (!file)
            throw std::runtime_error("open failed");

        const streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        string buffer;
        buffer.resize(static_cast<size_t>(size));
        file.read(buffer.data(), size);

        RemoveCR(buffer);

        return buffer;
    }

    void FileIO::RemoveCR(string& buffer)
    {
        buffer.erase(
            remove(buffer.begin(), buffer.end(), '\r'),
            buffer.end()
        );
    }

    string FileIO::ReadFixedWidthText(const string& path, size_t width)
    {
        ifstream file(path);
        if (!file)
            throw std::runtime_error("open failed");

        string result;
        string line;

        while (std::getline(file, line))
        {
            RemoveCR(line);

            if (line.size() > width)
                line = line.substr(0, width);
            else if (line.size() < width)
                line.append(width - line.size(), ' ');

            result += line;
            result += '\n';
        }

        return result;
    }
}