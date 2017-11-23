#ifndef SENDER_H
#define SENDER_H
#include "jackclient.h"
#include "netclient.h"

class Sender : public JackClient, public NetClient, public SoundFile{
    int srPortNumber;                   //Puerto comunicacion.
    int fpgaPortNumber;
    string srHostName;                //Host IP address
    int inputMode;
    int overflowCounter;
    int underflowCounter;
    int indexControl;
public:
    Sender();
    void create_sr_socket_connection();
    void create_fpga_socket_connection();
    void bind_fpga_ISAddress();
    void sender_socket_test();
    void finish();
    int getSrPortNumber() const;
    void setSrPortNumber(int value);
    int init_ISAddress();
    string getSrHostName() const;
    void setSrHostName(const string &value);
    int getInputMode() const;
    void setInputMode(int value);
    int set_callback_method();
    int callback_method(jack_nframes_t nframes);
    static void *sender_thread(void *arg);
    int getOverflowCounter() const;
    void setOverflowCounter(int value);
    int getUnderflowCounter() const;
    void setUnderflowCounter(int value);
    int getIndexControl() const;
    void setIndexControl(int value);
};

#endif // SENDER_H
