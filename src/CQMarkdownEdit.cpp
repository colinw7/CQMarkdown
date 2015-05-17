#include <CQMarkdownEdit.h>
#include <CQMarkdown.h>

CQMarkdownEdit::
CQMarkdownEdit(CQMarkdown *markdown) :
 QTextEdit(markdown), markdown_(markdown)
{
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  connect(this, SIGNAL(textChanged()), markdown_, SLOT(updatePreview()));
}

QSize
CQMarkdownEdit::
sizeHint() const
{
  return QSize(500, 600);
}
