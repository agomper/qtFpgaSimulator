#ifndef SOUNDFILE_H
#define SOUNDFILE_H

#include <sndfile.hh>
#include <cstdio>
#include <string>
#include <iostream>
using namespace std;

class SoundFile {
protected:
    string soundFileName;
    int fileNChannels;
    int fileSampleRate;
    int fileFormat;
    SndfileHandle sndfd;
public:
    SoundFile();
    bool create_file(int fileFormat);
    int open_file();
    void write_file();
    void read_file();
    SndfileHandle getSndfd() const;
    string getSoundFileName() const;
    void setSoundFileName(const string &value);
    int getFileNChannels() const;
    void setFileNChannels(int value);
    int getFileSampleRate() const;
    void setFileSampleRate(int value);
};

#endif // SOUNDFILE_H

