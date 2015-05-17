#include <CQMarkdown.h>
#include <CQMarkdownEdit.h>
#include <CQMarkdownPreview.h>
#include <QFile>
#include <QTextStream>

CQMarkdown::
CQMarkdown(QWidget *parent) :
 QSplitter(parent)
{
  edit_    = new CQMarkdownEdit(this);
  preview_ = new CQMarkdownPreview(this);

  addWidget(edit_);
  addWidget(preview_);
}

bool
CQMarkdown::
load(const QString &filename)
{
  QFile file(filename);

  if (! file.open(QFile::ReadOnly | QFile::Text))
    return false;

  QTextStream stream(&file);

  edit_->setText(stream.readAll());

  return true;
}

QString
CQMarkdown::
text() const
{
  return edit_->toPlainText();
}

void
CQMarkdown::
updatePreview()
{
  preview_->updateText();
}
