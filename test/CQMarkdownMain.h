#ifndef CQMarkdownMain_H
#define CQMarkdownMain_H

#include <QMainWindow>

class CQMarkdown;
class CQMarkdownConfigDlg;

class CQMarkdownMain : public QMainWindow {
  Q_OBJECT

 public:
  CQMarkdownMain(bool ref=false);

  void load(const QString &filename);

 public slots:
  void updateText();

 private slots:
  void saveMarkdownSlot();
  void saveMarkdownAsSlot();
  void saveHtmlSlot();

  void exitSlot();

  void fixedFontSlot(bool);

  void configSlot();

 private:
  CQMarkdown*          markdown_  { nullptr };
  CQMarkdownConfigDlg* configDlg_ { nullptr };
};

#endif
