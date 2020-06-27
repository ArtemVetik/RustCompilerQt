#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QPixmap>
#include <thread>
#include "filehandler.h"
#include "highlighter.h"

__declspec(dllimport) int Compile(const char *);
struct __declspec(dllimport) ErrorPosition {
    unsigned int startPos;
    unsigned int endPos;
};
__declspec(dllimport) ErrorPosition GetErrorPosition();

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow(); 

private slots:
    void on_openFileButton_triggered();
    void on_saveButton_triggered();
    void on_createFileButton_triggered();
    void on_saveAsButton_triggered();
    void on_exitButton_triggered();
    void on_codeEdit_textChanged();
    int on_compileButton_triggered();
    void on_compileAndRunButton_triggered();
    void on_aboutButton_triggered();

private:
    QString _windowTitleTemplate;
    bool _fileSaved;
    FileHandler _file;
    Ui::MainWindow *ui;
    Highlighter *_highlighter;
    std::thread *_executableThread;

   void closeEvent(QCloseEvent *event);
   void changeWindowTitle(const QString &fileName);
   void setupEditor();
   void viewError();
};

#endif // MAINWINDOW_H
