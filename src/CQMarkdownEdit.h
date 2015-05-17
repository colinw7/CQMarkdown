#ifndef CQMarkdownEdit_H
#define CQMarkdownEdit_H

#include <QTextEdit>

class CQMarkdown;

class CQMarkdownEdit : public QTextEdit {
  Q_OBJECT

 public:
  CQMarkdownEdit(CQMarkdown *markdown);

  QSize sizeHint() const;

 private:
  CQMarkdown *markdown_;
};

#endif
