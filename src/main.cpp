#include <CQMarkdown.h>
#include <CMarkdown.h>
#include <QApplication>
#include <iostream>

int
main(int argc, char **argv)
{
  QApplication app(argc, argv);

  bool html  = false;
  bool debug = false;

  QString filename;

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      QString arg(&argv[i][1]);

      if      (arg == "html")
        html = true;
      else if (arg == "debug")
        debug = true;
      else
        continue;
    }
    else
      filename = argv[i];
  }

  if (! html) {
    CQMarkdown *markdown = new CQMarkdown;

    markdown->load(filename);

    markdown->show();

    return app.exec();
  }
  else {
    CMarkdown markdown;

    markdown.setDebug(debug);

    QString html = markdown.processFile(filename);

    std::cout << html.toStdString() << std::endl;

    exit(0);
  }
}
