#ifndef CQMarkdownMain_H
#define CQMarkdownMain_H

#include <QMainWindow>

class CQMarkdown;

class CQMarkdownMain : public QMainWindow {
  Q_OBJECT

 public:
  CQMarkdownMain(bool ref=false);

  void load(const QString &filename);

 private slots:
  void saveMarkdownSlot();
  void saveHtmlSlot();

  void exitSlot();

  void fixedFontSlot(bool);

 private:
  CQMarkdown *markdown_;
};

#endif
