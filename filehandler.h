#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <QString>
#include <QPlainTextEdit>
#include <QFileDialog>
#include <QTextStream>
#include <QTextCodec>
#include <QObject>

class FileHandler {
private:
    QString _currentFilePath;

public:
    FileHandler() = default;
    bool openFile(QPlainTextEdit *textEdit);
    bool saveFile(QPlainTextEdit *textEdit);
    bool saveFileAs(QPlainTextEdit *textEdit);
    bool createFile();
    QString getFileName() const;
    QString getFilePath() const;
    QString readFile(const QString& filePath) const;

};

#endif // FILEHANDLER_H
