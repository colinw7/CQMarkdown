#include <CQMarkdownEdit.h>
#include <CQMarkdown.h>
#include <QTimer>

CQMarkdownEdit::
CQMarkdownEdit(CQMarkdown *markdown) :
 QTextEdit(markdown), markdown_(markdown)
{
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  connect(this, SIGNAL(textChanged()), this, SLOT(updateSlot()));

  timer_ = new QTimer;

  timer_->setSingleShot(true);

  connect(timer_, SIGNAL(timeout()), markdown_, SLOT(updatePreview()));
}

void
CQMarkdownEdit::
updateSlot()
{
  timer_->start(100);
}

QSize
CQMarkdownEdit::
sizeHint() const
{
  return QSize(500, 600);
}
