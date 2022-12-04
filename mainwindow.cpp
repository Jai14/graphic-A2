#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {

    // TO ADD NEW TEXTURES:
    // 1. Add jpg to startup files folder
    // 2. Add a QFile::copy here to copy the file to the desktop
    // 3. Add the filename to the resources.qrc file in a plain text editor (e.g. notepad)
    // 4. Right click project -> Rebuild
    QFile::copy(":/earth.jpg", QDir::homePath() + QString("/Desktop/earth.jpg"));
    QFile::copy(":/space.jpg", QDir::homePath() + QString("/Desktop/space.jpg"));
    QFile::copy(":/sun.jpg", QDir::homePath() + QString("/Desktop/sun.png"));

    opengl = new GLWidget();
    ui->setupUi(this);
    ui->glScrollArea->setWidgetResizable(true);
    ui->glScrollArea->setWidget(opengl);
    opengl->setFocus();

    QObject::connect(ui->resetButton, SIGNAL(clicked()), this, SLOT(resetCamera()));
    QObject::connect(ui->actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));

    Q_INIT_RESOURCE(resources);
}

void MainWindow::resetCamera() { opengl->resetCamera(); }

void MainWindow::closeEvent(QCloseEvent *event) { qApp->quit(); }

MainWindow::~MainWindow() {
    delete opengl;
    delete ui;
}
