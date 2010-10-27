#!/usr/bin/env python
import sys
out=[]
version=sys.argv[1].split('.')
if len(version)==2:
  major, minor, patch=(int(version[0]), int(version[1]), 0)
else:
  major, minor, patch=(int(version[0]), int(version[1]), int(version[2]))
for line in open('modules/config/version.hh'):
  if line.startswith('#define OST_VERSION_MAJOR'):
    out.append('#define OST_VERSION_MAJOR %d\n' % major)
  elif line.startswith('#define OST_VERSION_MINOR'):
    out.append('#define OST_VERSION_MINOR %d\n' % minor)
  elif line.startswith('#define OST_VERSION_PATCH'):
    out.append('#define OST_VERSION_PATCH %d\n' % patch)
  elif line.startswith("#define OST_VERSION_STRING"):
    out.append('#define OST_VERSION_STRING "%d.%d.%d"\n' % (major,minor,patch))
  else:
    out.append(line)

open('modules/config/version.hh', 'w').write(''.join(out))