# -*- coding: utf-8 -*-
# class to extend inlinepatterns class to handle  doxygen references by
#inserting links(<a>) created from a previuosly compiled list of classnames
#within ost and their relative location within the source tree
#
#Author Juergen Haas
import markdown
import re



doxBRK=r'(dox|ex)\[(.*?)(\|.*?)?\]'
DOXYREF_RE= doxBRK

class DoxyRefPattern(markdown.inlinepatterns.LinkPattern):
    """ Match to a stored doxygen reference and return link element. """
    def __init__ (self, pattern, markdown_instance=None, doxyref={}, example_dict={}):
        """
        Create an instant of an inline pattern.

        Keyword arguments:

        * pattern: A regular expression that matches a pattern

        """
        self.pattern = pattern
        self.doxyref = doxyref
        self.example_dict=example_dict
        self.compiled_re = re.compile("^(.*?)%s(.*?)$" % pattern, re.DOTALL)
        # Api for Markdown to pass safe_mode into instance
        self.safe_mode = False
        if markdown_instance:
            self.markdown = markdown_instance

    def handleMatch(self, m):
       if m.group(2)=='dox':
         text = m.group(3)
         if m.group(4):
           if m.group(4).find('\|'):
             linktext=m.group(4).replace('|','')
           else:
             linktext=text
         else:
           linktext=text
         if text in self.doxyref:
             href = self.doxyref[text]
         else:
             print 'doxyref  not found for', text
             return
         title= None
         return self.makeTag(href, title, linktext)
       else:
         ref=m.group(3)
         text=ref
         if m.group(4):
           text=m.group(4)[1:]
         return self.makeTag('dox/html/'+self.example_dict.get(ref, ''), None, text) 

    def makeTag(self, href, title, text):
        el = markdown.etree.Element('a')

        el.set('href', self.sanitize_url(href))

        el.text = text
        return el
