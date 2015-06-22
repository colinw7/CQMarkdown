#ifndef CQMarkdownEdit_H
#define CQMarkdownEdit_H

#include <QTextEdit>

class CQMarkdown;
class QTimer;

class CQMarkdownEdit : public QTextEdit {
  Q_OBJECT

 public:
  CQMarkdownEdit(CQMarkdown *markdown);

  QSize sizeHint() const;

 private slots:
  void updateSlot();

 private:
  CQMarkdown *markdown_;
  QTimer     *timer_;
};

#endif
