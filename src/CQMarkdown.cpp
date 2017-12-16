#include <CQMarkdown.h>
#include <CQMarkdownEdit.h>
#include <CQMarkdownPreview.h>
#include <QFile>
#include <QTextStream>

CQMarkdown::
CQMarkdown(QWidget *parent, bool ref) :
 QSplitter(parent), filename_("")
{
  setObjectName("markdown");

  edit_    = new CQMarkdownEdit(this);
  preview_ = new CQMarkdownPreview(this, ref);

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
saveMarkdown(const QString &filename)
{
  QString filename1 = filename;

  if (filename1 == "")
    filename1 = filename_;

  if (filename1 == "")
    return false;

  QFile file(filename1);

  file.open(QIODevice::WriteOnly);

  if (! file.isOpen())
    return false;

  QTextStream outStream(&file);

  outStream << edit_->text();

  file.close();

  return true;
}

bool
CQMarkdown::
saveHtml(const QString &filename)
{
  if (filename == "")
    return false;

  QFile file(filename);

  file.open(QIODevice::WriteOnly);

  if (! file.isOpen())
    return false;

  QTextStream outStream(&file);

  outStream << preview_->html();

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
