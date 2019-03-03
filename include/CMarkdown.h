#ifndef CMarkdown_H
#define CMarkdown_H

#include <QString>
#include <vector>
#include <map>

class CMarkdownBlock;

//---

enum class CMarkdownTagType {
  NONE,
  ROOT,
  DOCUMENT,
  BLOCKQUOTE,
  P,
  H1,
  H2,
  H3,
  H4,
  H5,
  H6,
  UL,
  OL,
  LI,
  PRE,
  CODE,
  TABLE,
  TR,
  TD,
  HR,
  EM,
  STRONG,
  STRIKE,
  A
};

//---

struct CMarkdownTagData {
  CMarkdownTagType type       { CMarkdownTagType::NONE };
  QString          name;
  QString          color;
  QString          font;
  bool             singleLine { false };
  bool             recurse    { false };

  CMarkdownTagData() { }

  CMarkdownTagData(CMarkdownTagType type, const QString &name) :
   type(type), name(name) {
  }
};

//---

class CMarkdown {
 public:
  enum class Format {
    HTML,
    TTY
  };

  struct LinkRef {
    QString ref;
    QString dest;
    QString title;
  };

  using Links    = std::map<QString,LinkRef>;
  using TagDatas = std::map<CMarkdownTagType,CMarkdownTagData>;

 public:
  CMarkdown();

  //! get/set debug
  bool isDebug() const { return debug_; }
  void setDebug(bool d);

  QString fileToHtml  (const QString &filename);
  QString fileToTty   (const QString &filename);
  QString fileToFormat(const QString &filename, Format format);

  QString textToHtml  (const QString &str);
  QString textToTty   (const QString &str);
  QString textToFormat(const QString &str, Format format);

  void addLink(const LinkRef &link);
  bool getLink(const QString &ref, LinkRef &link) const;

  //---

  static bool isSingleLineType(CMarkdownTagType type);
  static bool isRecurseType   (CMarkdownTagType type);

  static QString typeColor(CMarkdownTagType type);
  static void setTypeColor(CMarkdownTagType type, const QString &color);

  static QString typeFont(CMarkdownTagType type);
  static void setTypeFont(CMarkdownTagType type, const QString &font);

  static QString typeName(CMarkdownTagType type);

  static CMarkdownTagType stringToType(const QString &str);

  static CMarkdownTagData &getTagData(CMarkdownTagType type);

  static TagDatas &getTagDatas();

 private:
  bool readLine(QString &line);

 private:
  using Blocks = std::vector<CMarkdownBlock *>;

  QString str_;       // input string
  int     len_ { 0 }; // input string length
  int     pos_ { 0 }; // input string position

  bool            debug_     { false };
  CMarkdownBlock *rootBlock_ { nullptr };
  Links           links_;
};

//-------

class CMarkdownBlock {
 public:
  struct CodeFence {
    QChar   c;
    int     n { 0 };
    QString info;
  };

  struct LineData {
    QString line;
    int     indent { 0 };
    bool    brk    { false };
    bool    blank  { true };
  };

  struct ListData {
    int     indent { 0 };
    int     n { 0 };
    QChar   c;
    QString text;
  };

  struct Line {
    QString line;
    bool    brk { false };

    Line(const QString &line1, bool brk1=false) :
     line(line1), brk(brk1) {
    }
  };

  struct ATXData {
    CMarkdownTagType type;
    QString          text;
  };

 public:
  using Lines   = std::vector<Line>;
  using LinkRef = CMarkdown::LinkRef;

 public:
  CMarkdownBlock(CMarkdown *parent);
  CMarkdownBlock(CMarkdownBlock *parent, CMarkdownTagType type);

 ~CMarkdownBlock();

  CMarkdownBlock *parent() const { return parent_; }

  CMarkdown *markdown() const { return (parent_ ? parent_->markdown() : markdown_); }

  CMarkdownTagType blockType() const { return type_; }

  void addBlock(CMarkdownBlock *block);

  void addLine(const Line &line);

  void appendLine(const QString &line);

