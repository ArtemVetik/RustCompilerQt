#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), _fileSaved(false), ui(new Ui::MainWindow), _highlighter(nullptr),
                                                                                        _executableThread(nullptr) {
    ui->setupUi(this);

    setupEditor();
    _windowTitleTemplate = this->windowTitle();
}

MainWindow::~MainWindow() {
    delete ui;
    ui = nullptr;
    if (_highlighter) {
        delete _highlighter;
        _highlighter = nullptr;
    }
    if (_executableThread) {
        delete _executableThread;
        _executableThread = nullptr;
    }
}

void MainWindow::on_openFileButton_triggered() {
    _fileSaved = _file.openFile(ui->codeEdit);
    changeWindowTitle(_file.getFileName());
}

void MainWindow::on_saveButton_triggered() {
    _fileSaved = _file.saveFile(ui->codeEdit);
    changeWindowTitle(_file.getFileName());
}

void MainWindow::on_createFileButton_triggered() {
    _fileSaved = _file.createFile();
    ui->codeEdit->setPlainText("");
    changeWindowTitle(_file.getFileName());
}

void MainWindow::changeWindowTitle(const QString &fileName) {
    this->setWindowTitle(_windowTitleTemplate + "   |   " + fileName + ((!_fileSaved) ? "*" : ""));
}

void MainWindow::on_saveAsButton_triggered() {
    _fileSaved = _file.saveFileAs(ui->codeEdit);
    changeWindowTitle(_file.getFileName());
}

void MainWindow::on_exitButton_triggered() {
    if (!_fileSaved) {
        QMessageBox msgBox(this);
        msgBox.setText("Вы хотите сохранить изменения в файле?");
        msgBox.setStandardButtons(QMessageBox::Yes| QMessageBox::No | QMessageBox::Cancel);
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int res = msgBox.exec();
        if (res == QMessageBox::Yes) {
            if (_file.getFileName().isEmpty()) {
                _file.saveFileAs(ui->codeEdit);
                if (_file.getFileName().isEmpty())
                    return;
            }
            else
                _file.saveFile(ui->codeEdit);
        }
        else if (res == QMessageBox::Cancel)
            return;
    }

    QApplication::exit();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    on_exitButton_triggered();
    event->ignore();
}

void MainWindow::on_codeEdit_textChanged() {
    _fileSaved = false;
    changeWindowTitle(_file.getFileName());
}

int MainWindow::on_compileButton_triggered() {
    int compileResult = -1;
    if (!_fileSaved) {
        _fileSaved = _file.saveFile(ui->codeEdit);
    }
    changeWindowTitle(_file.getFileName());
    if (_fileSaved) {
        compileResult = Compile(_file.getFilePath().toUtf8().constData());
        ui->consoleOutput->setText(_file.readFile("Output/consoleOutput.txt"));
        ui->errorMessagesOutput->setText(_file.readFile("Output/errorOutput.txt"));

        if (compileResult != 0) {
            viewError();
        }
    }

    return compileResult;
}

void MainWindow::setupEditor() {
    _highlighter = new Highlighter(ui->codeEdit->document());
}

void MainWindow::on_compileAndRunButton_triggered() {
    if (on_compileButton_triggered() == 0) {
        QString fileName = _file.getFileName().split(".").front() + ".exe";
        QStringList pieces = _file.getFilePath().split("/");
        pieces.removeLast();
        QString filePath = pieces.join("/");
        std::string systemString = "cd " + std::string(filePath.toUtf8().constData()) + " && " + std::string(fileName.toUtf8().constData());
        if (_executableThread)
            delete _executableThread;

        _executableThread = new std::thread([=]()
        {
            system(systemString.c_str());
        });
        _executableThread->detach();
    }
}

void MainWindow::on_aboutButton_triggered()
{
    AboutDialog dialog;
    dialog.exec();
}

void MainWindow::viewError() {
    ErrorPosition errorPosition = GetErrorPosition();
    _highlighter->ErrorHighlighting(ui->codeEdit, errorPosition.startPos, errorPosition.endPos);
    ui->messages->setCurrentIndex(1);
    QList<QDockWidget*> docks;
    QList<int> dockSizes;
    docks.push_back(ui->messageBar);
    dockSizes.push_back(0.45 * MainWindow::height());
    QMainWindow::resizeDocks(docks, dockSizes, Qt::Orientation::Vertical);
}

void MainWindow::on_actionQt_triggered()
{
    QDesktopServices::openUrl(QUrl("https://github.com/ArtemVetik/RustCompilerQt"));
}

void MainWindow::on_actionRustCompiler_triggered()
{
    QDesktopServices::openUrl(QUrl("https://github.com/ArtemVetik/RustCompiler"));
}

void MainWindow::on_actionDocumentation_triggered()
{
    QDesktopServices::openUrl(QUrl("https://artemvetik.github.io/RustCompiler.io"));
}
