#ifndef CQMarkdownMain_H
#define CQMarkdownMain_H

#include <QMainWindow>

class CQMarkdown;

class CQMarkdownMain : public QMainWindow {
  Q_OBJECT

 public:
  CQMarkdownMain();

  void load(const QString &filename);

 private slots:
  void saveSlot();
  void exitSlot();

 private:
  CQMarkdown *markdown_;
};

#endif