  void preProcess();

  QString process(CMarkdown::Format format);

  QString processLines(CMarkdown::Format format);

  QString processList(CMarkdownTagType type, const ListData &list, CMarkdown::Format format);

  bool isContinuationLine(const QString &str) const;

  bool isSetTextLine(const QString &str, CMarkdownTagType &type) const;

  bool isIndentLine(const QString &str, int &n) const;

  bool isFormatLine(const QString &str) const;
  bool isFormatChar(const QString &str, int i) const;

  bool isStartCodeFence(const QString &str, CodeFence &fence) const;
  bool isEndCodeFence(const QString &str, const CodeFence &fence) const;

  bool isHtmlLine(const QString &str) const;

  bool isLinkReference(const QString &str, LinkRef &link) const;

  bool isBlockQuote(const QString &str, QString &quote) const;

  bool isUnorderedListLine(const QString &str, ListData &list) const;
  bool isOrderedListLine  (const QString &str, ListData &list) const;

  bool isTableLine(const QString &str) const;

  void parseTableLine(const QString &str);

  void parseLine(const QString &line);

  QString replaceEmbeddedStyles(const QString &str, bool code, CMarkdown::Format format) const;

  QString imageSrc(const QString &filename) const;

  void splitLinkRef(const QString &str, QString &href, QString &title) const;

  QString replaceHtmlChars(const QString &str) const;

  bool isAutoLink(const QString &str, int &i, QString &ref) const;

  bool isASCIIPunct(const QChar &c) const;

  bool getLine(LineData &line);
  void ungetLine();

  CMarkdownBlock *startBlock(CMarkdownTagType type);

  void addBlockLine(const QString &line, bool brk=false);
  void appendBlockLine(const QString &line);

  void flushBlocks();

  CMarkdownBlock *endBlock();

  void print(int depth=0) const;

  QString toText(CMarkdown::Format format) const;

  QString anchorText(const QString &ref, const QString &title, const QString &str,
                     CMarkdown::Format format) const;

  QString emphasisText(const QString &text, CMarkdown::Format format) const;
  QString boldText    (const QString &text, CMarkdown::Format format) const;
  QString strikeText  (const QString &text, CMarkdown::Format format) const;

  QString startTag(CMarkdownTagType type, CMarkdown::Format format) const;
  QString endTag  (CMarkdownTagType type, CMarkdown::Format format) const;
  QString fullTag (CMarkdownTagType type, CMarkdown::Format format) const;

  QString htmlStyle(CMarkdownTagType type) const;

  QString ttyStartStyle(CMarkdownTagType type) const;
  QString ttyEndStyle  (CMarkdownTagType type) const;

 private:
  using Blocks = std::vector<CMarkdownBlock *>;

  CMarkdown*       markdown_  { nullptr };
  CMarkdownBlock*  parent_    { nullptr };
  CMarkdownTagType type_      { CMarkdownTagType::ROOT };
  Lines            lines_;
  Blocks           blocks_;
  QString          processedText_;
  bool             processed_ { false };

  mutable int currentLine_ { 0 };

  CMarkdownBlock *rootBlock_    { nullptr };
  CMarkdownBlock *currentBlock_ { nullptr };
};

//------

namespace CMarkdownParse {
  bool isATXHeader(const QString &str, CMarkdownBlock::ATXData &atxData, int &istart, int &iend);

  bool isLinkReference(const QString &str, CMarkdown::LinkRef &linkRef, int &istart, int &iend);

  bool isRule(const QString &str, int &istart, int &iend);

  int parseSurroundText(const QString &str, int &i, QString &str1, int &start1);
  int parseSurroundText(const QString &str, int &i, const QChar &c, QString &str1, int &start1);

  bool isASCIIPunct(const QChar &c);

  bool isBlankLine(const QString &str);

  int skipSpace(const QString &str, int &i);
  int backSkipSpace(const QString &str, int &i);

  int skipChar(const QString &str, int &i, const QChar &c);
  int backSkipChar(const QString &str, int &i, const QChar &c);
}

#endif
