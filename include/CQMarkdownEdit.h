#ifndef CQMarkdownEdit_H
#define CQMarkdownEdit_H

#include <QFrame>
#include <QTextEdit>

class CQMarkdown;
class CQMarkdownEditToolBar;
class CQMarkdownEditText;
class CQMarkdownEditSyntaxHighlight;

class QTimer;
class QToolButton;

class CQMarkdownEdit : public QFrame {
  Q_OBJECT

 public:
  CQMarkdownEdit(CQMarkdown *markdown);

  CQMarkdownEditText *edit() const { return edit_; }

  QString text() const;
  void setText(const QString &text);

  QSize sizeHint() const;

 public slots:
  void normalSlot();
  void boldSlot();
  void italicSlot();
  void headerSlot();
  void ulSlot();
  void olSlot();
  void blockquoteSlot();
  void indentSlot();
  void outdentSlot();
  void linkSlot();
  void imageSlot();

 private:
  QString removeListChars(const QString &str) const;

 private slots:
  void updateSlot();
  void selectionSlot();

 private:
  CQMarkdown            *markdown_ { nullptr };
  CQMarkdownEditToolBar *toolbar_  { nullptr };
  CQMarkdownEditText    *edit_     { nullptr };
  QTimer                *timer_    { nullptr };
};

//------

class CQMarkdownEditToolBar : public QFrame {
  Q_OBJECT

 public:
  CQMarkdownEditToolBar(CQMarkdownEdit *edit);

  CQMarkdownEdit *edit() const { return edit_; }

  void updateState(bool selected);

  int headerInd(QToolButton *button) const;

 private:
  typedef std::vector<QToolButton *> HeaderButtons;

  CQMarkdownEdit *edit_             { nullptr };
  QToolButton    *normalButton_     { nullptr };
  QToolButton    *boldButton_       { nullptr };
  QToolButton    *italicButton_     { nullptr };
  HeaderButtons   hButtons_;
  QToolButton    *ulButton_         { nullptr };
  QToolButton    *olButton_         { nullptr };
  QToolButton    *blockquoteButton_ { nullptr };
  QToolButton    *indentButton_     { nullptr };
  QToolButton    *outdentButton_    { nullptr };
  QToolButton    *linkButton_       { nullptr };
  QToolButton    *imageButton_      { nullptr };
};

//------

class CQMarkdownEditText : public QTextEdit {
  Q_OBJECT

 public:
  CQMarkdownEditText(CQMarkdownEdit *edit);

  void keyPressEvent(QKeyEvent *e);

 private:
  CQMarkdownEdit                *edit_ { nullptr };
  CQMarkdownEditSyntaxHighlight *highlighter_ { nullptr };
};

#endif
