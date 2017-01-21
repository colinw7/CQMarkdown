#include <CQMarkdownEdit.h>
#include <CQMarkdown.h>

#include <QToolButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QTimer>

#include <svg/normal_svg.h>
#include <svg/bold_svg.h>
#include <svg/italic_svg.h>

CQMarkdownEdit::
CQMarkdownEdit(CQMarkdown *markdown) :
 QFrame(markdown), markdown_(markdown)
{
  setObjectName("edit");

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(0);

  toolbar_ = new CQMarkdownEditToolBar(this);

  layout->addWidget(toolbar_);

  edit_ = new QTextEdit;
  edit_->setObjectName("edit");

  layout->addWidget(edit_);

  connect(edit_, SIGNAL(textChanged()), this, SLOT(updateSlot()));

  connect(edit_, SIGNAL(selectionChanged()), this, SLOT(selectionSlot()));

  timer_ = new QTimer;

  timer_->setSingleShot(true);

  connect(timer_, SIGNAL(timeout()), markdown_, SLOT(updatePreview()));
}

void
CQMarkdownEdit::
setText(const QString &text)
{
  edit_->setText(text);
}

QString
CQMarkdownEdit::
text() const
{
  return edit_->toPlainText();
}

void
CQMarkdownEdit::
updateSlot()
{
  timer_->start(100);
}

void
CQMarkdownEdit::
selectionSlot()
{
  bool selected = edit_->textCursor().hasSelection();

  toolbar_->updateState(selected);
}

void
CQMarkdownEdit::
normalSlot()
{
  QString text = edit_->textCursor().selectedText();

  int i   = 0;
  int len = text.length();

  while (i < len && text[i].isSpace())
    ++i;

  while (i < len && (text[i] == '*' || text[i] == '_'))
    ++i;

  int i1 = i;

  i = len - 1;

  while (i > i1 && text[i].isSpace())
    --i;

  while (i > i1 && (text[i] == '*' || text[i] == '_'))
    --i;

  int i2 = i;

  QString text1 = text.mid(i1, i2 - i1 + 1);

  edit_->textCursor().insertText(text1);
}

void
CQMarkdownEdit::
boldSlot()
{
  QString text = edit_->textCursor().selectedText();

  edit_->textCursor().insertText("**" + text + "**");
}

void
CQMarkdownEdit::
italicSlot()
{
  QString text = edit_->textCursor().selectedText();

  edit_->textCursor().insertText("_" + text + "_");
}

QSize
CQMarkdownEdit::
sizeHint() const
{
  return QSize(500, 600);
}

//------

CQMarkdownEditToolBar::
CQMarkdownEditToolBar(CQMarkdownEdit *edit) :
 QFrame(edit), edit_(edit)
{
  setObjectName("toolbar");

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(2); layout->setSpacing(2);

  //---

  normalButton_ = new QToolButton;
  normalButton_->setIcon(CQPixmapCacheInst->getIcon("NORMAL"));

  connect(normalButton_, SIGNAL(clicked()), edit_, SLOT(normalSlot()));

  layout->addWidget(normalButton_);

  //---

  boldButton_ = new QToolButton;
  boldButton_->setIcon(CQPixmapCacheInst->getIcon("BOLD"));

  connect(boldButton_, SIGNAL(clicked()), edit_, SLOT(boldSlot()));

  layout->addWidget(boldButton_);

  //---

  italicButton_ = new QToolButton;
  italicButton_->setIcon(CQPixmapCacheInst->getIcon("ITALIC"));

  connect(italicButton_, SIGNAL(clicked()), edit_, SLOT(italicSlot()));

  layout->addWidget(italicButton_);

  layout->addStretch(1);

  //---

  updateState(false);
}

void
CQMarkdownEditToolBar::
updateState(bool selected)
{
  normalButton_->setEnabled(selected);
  boldButton_  ->setEnabled(selected);
  italicButton_->setEnabled(selected);
}
