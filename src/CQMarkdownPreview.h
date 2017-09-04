#ifndef CQMarkdownPreview_H
#define CQMarkdownPreview_H

#include <CMarkdown.h>
#include <QTabWidget>
#include <vector>
#include <map>

class CQMarkdown;

#ifdef USE_WEB_VIEW
class QWebView;
#endif

class QTextEdit;

class CQMarkdownPreview : public QTabWidget {
 public:
  CQMarkdownPreview(CQMarkdown *markdown);

  void updateText();

  QSize sizeHint() const;

 private:
  CQMarkdown *markdown_;

#ifdef USE_WEB_VIEW
  QWebView   *markHtmlEdit_;
  QWebView   *refHtmlEdit_;
#else
  QTextEdit  *markHtmlEdit_;
  QTextEdit  *refHtmlEdit_;
#endif

  QTextEdit  *markTextEdit_;
  QTextEdit  *refTextEdit_;
  CMarkdown   mark_;
  QString     html_;
};

#endif
