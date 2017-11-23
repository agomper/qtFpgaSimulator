#include "window.h"
#include "ui_window.h"
#include "sender.h"
#include <QFileDialog>

Sender senderObj;

Window::Window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Window)
{
    deactivate = false;
    ui->setupUi(this);
    ui->activateButton->setEnabled(false);
    ui->deactivateButton->setEnabled(false);
    ui->setChannelsButton->setEnabled(false);
    ui->pushButtonChoose->setEnabled(false);
    ui->comboBox->setEnabled(false);
}

Window::~Window()
{
    delete ui;
}

int Window::on_connectButton_clicked()
{
    int initResult;
    QString auxMessage;

    senderObj.setSrPortNumber(ui->spinBoxPortNumber->value());
    senderObj.setSrHostName(ui->lineEditIPAddress->text().toStdString());

    senderObj.create_sr_socket_connection();
    if(senderObj.getSrSocketFD() < 0) {
        ui->messagesPanel->appendPlainText("Simulator socket creation failed.");
        ui->labelConnection->setText("Error!");
        return 0;
    }
    else {
        ui->messagesPanel->appendPlainText("Socket avaliable.");
    }

    initResult = senderObj.init_ISAddress();
    if (initResult == 0) {
        ui->messagesPanel->appendPlainText("IP address: Unknown host.");
        ui->labelConnection->setText("Error!");
        return 0;
    }
    else {
        ui->messagesPanel->appendPlainText("Connected to: "
                                           +ui->lineEditIPAddress->text());
    }

    ui->labelConnection->setText("Ok!");

    senderObj.init_payload(ui->spinBoxPayload->value());
    auxMessage=QString::number(senderObj.getPayloadSamples());
    ui->messagesPanel->appendPlainText("Package audio payload (Samples) :"+auxMessage);
    auxMessage=QString::number(senderObj.getPayloadBytes());
    ui->messagesPanel->appendPlainText("Package audio payload (Bytes) :"+auxMessage);

    ui->connectButton->setEnabled(false);
    ui->lineEditIPAddress->setEnabled(false);
    ui->spinBoxPortNumber->setEnabled(false);
    ui->spinBoxPayload->setEnabled(false);
    ui->comboBox->setEnabled(true);
    return 1;
}

void Window::on_setChannelsButton_clicked() {
    senderObj.setChannels(ui->spinBoxChannels->value());
    QString auxMessage=QString::number(ui->spinBoxChannels->value());
    ui->messagesPanel->appendPlainText("Number of channels: "
                                       +auxMessage);
    ui->activateButton->setEnabled(true);
    ui->setChannelsButton->setEnabled(false);
    ui->spinBoxChannels->setEnabled(false);
    ui->comboBox->setEnabled(false);
}

void Window::on_pushButtonChoose_clicked() {
    int openFileStatus;
    QString auxMessage;

    QString fileName = QFileDialog::getOpenFileName(this);
    ui->messagesPanel->appendPlainText("File path: "+fileName);
    ui->lineEditFilePath->setText(fileName);

    senderObj.setSoundFileName(fileName.toStdString());
    openFileStatus = senderObj.open_file();
    if (openFileStatus == 1) {
        senderObj.setChannels(senderObj.getFileNChannels());

        ui->messagesPanel->appendPlainText("File opened correctly.");
        auxMessage = QString::number(senderObj.getFileNChannels());
        ui->messagesPanel->appendPlainText("File number of channels: "
                                           +auxMessage);
        auxMessage = QString::number(senderObj.getFileSampleRate());
        ui->messagesPanel->appendPlainText("File sample rate: "
                                           +auxMessage);
        ui->spinBoxChannels->setValue(senderObj.getFileNChannels());
    }
    else {
        ui->messagesPanel->appendPlainText("Error opening the file.");
    }

    ui->activateButton->setEnabled(true);
}

int Window::on_activateButton_clicked() {
    int threadCheck;
    pthread_t netcomThread;
    senderObj.setInputMode(ui->comboBox->currentIndex());
    ui->activateButton->setEnabled(false);
    ui->deactivateButton->setEnabled(true);

    if (senderObj.open_jack_client("simulator_client") == true) {
        ui->messagesPanel->appendPlainText("JACK client opened.");
        QString aux1 = QString::fromStdString(senderObj.getClientName());
        QString aux2 = QString::number(senderObj.getSampleRate());
        ui->clientName->setText(aux1);
        ui->sampleRate->setText(aux2);
    }
    else {
        ui->messagesPanel->appendPlainText("Error opening the JACK client.");
        ui->messagesPanel->appendPlainText("Is it JACK server activated? !!!");
        ui->clientName->setText("Error!");
        ui->sampleRate->setText("Error!");
        return 0;
    }
    //Tell the Jack server to call @a process_callback whenever there is
    //work be done, passing @a arg as the second argument.
    senderObj.set_callback_method();
    ui->messagesPanel->appendPlainText("JACK client: Assigned callback function.");
    senderObj.jack_port_make_standard(1);
    ui->messagesPanel->appendPlainText("JACK client: Ports created.");
    senderObj.jack_client_activate(senderObj.getClientfd());
    ui->messagesPanel->appendPlainText("JACK client: Activated.");


    ui->messagesPanel->appendPlainText("Creating simulator thread.");

    threadCheck = pthread_create(&netcomThread,NULL,
                            senderObj.sender_thread, &senderObj);
    if (threadCheck != 0)
        ui->messagesPanel->appendPlainText("Sender thread error.");

    while (deactivate == false) {
        QString counter;

        counter = QString::number(senderObj.getOverflowCounter());
        ui->overflowCounter->setText(counter);
        counter = QString::number(senderObj.getUnderflowCounter());
        ui->underflowCounter->setText(counter);
        counter = QString::number(senderObj.getIndexControl());
        ui->labelIndexControl->setText(counter);

        QCoreApplication::processEvents();
    }

    pthread_join(netcomThread, NULL);
    senderObj.finish();

    ui->messagesPanel->appendPlainText("Client deactivated. Please, restart"
                                       " the program.");

    return 0;
}

void Window::on_deactivateButton_clicked()
{
    deactivate = true;
    senderObj.setDeactivateSignal(true);
    ui->activateButton->setEnabled(false);
    ui->deactivateButton->setEnabled(false);
}

void Window::on_comboBox_currentIndexChanged(int index)
{
    if (index == 0) { //Microphone
        ui->pushButtonChoose->setEnabled(false);
        ui->lineEditFilePath->setEnabled(false);
        ui->spinBoxChannels->setEnabled(true);
        ui->setChannelsButton->setEnabled(true);
        ui->activateButton->setEnabled(true);
    }
    else if (index == 1) { //File
        ui->pushButtonChoose->setEnabled(true);
        ui->lineEditFilePath->setEnabled(true);
        ui->spinBoxChannels->setEnabled(false);
        ui->setChannelsButton->setEnabled(false);
        ui->activateButton->setEnabled(true);
    }
    else if (index = 2) { //Nothing
        ui->pushButtonChoose->setEnabled(false);
        ui->lineEditFilePath->setEnabled(false);
        ui->spinBoxChannels->setEnabled(false);
        ui->setChannelsButton->setEnabled(false);
        ui->activateButton->setEnabled(false);
    }
}
