#ifndef CQMarkdownPreview_H
#define CQMarkdownPreview_H

#include <CMarkdown.h>
#include <QTabWidget>
#include <vector>
#include <map>

class CQMarkdown;
class QTextEdit;

class CQMarkdownPreview : public QTabWidget {
 public:
  CQMarkdownPreview(CQMarkdown *markdown);

  void updateText();

  QSize sizeHint() const;

 private:
  CQMarkdown *markdown_;
  QTextEdit  *markHtmlEdit_;
  QTextEdit  *markTextEdit_;
  QTextEdit  *refHtmlEdit_;
  QTextEdit  *refTextEdit_;
  CMarkdown   mark_;
  QString     html_;
};

#endif
