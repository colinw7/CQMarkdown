#ifndef CQMarkdownEdit_H
#define CQMarkdownEdit_H

#include <QFrame>

class CQMarkdown;
class CQMarkdownEditToolBar;

class QTextEdit;
class QTimer;
class QToolButton;

class CQMarkdownEdit : public QFrame {
  Q_OBJECT

 public:
  CQMarkdownEdit(CQMarkdown *markdown);

  QTextEdit *edit() const { return edit_; }

  QString text() const;
  void setText(const QString &text);

  QSize sizeHint() const;

 public slots:
  void normalSlot();
  void boldSlot();
  void italicSlot();

 private slots:
  void updateSlot();
  void selectionSlot();

 private:
  CQMarkdown            *markdown_ { nullptr };
  CQMarkdownEditToolBar *toolbar_  { nullptr };
  QTextEdit             *edit_     { nullptr };
  QTimer                *timer_    { nullptr };
};

//------

class CQMarkdownEditToolBar : public QFrame {
  Q_OBJECT

 public:
  CQMarkdownEditToolBar(CQMarkdownEdit *edit);

  void updateState(bool selected);

 private:
  CQMarkdownEdit *edit_         { nullptr };
  QToolButton    *normalButton_ { nullptr };
  QToolButton    *boldButton_   { nullptr };
  QToolButton    *italicButton_ { nullptr };
};

#endif
