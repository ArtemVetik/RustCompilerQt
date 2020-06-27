#include "filehandler.h"

bool FileHandler::openFile(QPlainTextEdit *textEdit) {
    _currentFilePath = QFileDialog::getOpenFileName(nullptr, QObject::tr("Открытие файла"), "/home/", QObject::tr("Все файлы (*.rs)"));
    QFile file(_currentFilePath);

    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        textEdit->setPlainText(stream.readAll());
        file.close();
    }

    return !_currentFilePath.isEmpty();
}

bool FileHandler::saveFile(QPlainTextEdit *textEdit) {
    if (_currentFilePath.isEmpty()) {
        _currentFilePath = QFileDialog::getSaveFileName(nullptr, QObject::tr("Сохранить файл как"), "/home/", QObject::tr("Все файлы (*.rs)"));
    }

    QFile file(_currentFilePath);

    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        stream.setCodec("OEM-866");
        QString str;
        stream << textEdit->toPlainText().toUtf8();
        file.close();
    }

    return !_currentFilePath.isEmpty();
}

bool FileHandler::saveFileAs(QPlainTextEdit *textEdit) {
    _currentFilePath.clear();
    return saveFile(textEdit);
}

bool FileHandler::createFile() {
    _currentFilePath = QFileDialog::getSaveFileName(nullptr, QObject::tr("Создание файла"), "/home/", QObject::tr("Все файлы (*.rs)"));
    QFile file(_currentFilePath);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.close();

    return !_currentFilePath.isEmpty();
}

QString FileHandler::getFileName() const {
    QStringList stringList = _currentFilePath.split("/");
    return stringList.back();
}

QString FileHandler::getFilePath() const {
    return _currentFilePath;
}

QString FileHandler::readFile(const QString& filePath) const {
    QFile file(filePath);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    if (file.isOpen()) {
        QString fileBuffer = file.readAll();
        file.close();
        return fileBuffer;
    }

    return "";
}
