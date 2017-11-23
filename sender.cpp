#include "sender.h"

int Sender::getOverflowCounter() const
{
    return overflowCounter;
}

void Sender::setOverflowCounter(int value)
{
    overflowCounter = value;
}

int Sender::getUnderflowCounter() const
{
    return underflowCounter;
}

void Sender::setUnderflowCounter(int value)
{
    underflowCounter = value;
}

int Sender::getIndexControl() const
{
    return indexControl;
}

void Sender::setIndexControl(int value)
{
    indexControl = value;
}

Sender::Sender() {
    overflowCounter = 0;
    underflowCounter = 0;
    indexControl = 0;
    //    //******FPGA********//
    //    fpgaPortNumber = 57150;

    //    fpgaISAddress.sin_family = AF_INET;
    //    fpgaISAddress.sin_port = htons(fpgaPortNumber);
//    fpgaISAddress.sin_addr.s_addr = htonl(INADDR_ANY);
}

int Sender::getSrPortNumber() const
{
    return srPortNumber;
}

void Sender::setSrPortNumber(int value)
{
    srPortNumber = value;
}

int Sender::init_ISAddress() {
    srISAddress.sin_family = AF_INET;
    srISAddress.sin_port = htons(srPortNumber);

    struct hostent *hostinfo = gethostbyname(srHostName.c_str());

    if (hostinfo == NULL) {
        cout<<"Unknown SR Host. \n";
        return 0;
    }
    else {
        srISAddress.sin_addr = *(struct in_addr *) hostinfo->h_addr_list[0];
        return 1;
    }
}

string Sender::getSrHostName() const
{
    return srHostName;
}

void Sender::setSrHostName(const string &value)
{
    srHostName = value;
}

int Sender::getInputMode() const
{
    return inputMode;
}

void Sender::setInputMode(int value)
{
    inputMode = value;
}

int aux_callback_method(jack_nframes_t nframes, void* p) {
    return static_cast<Sender*>(p)->callback_method(nframes);
}

int Sender::set_callback_method() {
    jack_set_process_callback(clientfd, aux_callback_method, this);
    return 0;
}

/*********************JACK CALLBACK FUNCTION*******************/
int Sender::callback_method(jack_nframes_t nframes) {
    int i, j;                         //Iteradores auxiliares
    float *in[channels];
    float fileBuffer[nframes*channels];

    if (inputMode == 1) {
        sndfd.read(fileBuffer, nframes*channels);
    }

    //Los punteros in apuntan a lo mismo que los Jack port.
    //Se hace un for para igual cada i a un channel.
    for(i = 0; i < channels; i++) {
        in[i] = (float *) jack_port_get_buffer(jackPort[i], nframes);
    }

    //El buffer del paquete d se llena con la informacion de los Jack ports.
    for(i = 0; i < nframes; i++) {  //Siendo n = 1024 (Frames/Period)
        for(j = 0; j < channels; j++) {
            //El j_buffer guarda los datos de cada frame para cada uno
            //de los canales. in[Canal][Frame]
            if (inputMode == 0) {
                jackBuffer[(i*channels)+j]
                        = (float) in[j][i]; //JACK PORTS
            }
            else if (inputMode == 1) {
                jackBuffer[(i*channels)+j]
                        = fileBuffer[(i*channels)+j]; //FILE
            }
            else {
                cout<<"Error JACK callback"<<endl;
                exit(1);
            }
        }
    }

    //Comprueba si hay espacio en buffer.
    int bytes_available = (int) jack_ringbuffer_write_space(ringBuffer);
    int bytes_to_write = nframes * sizeof(float) * channels;
    if(bytes_to_write > bytes_available) {
        printf ("jack-udp send: buffer overflow error (UDP thread late)\n");
        overflowCounter++;
        //cout<<"Overflow Counter: "<<overflow_counter<<endl;
    } else {
        jack_ringbuffer_write_exactly(bytes_to_write);
    }

    char b = 1;
    if(write(comPipe[1], &b, 1)== -1) {
        cout<<"Simulator: error writing communication pipe."<<endl;
        exit(1);
    }
    return 0;
}


void Sender::create_sr_socket_connection() {
    //int socket(int domain, int type, int protocol)
    srSocketFD = socket(AF_INET, SOCK_DGRAM, 0);
//    if(srSocketFD < 0) {
//        cout<<"Sender/Receiver socket creation failed. \n";
//        exit(1);
//    }
}

void Sender::create_fpga_socket_connection() {
    //int socket(int domain, int type, int protocol)
    fpgaSocketFD = socket(AF_INET, SOCK_DGRAM, 0);
    if(fpgaSocketFD < 0) {
        cout<<"FPGA/Sender socket creation failed. \n";
        exit(1);
    }
}

void Sender::bind_fpga_ISAddress()
{
    //int bind(int fd, struct sockaddr *local_addr, socklen_t addr_length)
    int code = bind(fpgaSocketFD,
                    (struct sockaddr *)&fpgaISAddress, sizeof(fpgaISAddress));
    if(code < 0) {
        cout<<("Receiver could not bind the fpgaAddress. \n");
        exit(1);
    }
    cout<<"Waiting on port: "<<fpgaPortNumber<<endl;
}

void Sender::sender_socket_test() {
    char my_message[30] = "This is a test message.";
    /* send a message to the server */
    if (sendto(srSocketFD, my_message, strlen(my_message),
               0, (struct sockaddr *)&srISAddress, sizeof(srISAddress)) < 0) {
        perror("Sender socket test failed. \n");
        exit(1);
    }
}

/**********************SENDER THREAD*******************************/
void *Sender::sender_thread(void *arg) {
    Sender *sender = (Sender *) arg;
    networkPacket p;                           //Network package
    p.index = 0;                          //Inicializa el indice a 0


    while(sender->getDeactivateSignal() == false) {
        sender->jack_ringbuffer_wait_for_read(sender->getPayloadBytes(),
                                              sender->getComPipe(0), 1);

        sender->setIndexControl(sender->getIndexControl()+1);
        p.index = sender->getIndexControl();
        //if (p.index % 1000 == 1)
        //      cout<<"Indice nuevo paquete: "<<p.index<<endl;

        p.channels = sender->getChannels();
        p.frames = sender->getPayloadSamples() / sender->getChannels();

        sender->jack_ringbuffer_read_exactly((char *)&(p.data),
                                             sender->getPayloadBytes());
        sender->packet_sendto(&p, sender->getSrSocketFD(),
                              sender->getSrISAddress());
    }
    //cout<<"Hilo terminado"<<endl;
}



void Sender::finish() {
    close(srSocketFD);
    jack_ringbuffer_free(ringBuffer);
    jack_client_close(clientfd);
    close(comPipe[0]);
    close(comPipe[1]);
    free(jackBuffer);
}

