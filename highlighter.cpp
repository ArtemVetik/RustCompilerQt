#include "highlighter.h"

Highlighter::Highlighter(QTextDocument *parent) : QSyntaxHighlighter(parent) {
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::darkMagenta);
    keywordFormat.setFontWeight(QFont::Bold);
    const QString keywordPatterns[] = {
        QStringLiteral("\\bfn\\b"), QStringLiteral("\\bi32\\b"), QStringLiteral("\\bf32\\b"),
        QStringLiteral("\\busize\\b"), QStringLiteral("\\bwhile\\b"), QStringLiteral("\\bloop\\b"),
        QStringLiteral("\\bif\\b"), QStringLiteral("\\belse\\b"), QStringLiteral("\\blet\\b"),
        QStringLiteral("\\bmut\\b"), QStringLiteral("&"), QStringLiteral("\\btrue\\b"), QStringLiteral("\\bfalse\\b"),
        QStringLiteral(";"), QStringLiteral("\\breturn\\b"), QStringLiteral("\\bbreak\\b"), QStringLiteral("\\bbool\\b")
    };
    for (const QString &pattern : keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    numberFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression(QStringLiteral("\\b[0-9.]*"));
    rule.format = numberFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::darkGray);

    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression(QStringLiteral("\".*\""));
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_]+!?+(?=\\()"));
    rule.format = functionFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(Qt::darkGray);
    rule.pattern = QRegularExpression(QStringLiteral("//.*"));
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    commentStartExpression = QRegularExpression(QStringLiteral("/\\*"));
    commentEndExpression = QRegularExpression(QStringLiteral("\\*/"));
}

void Highlighter::ErrorHighlighting(QPlainTextEdit* const &editText, int startPos, int endPos)
{
    QTextCursor cursor(editText->textCursor());
    cursor.movePosition(QTextCursor::End);
    editText->setTextCursor(cursor);

    QTextCursor cursor2(editText->textCursor());
    cursor2.setPosition(startPos);
    editText->setTextCursor(cursor2);

    cursor.setPosition(startPos);
    editText->setTextCursor(cursor);

    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!editText->isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(255, 0, 0, 200);
        selection.format.setBackground(lineColor);
        selection.cursor = editText->textCursor();
        selection.cursor.setPosition(startPos);
        selection.cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, endPos - startPos+1);
        extraSelections.append(selection);
    }

    editText->setExtraSelections(extraSelections);
}

void Highlighter::highlightBlock(const QString &text) {
    for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0) {

        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        else {
            commentLength = endIndex - startIndex
                            + match.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}
