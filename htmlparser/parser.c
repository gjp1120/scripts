#include <libxml/xpath.h>
#include <libxml/HTMLparser.h>

int main(int argc, char **argv)
{
  htmlParserCtxtPtr ctxt = htmlNewParserCtxt();
  htmlDocPtr f = htmlCtxtReadFile(ctxt, "./index155.html","gb18030", HTML_PARSE_NOERROR);
  xmlXPathContextPtr p = xmlXPathNewContext(f);
  puts(xmlXPathCastToString(xmlXPathEval("//div[@class = 'list']",p)));

  return 0;
}
