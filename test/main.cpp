#include <CQMarkdownMain.h>
#include <CMarkdown.h>
#include <iostream>

#ifdef CQ_APP_H
#include <CQApp.h>
#else
#include <QApplication>
#endif

int
main(int argc, char **argv)
{
#ifdef CQ_APP_H
  CQApp app(argc, argv);
#else
  QApplication app(argc, argv);
#endif

  bool html  = false; // output as html
  bool text  = false; // output as text
  bool ref   = false; // use reference implementation for compare
  bool debug = false; // debug

  QString filename;

  using TagValue = std::map<CMarkdownTagType,QString>;

  TagValue tagColor, tagFont;

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      QString arg(&argv[i][1]);

      if      (arg == "html") {
        html = true;
      }
      else if (arg == "text") {
        text = true;
      }
      else if (arg == "ref") {
        ref = true;
      }
      else if (arg == "debug") {
        debug = true;
      }
      else if (arg == "color") {
        QString colorStr = argv[++i];

        QStringList colorStrs = colorStr.split(";", QString::KeepEmptyParts);

        for (int j = 0; j < colorStrs.length(); ++j) {
          const QString colorStr1 = colorStrs[j];

          QStringList colorStrs1 = colorStr1.split("=", QString::KeepEmptyParts);

          if (colorStrs1.size() == 2) {
            CMarkdownTagType type = CMarkdown::stringToType(colorStrs1[0]);

            if (type != CMarkdownTagType::NONE)
              tagColor[type] = colorStrs1[1];
            else
              std::cerr << "Invalid tag name '" << colorStrs1[0].toStdString() << "'\n";
          }
          else {
            std::cerr << "Invalid color string '" << colorStr1.toStdString() << "'\n";
          }
        }
      }
      else if (arg == "font") {
        QString fontStr = argv[++i];

        QStringList fontStrs = fontStr.split(";", QString::KeepEmptyParts);

        for (int j = 0; j < fontStrs.length(); ++j) {
          const QString fontStr1 = fontStrs[j];

          QStringList fontStrs1 = fontStr1.split("=", QString::KeepEmptyParts);

          if (fontStrs1.size() == 2) {
            CMarkdownTagType type = CMarkdown::stringToType(fontStrs1[0]);

            if (type != CMarkdownTagType::NONE)
              tagFont[type] = fontStrs1[1];
            else
              std::cerr << "Invalid tag name '" << fontStrs1[0].toStdString() << "'\n";
          }
          else {
            std::cerr << "Invalid font string '" << fontStr1.toStdString() << "'\n";
          }
        }
      }
      else {
        continue;
      }
    }
    else {
      filename = argv[i];
    }
  }

  if (! html && ! text) {
    CQMarkdownMain *markdown = new CQMarkdownMain(ref);

    markdown->load(filename);

    markdown->show();

    return app.exec();
  }
  else {
    CMarkdown markdown;

    markdown.setDebug(debug);

    for (const auto &p : tagColor)
      markdown.setTypeColor(p.first, p.second);

    for (const auto &p : tagFont)
      markdown.setTypeFont(p.first, p.second);

    QString text;

    if (html)
      text = markdown.fileToHtml(filename);
    else
      text = markdown.fileToTty(filename);

    std::cout << text.toStdString() << "\n";

    exit(0);
  }
}
