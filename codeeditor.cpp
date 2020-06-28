#include "codeeditor.h"

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent) {
    lineNumberArea = new LineNumberArea(this);

    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

int CodeEditor::lineNumberAreaWidth() {
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */) {
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy) {
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

int CodeEditor::getTabDeep()
{
    int resDeep = 0;

    QString str = toPlainText();
    int currentPos = textCursor().position();
    for (int i = 0; i < currentPos; i++) {
        if (str[i] == "{")
            resDeep++;
        else if (str[i] == "}")
            resDeep--;
    }

    return resDeep;
}

QList<QTextEdit::ExtraSelection> CodeEditor::bracketHighlighting() {
    QString str = toPlainText();
    int currentPos = textCursor().position();
    int pairBracketPosition = -1;

    if (str[currentPos] == "{")
        pairBracketPosition = getPairBracket(currentPos+1, SearchDirect::direct, "{", "}");

    else if (currentPos > 1 && str[currentPos-1] == "}") {
        pairBracketPosition = getPairBracket(currentPos-2, SearchDirect::reverse, "}", "{");
        currentPos--;
    }

    else if (str[currentPos] == "(")
        pairBracketPosition = getPairBracket(currentPos+1, SearchDirect::direct, "(", ")");

    else if (currentPos > 1 && str[currentPos-1] == ")") {
        pairBracketPosition = getPairBracket(currentPos-2, SearchDirect::reverse, ")", "(");
        currentPos--;
    }

    else if (str[currentPos] == "[")
        pairBracketPosition = getPairBracket(currentPos+1, SearchDirect::direct, "[", "]");

    else if (currentPos > 1 && str[currentPos-1] == "]") {
        pairBracketPosition = getPairBracket(currentPos-2, SearchDirect::reverse, "]", "[");
        currentPos--;
    }

    if (pairBracketPosition < 0)
        return QList<QTextEdit::ExtraSelection>();

    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!this->isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(98, 99, 155, 150);
        selection.format.setBackground(lineColor);
        selection.cursor = this->textCursor();
        selection.cursor.setPosition(pairBracketPosition);
        selection.cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 1);
        extraSelections.append(selection);
        selection.cursor.setPosition(currentPos);
        selection.cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 1);
        extraSelections.append(selection);
    }

    return extraSelections;
}

int CodeEditor::getPairBracket(int curPos, const CodeEditor::SearchDirect &searchDirect, const QString &incrementSymbol,
                                                                                const QString &decrementSymbol) {
    int pairMatching = 1;
    QString text = toPlainText();

    while (curPos >= 0 && curPos <= text.length()-1) {
        if (text[curPos] == incrementSymbol)
            pairMatching++;
        else if (text[curPos] == decrementSymbol)
            pairMatching--;
        if (pairMatching == 0) {
            return curPos;
        }

        curPos += searchDirect;
    }

    return -1;
}

bool CodeEditor::passRightBracket(QKeyEvent *e) {
    QString plainText = this->toPlainText();
    int curCursor = this->textCursor().position();
    QString incrementSymbol;
    QString decrementSymbol;
	
    switch (e->key()) {
        case Qt::Key_BraceRight:
            incrementSymbol = "}";
            decrementSymbol = "{";
            break;
        case Qt::Key_BracketRight:
            incrementSymbol = "]";
            decrementSymbol = "[";
            break;
        case Qt::Key_ParenRight:
            incrementSymbol = ")";
            decrementSymbol = "(";
            break;
        case Qt::Key_QuoteDbl:
            incrementSymbol = "\0";
            decrementSymbol = "\"";
            break;
    }

    if (plainText[curCursor] == incrementSymbol || (incrementSymbol == "\0" && plainText[curCursor] == "\"")) {
        if (getPairBracket(curCursor-2, SearchDirect::reverse, incrementSymbol, decrementSymbol) != -1) {
            textCursor().deletePreviousChar();
            QTextCursor cur = textCursor();
            cur.movePosition(QTextCursor::NextCharacter);
            setTextCursor(cur);
            return true;
        }
    }

    return false;
}

bool CodeEditor::deletePairBrackets() {
    std::string plainText = this->toPlainText().toUtf8().constData();
    int curCursor = this->textCursor().position();

    switch (plainText[curCursor]) {
        case '}':
            if (plainText[curCursor-1] == '{')
                break;
            return false;
        case ']':
            if (plainText[curCursor-1] == '[')
                break;
            return false;
        case ')':
            if (plainText[curCursor-1] == '(')
                break;
            return false;
        case '\"':
            if (plainText[curCursor-1] == '\"')
                break;
            return false;
        default:
            qDebug() << "FALSE: ";
            qDebug() << plainText[curCursor-1] << endl;
            return false;
    }

    textCursor().deletePreviousChar();
    QTextCursor cur = textCursor();
    cur.movePosition(QTextCursor::NextCharacter);
    setTextCursor(cur);
    textCursor().deletePreviousChar();
    return true;
}

void CodeEditor::resizeEvent(QResizeEvent *e) {
    QPlainTextEdit::resizeEvent(e);
    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Backspace) {
        if (deletePairBrackets())
            return;
    }

    QPlainTextEdit::keyPressEvent(e);
    bool isPassing = passRightBracket(e);

    if (e->type() == QEvent::KeyPress && e->key() == Qt::Key_BraceLeft) {
            textCursor().insertText("}");
            QTextCursor cur = textCursor();
            cur.movePosition(QTextCursor::PreviousCharacter);
            setTextCursor(cur);
    }
    else if (e->type() == QEvent::KeyPress && e->key() == Qt::Key_BracketLeft) {
            textCursor().insertText("]");
            QTextCursor cur = textCursor();
            cur.movePosition(QTextCursor::PreviousCharacter);
            setTextCursor(cur);
    }
    else if (e->type() == QEvent::KeyPress && e->key() == Qt::Key_ParenLeft) {
            textCursor().insertText(")");
            QTextCursor cur = textCursor();
            cur.movePosition(QTextCursor::PreviousCharacter);
            setTextCursor(cur);
    }
    else if (e->type() == QEvent::KeyPress && e->key() == Qt::Key_QuoteDbl) {
            if (isPassing == true)
                return;
            textCursor().insertText("\"");
            QTextCursor cur = textCursor();
            cur.movePosition(QTextCursor::PreviousCharacter);
            setTextCursor(cur);
    }
    else if (e->type() == QEvent::KeyPress && e->key() == Qt::Key_Return) {
        int deep = getTabDeep();
        QString str = toPlainText();
        int cursorPos = textCursor().position();
        if (cursorPos >= 2 && str[cursorPos-2] == "{" && str[cursorPos] == "}") {
            if (deep >= 0)
                textCursor().insertText(std::string(deep, '\t').c_str());
            int saveCursorPos = textCursor().position();
            textCursor().insertText("\n");
            if (deep >= 1)
                textCursor().insertText(std::string(deep-1, '\t').c_str());
            QTextCursor cur = textCursor();
            cur.setPosition(saveCursorPos);
            setTextCursor(cur);
        }
        else {
            if (deep >= 0)
                textCursor().insertText(std::string(deep, '\t').c_str());
        }
    }
}

void CodeEditor::highlightCurrentLine() {
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(191, 171, 136, 150);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
        extraSelections.append(bracketHighlighting());
    }


    setExtraSelections(extraSelections);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event) {
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}
