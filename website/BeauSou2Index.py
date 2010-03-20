# -*- coding: utf-8 -*-
#html-juergen greets doxygaehn

from BeautifulSoup import BeautifulSoup
import re,os

def Links(ifile):
  doxrelpath=os.path.dirname(os.path.dirname(ifile))
  if doxrelpath.startswith('html/'):
    doxrelpath=re.sub(r'html\/','',doxrelpath,re.I)
  if not doxrelpath.endswith('/'):
    doxrelpath=doxrelpath+'/'
  ih=open(ifile,'r')
  lines=ih.readlines()
  ih.close()
  soup=BeautifulSoup(''.join(lines))
  anchors=soup('a')
  for anch in anchors:
    namespace=[]
    if anch.has_key('href'):
      yield anch

def CreateClassIndex(ifile, ofile):
  classlist=[]
  doxrelpath=os.path.dirname(os.path.dirname(ifile))  
  if doxrelpath.startswith('html/'):
    doxrelpath=re.sub(r'html\/','',doxrelpath,re.I)
  if not doxrelpath.endswith('/'):
    doxrelpath=doxrelpath+'/'
  for link in Links(ifile):
    m=re.search('(.*class(ost|geom).*)',link.get('href'),re.I)
    if m:
      m2=re.search(r'class([^_]*)',m.group(0))
      if m2:
        namespace=[]        
        namespace.append(m2.group(1))
        p=re.compile(r'([^_]+)_1_1')
        res=p.findall(m.group(0))
        for item in res:
          if not re.search(r'class',item):
            namespace.append(item.replace(' &gt;','>').replace('&lt; ','<'))
        namespace.append(link.string.replace(' &gt;','>').replace('&lt; ','<'))
        mylink={}
        mylink['::'.join(namespace)]=m.group(0)
        classlist.append(mylink)
  oh=open(ofile,'w')       
  for myclass in classlist:
    for key,value in myclass.iteritems():
      path_prefix=doxrelpath+'html/'+value+'\n'
      oh.write(key+','+path_prefix) 
  
def CreateExampleIndex(ifile, ofile):
  oh=open(ofile, 'w')  
  for link in Links(ifile):
      if link.get('class')=='el':
         oh.write(link.string+','+link.get('href')+'\n') 
