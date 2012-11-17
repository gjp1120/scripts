#! /bin/env python

import os,sys
import gdbm

path, width, height = sys.argv[1:3]
maxln = height

ext = path.split('.')[-1].lower()

if ext in ('mp4', 'mkv', 'rm', 'rmvb', 'avi', 'flv', 'f4v')
  db = gdbm.open(os.path.expanduser('~/.cache/ranger/preview_cache.dbm'), 'c')
  if db.haskey(path)
    mtime, content = db[path]
    real_mtime = os.stat(path).st_mtime
  else
    
elif ext in ('7z', 'rar', 'gz', 'xz', 'bz2', 'tgz', 'rpm', 'tbz2')
  os.system('als "%s"' % path)

elif ext in ('txt')
  os.system('preview_r')
