#! /bin/env python

import lxml
import lxml.html
import os, sys

import json
import re

PREFIX = os.path.expanduser("~/Public/www.yinbiav.com")
FILEH  = {}
HTML_HEADER = \
"""<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf8" />
</head>
<body>
"""

def write_out_result(handle, desc, img, links):
  # Write Out
  # Desc
  handle.write("<p>\n")
  handle.write(desc)
  handle.write("</p>\n")
  # Links
  handle.write("<p>\n")
  for link in links:
    handle.write("%s<br/>\n" % link)
  handle.write("</p>\n")
  # Image
  handle.write("<img src=\"%s\"/>\n" % img[1:])

def init_file_handle(fh):
  fh.write(HTML_HEADER)

for filename in sys.argv[1:]:
  # FILE1
  with open(filename) as fd:
    etr1 = lxml.etree.HTML(fd.read().decode("gb18030"))
    desc = "".join([s.encode("utf8") for s in etr1.xpath("//div[@class='list']").pop().itertext()])
    img  = etr1.xpath("//div[@class='list']/table/tr[1]/td/img/@src").pop()
    content_type    = etr1.xpath("//div[@class='list']/table/tr[1]/td[1]/text()").pop()[5:]
    player_filename = etr1.xpath("//ul/li/a/@href").pop()

  # Create Type
  if content_type not in FILEH:
    FILEH[content_type] = open("%s.html" % content_type, "w")
    init_file_handle(FILEH[content_type])

  # FILE2
  with open(os.path.join(PREFIX, player_filename[1:])) as fd:
    etr_player = lxml.etree.HTML(fd.read().decode("gb18030"))
    player_data_filename = etr_player.xpath("//td[@align='center']/script/@src").pop()

  # FILE3
  with open(os.path.join(PREFIX, player_data_filename[1:])) as fd:
    s = re.findall("\[\[.*\]\]\]", fd.read()).pop()
    s = s.replace("'", '"').decode("gb18030")

  # Parse Json
  try:
    arry = json.loads(s)[0][1]
    links = [l.encode('utf8') for l in arry]
  except UnicodeDecodeError as e:
    sys.stderr.write("error! parse %s failed\n" % player_data_filename)
    print >>sys.stderr, e
    links = []

  # Write Out
  write_out_result(FILEH[content_type], desc, img, links)

# Parse End
for key in FILEH:
  FILEH[key].write("</body>\n</html>\n")
  FILEH[key].close()

