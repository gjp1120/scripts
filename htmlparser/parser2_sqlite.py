#! /bin/env python

import lxml
import lxml.html
import os, sys

import json
import re

import sqlite3

PREFIX = os.path.expanduser("~/Public/www.yinbiav.com")
TABLES = {}
handle = sqlite3.connect("database.db")
cursor = handle.cursor()

def write_out_result(table, title, area, date, img, links):
  cursor.execute("insert into %s values (?, ?, ?, ?, ?)" % table, (title, area,
                                                                   date, img, ",".join(links)))

for filename in sys.argv[1:]:
  # FILE1
  with open(filename) as fd:
    etr1 = lxml.etree.HTML(fd.read().decode("gb18030"))
    #desc = "".join([s.encode("utf8") for s in etr1.xpath("//div[@class='list']").pop().itertext()])
    table = etr1.xpath("//div[@class='list']/table").pop()
    img   = table.xpath("tr[1]/td/img/@src").pop()
    content_type = table.xpath("tr[1]/td[1]/text()").pop()[5:]
    name  = table.xpath("tr[2]/td/strong/text()").pop()
    area  = table.xpath("tr[3]/td/text()").pop()[5:]
    date  = table.xpath("tr[4]/td/text()").pop()
    player_filename = etr1.xpath("//ul/li/a/@href").pop()

  # Create Type
  if content_type not in TABLES:
    handle.execute("create table %s (TITLE text, AREA text, DATE text, IMG text, LINKS text)" % content_type)
    TABLES[content_type] = 1

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
    links = json.loads(s)[0][1]
  except UnicodeDecodeError as e:
    sys.stderr.write("error! parse %s failed\n" % player_data_filename)
    print >>sys.stderr, e
    links = []

  # Write Out
  write_out_result(content_type, name, area, date, img, links)

cursor.close()
handle.commit()
handle.close()
