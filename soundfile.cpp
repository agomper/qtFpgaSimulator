#include "soundfile.h"

SndfileHandle SoundFile::getSndfd() const
{
    return sndfd;
}

string SoundFile::getSoundFileName() const
{
    return soundFileName;
}

void SoundFile::setSoundFileName(const string &value)
{
    soundFileName = value;
}

int SoundFile::getFileNChannels() const
{
    return fileNChannels;
}

void SoundFile::setFileNChannels(int value)
{
    fileNChannels = value;
}

int SoundFile::getFileSampleRate() const
{
    return fileSampleRate;
}

void SoundFile::setFileSampleRate(int value)
{
    fileSampleRate = value;
}

SoundFile::SoundFile()
{

}

//Last argument: Format | Subtype
bool SoundFile::create_file(int fFormat){
    fileFormat = fFormat;

    sndfd = SndfileHandle (soundFileName, SFM_WRITE, fileFormat,
                           fileNChannels, fileSampleRate) ;

    if (sndfd != NULL) {
        //        cout<<"File created. \n ";
        return true;
    }
    else {
        //        cout<<"File creation error. \n";
        //        printf("%s\n",sf_strerror(NULL));
        //        exit(1);
        return false;
    }
}

int SoundFile::open_file() {
    sndfd = SndfileHandle(soundFileName);

    if (sndfd != NULL) {
//        cout<<"File opened \n";
//        cout<<"Name: "<< soundFileName  <<endl;
//        cout<<"Sample rate: "<< sndfd.samplerate() <<endl;
//        cout<<"Channels: "<< sndfd.channels() <<endl;
        fileSampleRate = sndfd.samplerate();
        fileNChannels = sndfd.channels();

        if (sndfd.samplerate()==0)
            return 0;
        else
            return 1;
    }
    else {
        cout<<"File creation error. \n";
        printf("%s\n",sf_strerror(NULL));
        return 0;
    }
}
