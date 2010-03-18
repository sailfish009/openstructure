# -*- coding: utf-8 -*-
#Authors Marco Biasini, Juergen Haas
import markdown
import os, re
import codecs,shutil
import doxypattern
from xml import etree
from optparse import OptionParser
from BeauSou2Index import *


#please use this to flag a mkdown document as incomplete:
#> <div style="height:70px;background-color:#DD0000;border-bottom:1px solid #DDDDDD;border-top:1px solid #DDDDDD;">This document needs love!
#
#default path to classes.html
#please omit the leading ''./''
doxrelpath='html/dox/'
CLASSES_PATH=doxrelpath+'html/classes.html'

p=OptionParser()
p.add_option('--output_dir', '-o', help='output directory', 
             default='html')
p.add_option('--input_dir', '-i', help='input directory', default='raw')
p.add_option('--base_url', '-b', 
             help='base url to be used for all relative links. if not specified, '+
                  'will be set to the output directory')
p.add_option('--doxy_ref_file', '-d', help='file with compiled classnames, '+
                  'and corresponding doxygen page', default='index.txt')
p.add_option('--doxy_classfile','-c',help='file, where doxygen store the'+
             'alphabetical index',default='%s'%(CLASSES_PATH))
p.add_option('--run_doxygen','-r',help='run doxygen with Doxyfile given as'+
             'argument')
opts, args=p.parse_args()

if os.path.exists('../modules/scratch') or \
   os.path.exists('../modules/dng_scratch'):
  print '\nERROR ...exiting!\n\nPlease generate webpage from public checkout\n'
  os._exit(-1)
if not opts.base_url:
  opts.base_url='file://%s' % os.path.abspath(opts.output_dir)
scaffold_file=codecs.open('raw/scaffold.html', mode='r', 
                           encoding='utf8')
SCAFFOLD=scaffold_file.read()

def run_doxygen(doxyfile):
    if os.path.exists(doxyfile):
      print '\nrunning doxygen now...\n'
      cmd='doxygen %s' %(doxyfile)
      output=os.popen(cmd)
    else:
      print '\nDoxygen input file not found!\nPlease provide a valid doxygen '\
            'input file specifying --run_doxygen YOURFILE\n'
      os._exit(-1)
    
def load_doxy_refs():
  doxy_file=opts.doxy_ref_file
  if not os.path.isfile(doxy_file):
    print '\nWarning: Index file with doxygen classname references'\
          ' not found!\n         Use -d to specify filename'\
          '(default=index.txt)\n'
    return
  doxy_refs=codecs.open(doxy_file, mode="r",
                              encoding="utf8")
  lines=doxy_refs.readlines()

  for line in lines:
    fields=[]
    if re.search(',',line):
      fields=line.split(',')
    if len(fields)==2:
      doxy_dict[fields[0]]=fields[1]
  for line in open('examples_index.txt'):
    key, val=line.split(',')
    example_dict[key.strip()]=val.strip()

def mkdown_to_html(opts, dirname, names):
#  base_dir=os.path.relpath(dirname, opts.input_dir)
  if dirname==opts.input_dir: 
    base_dir=dirname[len(opts.input_dir):]
  else:
    base_dir=dirname[len(opts.input_dir)+1:]
    
  out_dir=os.path.join(opts.output_dir, base_dir)
  if out_dir.find('.svn')!=-1:
    return
  print out_dir
  if not os.path.exists(out_dir):
    os.mkdir(out_dir)
  for name in names:
    if name.endswith('.mkdown'):
      print '*- converting', name
      base_out_name=os.path.splitext(name)[0]+'.html'
      out_name=os.path.join(out_dir, base_out_name)            
      full_name=os.path.join(dirname, name)
      input_file=codecs.open(os.path.join(dirname, name), mode="r", 
                             encoding="utf8")
      text=input_file.read()
      md=markdown.Markdown(extensions=['toc2', 'codehilite', 'meta','tables'])
      md.inlinePatterns["doxylink"] = \
                      doxypattern.DoxyRefPattern(doxypattern.DOXYREF_RE,
                                              md,
                                              doxy_dict, example_dict)
      md.doc_url=os.path.join(base_dir, base_out_name)
      html=md.convert(text)
      meta=md.Meta
      toc=etree.ElementTree.tostring(md.TOC)
      title=''
      if 'title' in meta.keys():
        title=meta['title'][0]

      out_file=codecs.open(out_name, mode='w', encoding='utf8')
      out_file.write(SCAFFOLD % { 'CONTENT' : html, 'TITLE' : title, 
                                  'SITE_NAV' : toc, 'BASE' : opts.base_url})

def copy_images(opts, dirname, names):
   if dirname==opts.input_dir: 
     base_dir=dirname[len(opts.input_dir):]
   else:
     base_dir=dirname[len(opts.input_dir)+1:]
   out_dir=os.path.join(opts.output_dir, base_dir)
   for name in names:
     if name.endswith('.jpg') or name.endswith('.png'):
       shutil.copy(os.path.join(dirname,name), os.path.join(out_dir,name))

if not os.path.exists(opts.output_dir):
  os.mkdir(opts.output_dir)
if not os.path.exists(os.path.join(opts.output_dir, 'css')):
  os.mkdir(os.path.join(opts.output_dir, 'css'))
shutil.copy(os.path.join(opts.input_dir, 'css/ost.css'),
            os.path.join(opts.output_dir, 'css'))
if opts.run_doxygen:
  run_doxygen(opts.run_doxygen)
  if os.path.exists(opts.doxy_ref_file):
    os.unlink(opts.doxy_ref_file)
if not os.path.exists(opts.doxy_classfile):
  print 'Warning: Could not find the alphabetical index file generated\n'\
        'by doxygen (classes.html). Please specify with -c'\
        '(default=''%s'')' %(CLASSES_PATH)
  os._exit(-2)
else:
  print 'Found doxygen class file:',opts.doxy_classfile

doxy_dict={}
example_dict={}
#now create the index file listing the classnames and the corresponding
#doxygen html files for resolving the dox[ost::mol::BondHandle] mkdown tags
if not os.path.exists(opts.doxy_ref_file):
  CreateClassIndex(opts.doxy_classfile, opts.doxy_ref_file)
  CreateExampleIndex('html/dox/html/examples.html', 'examples_index.txt')
load_doxy_refs()
os.path.walk(opts.input_dir, mkdown_to_html, opts)
os.path.walk(opts.input_dir, copy_images, opts)