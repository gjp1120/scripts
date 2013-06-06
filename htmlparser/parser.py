#! /bin/env python

import libxml2
import os, sys

import json
import re

PREFIX = os.path.expanduser("~/Public/www.yinbiav.com")

print "<html>"
print """<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf8" />
</head>"""
print "<body>"

ctxt = libxml2.htmlNewParserCtxt()

for filename in sys.argv[1:]:
  f = ctxt.htmlCtxtReadFile(filename, "gb18030", libxml2.HTML_PARSE_NOERROR)

  describe = f.xpathEval("//div[@class='list']").pop().get_content()
  img = f.xpathEval("//div[@class='list']/table/tr[1]/td/img/@src").pop().get_content()

  player_filename = f.xpathEval("//ul/li/a/@href").pop().get_content()
  f.freeDoc()

  player_f = ctxt.htmlCtxtReadFile(os.path.join(PREFIX, player_filename[1:]), "gb18030", libxml2.HTML_PARSE_NOERROR)
  player_data_filename = player_f.xpathEval("//td[@align='center']/script/@src").pop().get_content()
  player_f.freeDoc()

  s = re.findall("\[\[.*\]\]\]", open(os.path.join(PREFIX, player_data_filename[1:])).read()).pop()
  s = s.replace("'", '"')
  try:
    arry = json.loads(s)[0][1]
    links = [l.encode('utf8') for l in arry]
  except:
    continue

  print "<p>"
  print describe
  print "</p>"

  print "<p>"
  for link in links:
    print "%s<br/>" % link
  print "</p>"

  print "<img src=\"%s\"/>\n" % img[1:]

  del f, describe, img, player_data_filename, player_f, player_filename, s, arry, links

print "</body>\n</html>"

