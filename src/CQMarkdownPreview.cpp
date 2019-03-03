#include <CQMarkdownPreview.h>
#include <CQMarkdown.h>
#include <CCommand.h>
#ifdef USE_WEB_VIEW
#include <QWebView>
#endif
#include <QTextEdit>

namespace {

QString runMarkdown(const QString &str) {
  char *env = getenv("CQMARKDOWN_EXEC");

  CCommand cmd(env ? env : "markdown");

  cmd.addStringSrc(str.toStdString());

  std::string res;

  cmd.addStringDest(res);

  cmd.start();

  cmd.wait();

  return QString(res.c_str());
}

}

//---

CQMarkdownPreview::
CQMarkdownPreview(CQMarkdown *markdown, bool ref) :
 QTabWidget(markdown), markdown_(markdown)
{
  setObjectName("preview");

#ifdef USE_WEB_VIEW
  if (ref) {
    addTab(markHtmlEdit_ = new QWebView, "Mark-HTML");
    addTab(refHtmlEdit_  = new QWebView, "Ref-HTML" );
  }
  else {
    addTab(markHtmlEdit_ = new QWebView, "HTML Output");
  }
#else
  if (ref) {
    addTab(markHtmlEdit_ = new QTextEdit, "Mark-HTML");
    addTab(refHtmlEdit_  = new QTextEdit, "Ref-HTML" );

    markHtmlEdit_->setReadOnly(true);
    refHtmlEdit_ ->setReadOnly(true);
  }
  else {
    addTab(markHtmlEdit_ = new QTextEdit, "HTML");

    markHtmlEdit_->setReadOnly(true);
  }
#endif

  if (ref) {
    addTab(markTextEdit_ = new QTextEdit, "Mark-Text");
    addTab(refTextEdit_  = new QTextEdit, "Ref-Text" );

    markTextEdit_->setReadOnly(true);
    refTextEdit_ ->setReadOnly(true);
  }
  else {
    addTab(markTextEdit_ = new QTextEdit, "HTML Text");

    markTextEdit_->setReadOnly(true);
  }

  markHtmlEdit_->setObjectName("markHtmlEdit");
  markTextEdit_->setObjectName("markTextEdit");

  if (ref) {
    refHtmlEdit_->setObjectName("refHtmlEdit");
    refTextEdit_->setObjectName("refTextEdit");
  }

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void
CQMarkdownPreview::
updateText()
{
  QString str = markdown_->text();

  if (refHtmlEdit_ || refTextEdit_) {
    QString refHtml = runMarkdown(str);

    if (refHtmlEdit_) {
#ifdef USE_WEB_VIEW
      refHtmlEdit_->setHtml(refHtml);
#else
      refHtmlEdit_->setHtml(refHtml);
#endif
    }

    if (refTextEdit_)
      refTextEdit_->setPlainText(refHtml);
  }

  html_ = mark_.textToHtml(str);

#ifdef USE_WEB_VIEW
  markHtmlEdit_->setHtml(html_);
#else
  markHtmlEdit_->setHtml(html_);
#endif
  markTextEdit_->setPlainText(html_);
}

QSize
CQMarkdownPreview::
sizeHint() const
{
  return QSize(800, 1200);
}
