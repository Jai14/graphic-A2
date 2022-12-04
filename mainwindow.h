#pragma once

#include "glwidget.h"
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    GLWidget *opengl;
    ~MainWindow();

public slots:
    void resetCamera();
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
};
