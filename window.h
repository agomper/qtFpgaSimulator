#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>

namespace Ui {
class Window;
}

class Window : public QMainWindow
{
    Q_OBJECT
    bool deactivate;

public:
    explicit Window(QWidget *parent = 0);
    ~Window();

private slots:
    int on_connectButton_clicked();

    void on_setChannelsButton_clicked();

    void on_pushButtonChoose_clicked();

    int on_activateButton_clicked();

    void on_deactivateButton_clicked();

    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::Window *ui;
};

#endif // WINDOW_H
