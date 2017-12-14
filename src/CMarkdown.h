#ifndef CMarkdown_H
#define CMarkdown_H

#include <QString>
#include <vector>
#include <map>

class CMarkdownBlock;

class CMarkdown {
 public:
  struct LinkRef {
    QString ref;
    QString dest;
    QString title;
  };

  typedef std::map<QString,LinkRef> Links;

 public:
  CMarkdown();

  bool isDebug() const { return debug_; }
  void setDebug(bool d);

  QString processFile(const QString &filename);
  QString processText(const QString &str);

  void addLink(const LinkRef &link);
  bool getLink(const QString &ref, LinkRef &link) const;

 private:
  bool readLine(QString &line);

 private:
  typedef std::vector<CMarkdownBlock *> Blocks;

  QString str_ {   }; // input string
  int     len_ { 0 }; // input string length
  int     pos_ { 0 }; // input string position

  bool            debug_     { false };
  CMarkdownBlock *rootBlock_ { nullptr };
  Links           links_;
};

//-------

class CMarkdownBlock {
 public:
  enum class BlockType {
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
  };

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
    BlockType type;
    QString   text;
  };

 public:
  typedef std::vector<Line>  Lines;
  typedef CMarkdown::LinkRef LinkRef;

 public:
  CMarkdownBlock(CMarkdown *parent);
  CMarkdownBlock(CMarkdownBlock *parent, BlockType type);

 ~CMarkdownBlock();

  CMarkdownBlock *parent() const { return parent_; }

  CMarkdown *markdown() const { return (parent_ ? parent_->markdown() : markdown_); }

  BlockType blockType() const { return type_; }

  void addBlock(CMarkdownBlock *block);

  void addLine(const Line &line);

  void appendLine(const QString &line);

  void preProcess();

  QString process();

  QString processLines();

  QString processList(BlockType type, const ListData &list);

  bool isContinuationLine(const QString &str) const;

  bool isSetTextLine(const QString &str, CMarkdownBlock::BlockType &type) const;

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

  QString replaceEmbeddedStyles(const QString &str) const;

  void splitLinkRef(const QString &str, QString &href, QString &title) const;

  QString replaceHtmlChars(const QString &str) const;

  bool isAutoLink(const QString &str, int &i, QString &ref) const;

  bool isASCIIPunct(const QChar &c) const;

  bool getLine(LineData &line);
  void ungetLine();

  CMarkdownBlock *startBlock(BlockType type);

  void addBlockLine(const QString &line, bool brk=false);
  void appendBlockLine(const QString &line);

  void flushBlocks();

  CMarkdownBlock *endBlock();

  void print(int depth=0) const;

  QString toHtml() const;

  static QString tagName(BlockType type);

  static bool isSingleLineType(BlockType type);
  static bool isRecurseType   (BlockType type);

 private:
  typedef std::vector<CMarkdownBlock *> Blocks;

  CMarkdown*      markdown_ { nullptr };
  CMarkdownBlock* parent_ { nullptr };
  BlockType       type_ { BlockType::ROOT };
  Lines           lines_;
  Blocks          blocks_;
  QString         html_;
  bool            processed_ { false };

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
