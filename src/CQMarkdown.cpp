#include <CQMarkdown.h>
#include <CQMarkdownEdit.h>
#include <CQMarkdownPreview.h>
#include <QFile>
#include <QTextStream>

CQMarkdown::
CQMarkdown(QWidget *parent) :
 QSplitter(parent), filename_("")
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
  filename_ = filename;

  QFile file(filename_);

  if (! file.open(QFile::ReadOnly | QFile::Text))
    return false;

  QTextStream stream(&file);

  edit_->setText(stream.readAll());

  return true;
}

bool
CQMarkdown::
save()
{
  if (filename_ == "")
    return false;

  QFile file(filename_);

  file.open(QIODevice::WriteOnly);

  if (! file.isOpen())
    return false;

  QTextStream outStream(&file);

  outStream << edit_->text();

  file.close();

  return true;
}

QString
CQMarkdown::
text() const
{
  return edit_->text();
}

void
CQMarkdown::
updatePreview()
{
  preview_->updateText();
}
