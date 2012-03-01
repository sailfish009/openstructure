import csv
import re
import math
from ost import stutil
import itertools
import operator
import cPickle
from ost import LogError, LogWarning, LogInfo, LogVerbose

def MakeTitle(col_name):
  return col_name.replace('_', ' ')

def IsStringLike(value):
  if isinstance(value, TableCol) or isinstance(value, BinaryColExpr):
    return False
  try:
    value+''
    return True
  except:
    return False

def IsNullString(value):
  value=value.strip().upper()
  return value in ('', 'NULL', 'NONE', 'NA')

def IsScalar(value):
  if IsStringLike(value):
    return True
  try:
    if isinstance(value, TableCol) or isinstance(value, BinaryColExpr):
      return False
    iter(value)
    return False
  except:
    return True

def GuessColumnType(iterator):
  empty=True
  possibilities=set(['bool', 'int', 'float'])
  for ele in iterator:
    str_ele=str(ele).upper()
    if IsNullString(str_ele):
      continue
    empty=False
    if 'int' in possibilities:
      try:
        int(str_ele)
      except ValueError:
        possibilities.remove('int')

    if 'float' in possibilities:
      try:
        float(str_ele)
      except ValueError:
        possibilities.remove('float')
    if 'bool' in possibilities:
      if str_ele not in set(['YES', 'NO', 'TRUE', 'FALSE']):
        possibilities.remove('bool')

    if len(possibilities)==0:
      return 'string'
  if len(possibilities)==2:
    return 'int'
  if empty:
    return 'string'
  # return the last element available
  return possibilities.pop()

class BinaryColExpr:
  def __init__(self, op, lhs, rhs):
    self.op=op
    self.lhs=lhs
    self.rhs=rhs
    if IsScalar(lhs):
      self.lhs=itertools.cyle([self.lhs])
    if IsScalar(rhs):
      self.rhs=itertools.cycle([self.rhs])
  def __iter__(self):
    for l, r in zip(self.lhs, self.rhs):
      if l!=None and r!=None:
        yield self.op(l, r)
      else:
        yield None
  def __add__(self, rhs):
    return BinaryColExpr(operator.add, self, rhs)

  def __sub__(self, rhs):
    return BinaryColExpr(operator.sub, self, rhs)

  def __mul__(self, rhs):
    return BinaryColExpr(operator.mul, self, rhs)

class TableCol:
  def __init__(self, table, col):
    self._table=table
    if type(col)==str:
      self.col_index=self._table.GetColIndex(col)
    else:
      self.col_index=col

  def __iter__(self):
    for row in self._table.rows:
      yield row[self.col_index]

  def __len__(self):
    return len(self._table.rows)

  def __getitem__(self, index):
    return self._table.rows[index][self.col_index]

  def __setitem__(self, index, value):
    self._table.rows[index][self.col_index]=value
  
  def __add__(self, rhs):
    return BinaryColExpr(operator.add, self, rhs)

  def __sub__(self, rhs):
    return BinaryColExpr(operator.sub, self, rhs)

  def __mul__(self, rhs):
    return BinaryColExpr(operator.mul, self, rhs)
  def __div__(self, rhs):
    return BinaryColExpr(operator.div, self, rhs)


class Table(object):
  """
  
  The table class provides convenient access to data in tabular form. An empty 
  table can be easily constructed as follows
  
  .. code-block:: python
  
    tab=Table()
    
  If you want to add columns directly when creating the table, column names
  and *column types* can be specified as follows
  
  .. code-block:: python
  
    tab=Table(['nameX','nameY','nameZ'], 'sfb')
    
  this will create three columns called nameX, nameY and nameZ of type string,
  float and bool, respectively. There will be no data in the table and thus,
  the table will not contain any rows.
  
  The following *column types* are supported:
  
  ======= ========
  name     abbrev
  ======= ========
  string     s
  float      f
  int        i
  bool       b
  ======= ========
  
  If you want to add data to the table in addition, use the following:
  
  .. code-block:: python
  
    tab=Table(['nameX','nameY','nameZ'],
              'sfb',
              nameX=['a','b','c'],
              nameY=[0.1, 1.2, 3.414],
              nameZ=[True, False, False])
              
  if values for one column is left out, they will be filled with NA, but if
  values are specified, all values must be specified (i.e. same number of
  values per column)
    
  """

  SUPPORTED_TYPES=('int', 'float', 'bool', 'string',)
  
  
  def __init__(self, col_names=None, col_types=None, **kwargs):
    self.col_names=col_names
    self.comment=''
    self.name=''
    
    self.col_types = self._ParseColTypes(col_types)
    self.rows=[]    
    if len(kwargs)>=0:
      if not col_names:
        self.col_names=[v for v in kwargs.keys()]
      if not self.col_types:
        self.col_types=['string' for u in range(len(self.col_names))]
      if len(kwargs)>0:
        self._AddRowsFromDict(kwargs)

  @staticmethod
  def _ParseColTypes(types, exp_num=None):
    if types==None:
      return None
    
    short2long = {'s' : 'string', 'i': 'int', 'b' : 'bool', 'f' : 'float'}
    allowed_short = short2long.keys()
    allowed_long = short2long.values()
    
    type_list = []
    
    # string type
    if IsScalar(types):
      if type(types)==str:
        types = types.lower()
        
        # single value
        if types in allowed_long:
          type_list.append(types)
        elif types in allowed_short:
          type_list.append(short2long[types])
        
        # comma separated list of long or short types
        elif types.find(',')!=-1:
          for t in types.split(','):
            if t in allowed_long:
              type_list.append(t)
            elif t in allowed_short:
              type_list.append(short2long[t])
            else:
              raise ValueError('Unknown type %s in types %s'%(t,types))
        
        # string of short types
        else:
          for t in types:
            if t in allowed_short:
              type_list.append(short2long[t])
            else:
              raise ValueError('Unknown type %s in types %s'%(t,types))
      
      # non-string type
      else:
        raise ValueError('Col type %s must be string or list'%types)
    
    # list type
    else:
      for t in types:
        # must be string type
        if type(t)==str:
          t = t.lower()
          if t in allowed_long:
            type_list.append(t)
          elif t in allowed_short:
            type_list.append(short2long[t])
          else:
            raise ValueError('Unknown type %s in types %s'%(t,types))
        
        # non-string type
        else:
          raise ValueError('Col type %s must be string or list'%types)
    
    if exp_num:
      if len(type_list)!=exp_num:
        raise ValueError('Parsed number of col types (%i) differs from ' + \
                         'expected (%i) in types %s'%(len(type_list),exp_num,types))
      
    return type_list

  def SetName(self, name):
    self.name = name
    
  def GetName(self):
    return self.name

  def _Coerce(self, value, ty):
    if value=='NA' or value==None:
      return None
    if ty=='int':
      return int(value)
    if ty=='float':
      return float(value)
    if ty=='string':
      return str(value)
    if ty=='bool':
      if isinstance(value, str) or isinstance(value, unicode):
        if value.upper() in ('FALSE', 'NO',):
          return False
        return True
      return bool(value)
    raise ValueError('Unknown type %s' % ty)

  def GetColIndex(self, col):
    '''
    Returns the column index for the column with the given name.

    :raises: ValueError if no column with the name is found
    '''
    if col not in self.col_names:
      raise ValueError('Table has no column named "%s"' % col)
    return self.col_names.index(col)
  
  def GetColNames(self):
    '''
    Returns a list containing all column names.
    '''
    return self.col_names
  
  def HasCol(self, col):
    '''
    Checks if the column with a given name is present in the table.
    '''
    return col in self.col_names
  
  def __getitem__(self, k):
    if type(k)==int:
      return TableCol(self, self.col_names[k])
    else:
      return TableCol(self, k)

  def __setitem__(self, k, value):
    col_index=k
    if type(k)!=int:
      col_index=self.GetColIndex(k)
    if IsScalar(value):
      value=itertools.cycle([value])
    for r, v in zip(self.rows, value):
      r[col_index]=v

  def ToString(self, float_format='%.3f', int_format='%d', rows=None):
    '''
    Convert the table into a string representation.
    The output format can be modified for int and float type columns by
    specifying a formatting string for the parameters 'float_format' and
    'int_format'.
    The option 'rows' specify the range of rows to be printed. The parameter
    must be a iterable containing two elements, e.g. [start_row, end_row].
    '''
    widths=[len(cn) for cn in self.col_names]
    sel_rows=self.rows
    if rows:
      sel_rows=self.rows[rows[0]:rows[1]]
    for row in sel_rows:
      for i, (ty, col) in enumerate(zip(self.col_types, row)):
        if col==None:
          widths[i]=max(widths[i], len('NA'))
        elif ty=='float':
          widths[i]=max(widths[i], len(float_format % col))
        elif ty=='int':
          widths[i]=max(widths[i], len(int_format % col))
        else:
          widths[i]=max(widths[i], len(str(col)))
    s=''
    if self.comment:
      s+=''.join(['# %s\n' % l for l in self.comment.split('\n')])
    total_width=sum(widths)+2*len(widths)
    for width, col_name in zip(widths, self.col_names):
      s+=col_name.center(width+2)
    s+='\n%s\n' % ('-'*total_width)
    for row in sel_rows:
      for width, ty, col in zip(widths, self.col_types, row):
        cs=''
        if col==None:
          cs='NA'.center(width+2)
        elif ty=='float':
          cs=(float_format % col).rjust(width+2)
        elif ty=='int':
          cs=(int_format % col).rjust(width+2)
        else:
          cs=' '+str(col).ljust(width+1)
        s+=cs
      s+='\n'
    return s

  def __str__(self):
    return self.ToString()
  
  def _AddRowsFromDict(self, d, merge=False):
    # get column indices
    idxs = [self.GetColIndex(k) for k in d.keys()]
    
    # convert scalar values to list
    old_len = None
    for k,v in d.iteritems():
      if IsScalar(v):
        d[k] = [v]
      else:
        if not old_len:
          old_len = len(v)
        elif old_len!=len(v):
          raise ValueError("Cannot add rows: length of data must be equal " + \
                           "for all columns in %s"%str(d))
    
    # convert column based dict to row based dict and create row and add data
    for i,data in enumerate(zip(*d.values())):
      new_row = [None for a in range(len(self.col_names))]
      for idx,v in zip(idxs,data):
        new_row[idx] = self._Coerce(v, self.col_types[idx])
        
      # partially overwrite existing row with new data
      if merge:
        merge_idx = self.GetColIndex(merge)
        added = False
        for i,r in enumerate(self.rows):
          if r[merge_idx]==new_row[merge_idx]:
            for j,e in enumerate(self.rows[i]):
              if new_row[j]==None:
                new_row[j] = e
            self.rows[i] = new_row
            added = True
            break
          
      # if not merge or merge did not find appropriate row
      if not merge or not added:
        self.rows.append(new_row)
      

  def AddRow(self, data, merge=None):
    """
    Add a row to the table.
    
    *row* may either be a dictionary or a list-like object.
    In the case of a dictionary the keys in the dictionary must match the column
    names. Columns not found in the dict will be initialized to None.
    Alternatively, if data is a list-like object, the row is initialized from
    the values in data. The number of items in data must match the number of
    columns in the table. A :class:`ValuerError` is raised otherwise. The values
    are added in the order specified in the list, thus, the order of the data
    must match the columns.
    
    *merge* looks for an existing row and adds the data there instead of
    appending a new row. If merge is set to an existing column name, all
    existing rows are searched for the first row where the value of column with
    the name specified in merge is equal to the value of the specified column in
    the new data. If such a row is found, all existing data of this row is 
    overwritten with the new data. If no matching row is found, a new row is 
    appended to the table.
    """
    if type(data)==dict:
      self._AddRowsFromDict(data, merge)
    else:
      if len(data)!=len(self.col_names):
        msg='data array must have %d elements, not %d'
        raise ValueError(msg % (len(self.col_names), len(self.data)))
      new_row = [self._Coerce(v, t) for v, t in zip(data, self.col_types)]
      
      # fully overwrite existing row with new data
      if merge:
        merge_idx = self.GetColIndex(merge)
        added = False
        for i,r in enumerate(self.rows):
          if r[merge_idx]==new_row[merge_idx]:
            self.rows[i] = new_row
            added = True
            break
      
      # if not merge or merge did not find appropriate row
      if not merge or not added:
        self.rows.append(new_row)

  def RemoveCol(self, col):
    """
    Remove column with the given name from the table
    """
    idx = self.GetColIndex(col)
    del self.col_names[idx]
    del self.col_types[idx]
    for row in self.rows:
      del row[idx]

  def AddCol(self, col_name, col_type, data=None):
    """
    Add a column to the right of the table.
    
    .. code-block:: python
    
      tab=Table(['x'], 'f', x=range(5))
      tab.AddCol('even', 'bool', itertools.cycle([True, False]))
      print tab
    
    will produce the table
    
    ====  ====
    x     even
    ====  ====
      0   True
      1   False
      2   True
      3   False
      4   True
    ====  ====
      
    if data is a constant instead of an iterable object, it's value
    will be written into each row 
    """
    col_type = self._ParseColTypes(col_type, exp_num=1)[0]
    self.col_names.append(col_name)
    self.col_types.append(col_type)
    if IsScalar(data):
      for row in self.rows:
        row.append(data)
    else:
      for row, d in zip(self.rows, data):
        row.append(d)



  def Filter(self, *args, **kwargs):
    """
    Returns a filtered table only containing rows matching all the predicates 
    in kwargs and args For example,
    
    .. code-block:: python
    
      tab.Filter(town='Basel')
    
    will return all the rows where the value of the column "town" is equal to 
    "Basel". Several predicates may be combined, i.e.
    
    .. code-block:: python
    
      tab.Filter(town='Basel', male=True)
      
    will return the rows with "town" equal to "Basel" and "male" equal to true.
    args are unary callables returning true if the row should be included in the
    result and false if not.
    """
    filt_tab=Table(self.col_names, self.col_types)
    for row in self.rows:
      matches=True
      for func in args:
        if not func(row):
          matches=False
          break
      for key, val in kwargs.iteritems():
        if row[self.GetColIndex(key)]!=val:
          matches=False
          break
      if matches:
        filt_tab.AddRow(row)
    return filt_tab

  @staticmethod
  def _LoadOST(stream_or_filename):
    fieldname_pattern=re.compile(r'(?P<name>[^[]+)(\[(?P<type>\w+)\])?')
    if not hasattr(stream_or_filename, 'read'):
      stream=open(stream_or_filename, 'r')
    else:
      stream=stream_or_filename
    header=False
    num_lines=0
    for line in stream:
      line=line.strip()
      if line.startswith('#'):
        continue
      if len(line)==0:
        continue
      num_lines+=1
      if not header:
        fieldnames=[]
        fieldtypes=[]
        for col in line.split():
          match=fieldname_pattern.match(col)
          if match:
            if match.group('type'):
              fieldtypes.append(match.group('type'))
            else:
              fieldtypes.append('string')
            fieldnames.append(match.group('name'))
        tab=Table(fieldnames, fieldtypes)
        header=True
        continue
      tab.AddRow(line.split())
    if num_lines==0:
      raise IOError("Cannot read table from empty stream")
    return tab

  def _GuessColumnTypes(self):
    for col_idx in range(len(self.col_names)):
      self.col_types[col_idx]=GuessColumnType(self[self.col_names[col_idx]])
    for row in self.rows:
      for idx in range(len(row)):
        row[idx]=self._Coerce(row[idx], self.col_types[idx])
        
  @staticmethod
  def _LoadCSV(stream_or_filename, sep):
    if not hasattr(stream_or_filename, 'read'):
      stream=open(stream_or_filename, 'r')
    else:
      stream=stream_or_filename
    reader=csv.reader(stream, delimiter=sep)
    first=True
    for row in reader:
      if first:
        header=row
        types='s'*len(row)
        tab=Table(header, types)
        first=False
      else:
        tab.AddRow(row)
    if first:
      raise IOError('trying to load table from empty CSV stream/file')

    tab._GuessColumnTypes()
    return tab

  @staticmethod
  def _LoadPickle(stream_or_filename):
    if not hasattr(stream_or_filename, 'read'):
      stream=open(stream_or_filename, 'rb')
    else:
      stream=stream_or_filename
    return cPickle.load(stream)

  @staticmethod
  def Load(stream_or_filename, format='ost', sep=','):
    """
    Load table from stream or file with given name.

    By default, the file format is *ost* (see below) and is *not* automatically
    determined (e.g. from file extension). Thus, it *format* must be specified
    for reading other file formats.

    The following file formats are understood:

    - ost

      This is an ost-specific, but still human readable file format. The file
      (stream) must start with header line of the form

        col_name1[type1] <col_name2[type2]>...

      The types given in brackets must be one of the data types the
      :class:`Table` class understands. Each following line in the file then must
      contains exactly the same number of data items as listed in the header. The
      data items are automatically converted to the column format. Lines starting
      with a '#' and empty lines are ignored.

    - pickle

      Deserializes the table from a pickled byte stream

    - csv

      Reads the table from comma separated values stream. Since there is no
      explicit type information in the csv file, the column types are guessed,
      using the following simple rules:

      * if all values are either NA/NULL/NONE the type is set to string
      * if all non-null values are convertible to float/int the type is set to
        float/int
      * if all non-null values are true/false/yes/no, the value is set to bool
      * for all other cases, the column type is set to string

    :returns: A new :class:`Table` instance
    """
    format=format.lower()
    if format=='ost':
      return Table._LoadOST(stream_or_filename)
    if format=='csv':
      return Table._LoadCSV(stream_or_filename, sep=sep)
    if format=='pickle':
      return Table._LoadPickle(stream_or_filename)
    raise ValueError('unknown format ""' % format)

  def Sort(self, by, order='+'):
    """
    Performs an in-place sort of the table, based on column.
    """
    sign=-1
    if order=='-':
      sign=1
    key_index=self.GetColIndex(by)
    def _key_cmp(lhs, rhs):
      return sign*cmp(lhs[key_index], rhs[key_index])
    self.rows=sorted(self.rows, _key_cmp)
    
  def GetUnique(self, col, ignore_nan=True):
    """
    Extract a list of all unique values from one column
    """
    idx = self.GetColIndex(col)
    seen = {}
    result = []
    for row in self.rows:
      item = row[idx]
      if item!=None or ignore_nan==False:
        if item in seen: continue
        seen[item] = 1
        result.append(item)
    return result
    
  def Zip(self, *args):
    """
    Allows to conveniently iterate over a selection of columns, e.g.
    
    .. code-block:: python
    
      tab=Table.Load('...')
      for col1, col2 in tab.Zip('col1', 'col2'):
        print col1, col2
    
    is a shortcut for
    
    .. code-block:: python
    
      tab=Table.Load('...')
      for col1, col2 in zip(tab['col1'], tab['col2']):
        print col1, col2
    """
    return zip(*[self[arg] for arg in args])

  def Plot(self, x, y=None, z=None, style='.', x_title=None, y_title=None,
           z_title=None, x_range=None, y_range=None, z_range=None,
           color=None, plot_if=None, legend=None,
           num_z_levels=10, diag_line=False, labels=None, max_num_labels=None,
           title=None, clear=True, save=False, **kwargs):
    """
    Plot x against y using matplot lib
    """
    try:
      import matplotlib.pyplot as plt
      import matplotlib.mlab as mlab
      import numpy as np
      idx1 = self.GetColIndex(x)
      xs = []
      ys = []
      zs = []
           
      if clear:
        plt.figure(figsize=[8, 6])
      
      if x_title:
        nice_x=x_title
      else:
        nice_x=MakeTitle(x)
      
      if y_title:
        nice_y=y_title
      else:
        if y:
          nice_y=MakeTitle(y)
        else:
          nice_y=None
      
      if z_title:
        nice_z = z_title
      else:
        if z:
          nice_z = MakeTitle(z)
        else:
          nice_z = None
      if color:
        kwargs['color']=color
      if legend:
        kwargs['label']=legend
      if y and z:
        idx3 = self.GetColIndex(z)
        idx2 = self.GetColIndex(y)
        for row in self.rows:
          if row[idx1]!=None and row[idx2]!=None and row[idx3]!=None:
            if plot_if and not plot_if(self, row):
              continue
            xs.append(row[idx1])
            ys.append(row[idx2])
            zs.append(row[idx3])
        levels = []
        if z_range:
          z_spacing = (z_range[1] - z_range[0]) / num_z_levels
          l = z_range[0]
        else:
          l = self.Min(z)
          z_spacing = (self.Max(z) - l) / num_z_levels
        
        for i in range(0,num_z_levels+1):
          levels.append(l)
          l += z_spacing
  
        xi = np.linspace(min(xs)-0.1,max(xs)+0.1,len(xs)*10)
        yi = np.linspace(min(ys)-0.1,max(ys)+0.1,len(ys)*10)
        zi = mlab.griddata(xs, ys, zs, xi, yi)
  
        plt.contour(xi,yi,zi,levels,linewidths=0.5,colors='k')
        plt.contourf(xi,yi,zi,levels,cmap=plt.cm.jet)
        plt.colorbar(ticks=levels)
            
      elif y:
        idx2=self.GetColIndex(y)
        for row in self.rows:
          if row[idx1]!=None and row[idx2]!=None:
            if plot_if and not plot_if(self, row):
              continue
            xs.append(row[idx1])
            ys.append(row[idx2])
        plt.plot(xs, ys, style, **kwargs)
        
      else:
        label_vals=[]
        
        if labels:
          label_idx=self.GetColIndex(labels)
        for row in self.rows:
          if row[idx1]!=None:
            if plot_if and not plot_if(self, row):
              continue
            xs.append(row[idx1])
            if labels:
              label_vals.append(row[label_idx])
        plt.plot(xs, style, **kwargs)
        if labels:
          interval = 1
          if max_num_labels:
            if len(label_vals)>max_num_labels:
              interval = int(math.ceil(float(len(label_vals))/max_num_labels))
              label_vals = label_vals[::interval]
          plt.xticks(np.arange(0, len(xs), interval), label_vals, rotation=45,
                     size='x-small')
      
      if title==None:
        if nice_z:
          title = '%s of %s vs. %s' % (nice_z, nice_x, nice_y)
        elif nice_y:
          title = '%s vs. %s' % (nice_x, nice_y)
        else:
          title = nice_x
  
      plt.title(title, size='x-large', fontweight='bold',
                verticalalignment='bottom')
      
      if legend:
        plt.legend(loc=0)
      
      if x and y:
        plt.xlabel(nice_x, size='x-large')
        if x_range:
          plt.xlim(x_range[0], x_range[1])
        if y_range:
          plt.ylim(y_range[0], y_range[1])
        if diag_line:
          plt.plot(x_range, y_range, '-')
        
        plt.ylabel(nice_y, size='x-large')
      else:
        if y_range:
          plt.ylim(y_range[0], y_range[1])
        if x_title:
          plt.xlabel(x_title, size='x-large')
        plt.ylabel(nice_y, size='x-large')
      if save:
        plt.savefig(save)
      return plt
    except ImportError:
      LogError("Function needs numpy and matplotlib, but I could not import it.")
      raise
    
  def PlotHistogram(self, col, x_range=None, num_bins=10, normed=False,
                    histtype='stepfilled', align='mid', x_title=None,
                    y_title=None, title=None, clear=True, save=False):
    """
    Create a histogram of the data in col for the range *x_range*, split into
    *num_bins* bins and plot it using matplot lib.
    """
    try:
      import matplotlib.pyplot as plt
      import numpy as np
      
      if len(self.rows)==0:
        return None
      
      idx = self.GetColIndex(col)
      data = []
      for r in self.rows:
        if r[idx]!=None:
          data.append(r[idx])
        
      if clear:
        plt.clf()
        
      n, bins, patches = plt.hist(data, bins=num_bins, range=x_range,
                                  normed=normed, histtype=histtype, align=align)
      
      if x_title:
        nice_x=x_title
      else:
        nice_x=MakeTitle(col)
      plt.xlabel(nice_x, size='x-large')
      
      if y_title:
        nice_y=y_title
      else:
        nice_y="bin count"  
      plt.ylabel(nice_y, size='x-large')
      
      if title:
        nice_title=title
      else:
        nice_title="Histogram of %s"%nice_x
      plt.title(nice_title, size='x-large', fontweight='bold')
      
      if save:
        plt.savefig(save)
      return plt
    except ImportError:
      LogError("Function needs numpy and matplotlib, but I could not import it.")
      raise
 
  def _Max(self, col):
    if len(self.rows)==0:
      return None, None
    idx = self.GetColIndex(col)
    col_type = self.col_types[idx]
    if col_type=='int' or col_type=='float':
      max_val = -float('inf')
    elif col_type=='bool':
      max_val = False
    elif col_type=='string':
      max_val = chr(0)
    max_idx = None
    for i in range(0, len(self.rows)):
      if self.rows[i][idx]>max_val:
        max_val = self.rows[i][idx]
        max_idx = i
    return max_val, max_idx
  
  def MaxRow(self, col):
    """
    Returns the row containing the cell with the maximal value in col. If 
    several rows have the highest value, only the first one is returned.
    None values are ignored.
    """
    val, idx = self._Max(col)
    return self.rows[idx]
  
  def Max(self, col):
    """
    Returns the maximum value in col. If several rows have the highest value,
    only the first one is returned. None values are ignored.
    """
    val, idx = self._Max(col)
    return val
  
  def MaxIdx(self, col):
    """
    Returns the row index of the cell with the maximal value in col. If
    several rows have the highest value, only the first one is returned.
    None values are ignored.
    """
    val, idx = self._Max(col)
    return idx
  
  def _Min(self, col):
    if len(self.rows)==0:
      return None, None
    idx=self.GetColIndex(col)
    col_type = self.col_types[idx]
    if col_type=='int' or col_type=='float':
      min_val=float('inf')
    elif col_type=='bool':
      min_val=True
    elif col_type=='string':
      min_val=chr(255)
    min_idx=None
    for i,row in enumerate(self.rows):
      if row[idx]!=None and row[idx]<min_val:
        min_val=row[idx]
        min_idx=i
    return min_val, min_idx

  def Min(self, col):
    """
    Returns the minimal value in col. If several rows have the lowest value,
    only the first one is returned. None values are ignored.
    """
    val, idx = self._Min(col)
    return val
  
  def MinRow(self, col):
    """
    Returns the row containing the cell with the minimal value in col. If 
    several rows have the lowest value, only the first one is returned.
    None values are ignored.
    """
    val, idx = self._Min(col)
    return self.rows[idx]
  
  def MinIdx(self, col):
    """
    Returns the row index of the cell with the minimal value in col. If
    several rows have the lowest value, only the first one is returned.
    None values are ignored.
    """
    val, idx = self._Min(col)
    return idx
  
  def Sum(self, col):
    """
    Returns the sum of the given column. Cells with None are ignored. Returns 
    0.0, if the column doesn't contain any elements.
    """
    idx = self.GetColIndex(col)
    col_type = self.col_types[idx]
    if col_type!='int' and col_type!='float':
      raise TypeError("Sum can only be used on numeric column types")
    s = 0.0
    for r in self.rows:
      if r[idx]!=None:
        s += r[idx] 
    return s 

  def Mean(self, col):
    """
    Returns the mean of the given column. Cells with None are ignored. Returns 
    None, if the column doesn't contain any elements.
    """
    idx = self.GetColIndex(col)
    col_type = self.col_types[idx]
    if col_type!='int' and col_type!='float':
      raise TypeError("Mean can only be used on numeric column types")
    
    vals=[]
    for v in self[col]:
      if v!=None:
        vals.append(v)
    try:
      return stutil.Mean(vals)
    except:
      return None
    
  def RowMean(self, mean_col_name, cols):
    """
    Adds a new column of type 'float' with a specified name (mean_col),
    containing the mean of all specified columns for each row.
    
    Cols are specified by their names and must be of numeric column
    type ('float', 'int'). Cells with None are ignored. Adds None if the row
    doesn't contain any values.
    
    
    == Example ==
   
    Staring with the following table:
    
    ==== ==== ====
    x     y    u           
    ==== ==== ====
     1    10  100 
     2    15  None 
     3    20  400 
    ==== ==== ====
    
    the code here adds a column with the name 'mean' to yield the table below:
    
    .. code-block::python
    
      tab.RowMean('mean', 'x', 'u')
    
    
    ==== ==== ==== ===== 
    x     y    u   mean           
    ==== ==== ==== =====
     1    10  100  50.5 
     2    15  None 2
     3    20  400  201.5 
    ==== ==== ==== =====
      
    """
    
    if IsScalar(cols):
      cols = [cols]
    
    cols_idxs = []
    for col in cols:
      idx = self.GetColIndex(col)
      col_type = self.col_types[idx]
      if col_type!='int' and col_type!='float':
        raise TypeError("RowMean can only be used on numeric column types")
      cols_idxs.append(idx)
      
    mean_rows = []
    for row in self.rows:
      vals = []
      for idx in cols_idxs:
        v = row[idx]
        if v!=None:
          vals.append(v)
      try:
        mean = stutil.Mean(vals)
        mean_rows.append(mean)
      except:
        mean_rows.append(None)
    
    self.AddCol(mean_col_name, 'f', mean_rows)
    
  def Median(self, col):
    """
    Returns the median of the given column. Cells with None are ignored. Returns 
    None, if the column doesn't contain any elements.
    """
    idx = self.GetColIndex(col)
    col_type = self.col_types[idx]
    if col_type!='int' and col_type!='float':
      raise TypeError("Mean can only be used on numeric column types")
    
    vals=[]
    for v in self[col]:
      if v!=None:
        vals.append(v)
    stutil.Median(vals)
    try:
      return stutil.Median(vals)
    except:
      return None
    
  def StdDev(self, col):
    """
    Returns the standard deviation of the given column. Cells with None are
    ignored. Returns None, if the column doesn't contain any elements.
    """
    idx = self.GetColIndex(col)
    col_type = self.col_types[idx]
    if col_type!='int' and col_type!='float':
      raise TypeError("Mean can only be used on numeric column types")
    
    vals=[]
    for v in self[col]:
      if v!=None:
        vals.append(v)
    try:
      return stutil.StdDev(vals)
    except:
      return None

  def Count(self, col, ignore_nan=True):
    """
    Count the number of cells in column that are not equal to None.
    """
    count=0
    idx=self.GetColIndex(col)
    for r in self.rows:
      if ignore_nan:
        if r[idx]!=None:
          count+=1
      else:
        count+=1
    return count

  def Correl(self, col1, col2):
    """
    Calculate the Pearson correlation coefficient between col1 and col2, only 
    taking rows into account where both of the values are not equal to None. If 
    there are not enough data points to calculate a correlation coefficient, 
    None is returned.
    """
    if IsStringLike(col1) and IsStringLike(col2):
      col1 = self.GetColIndex(col1)
      col2 = self.GetColIndex(col2)
    vals1, vals2=([],[])
    for v1, v2 in zip(self[col1], self[col2]):
      if v1!=None and v2!=None:
        vals1.append(v1)
        vals2.append(v2)
    try:
      return stutil.Correl(vals1, vals2)
    except:
      return None

  def SpearmanCorrel(self, col1, col2):
    """
    Calculate the Spearman correlation coefficient between col1 and col2, only 
    taking rows into account where both of the values are not equal to None. If 
    there are not enough data points to calculate a correlation coefficient, 
    None is returned.
    """
    try:
      import scipy.stats.mstats
      
      if IsStringLike(col1) and IsStringLike(col2):
        col1 = self.GetColIndex(col1)
        col2 = self.GetColIndex(col2)
      vals1, vals2=([],[])
      for v1, v2 in zip(self[col1], self[col2]):
        if v1!=None and v2!=None:
          vals1.append(v1)
          vals2.append(v2)
      try:
        correl = scipy.stats.mstats.spearmanr(vals1, vals2)[0]
        if scipy.isnan(correl):
          return None
        return correl
      except:
        return None

    except ImportError:
      LogError("Function needs scipy, but I could not import it.")
      raise
    

  def Save(self, stream_or_filename, format='ost', sep=','):
    """
    Save the table to stream or filename. For supported file formats, see
    :meth:`Load`
    """
    format=format.lower()
    if format=='ost':
      return self._SaveOST(stream_or_filename)
    if format=='csv':
      return self._SaveCSV(stream_or_filename, sep=sep)
    if format=='pickle':
      return self._SavePickle(stream_or_filename)
    raise ValueError('unknown format "%s"' % format)

  def _SavePickle(self, stream):
    if not hasattr(stream, 'write'):
      stream=open(stream, 'wb')
    cPickle.dump(self, stream, cPickle.HIGHEST_PROTOCOL)

  def _SaveCSV(self, stream, sep):
    if not hasattr(stream, 'write'):
      stream=open(stream, 'wb')

    writer=csv.writer(stream, delimiter=sep)
    writer.writerow(['%s' % n for n in self.col_names])
    for row in self.rows:
      row=list(row)
      for i, c in enumerate(row):
        if c==None:
          row[i]='NA'
      writer.writerow(row)

  def _SaveOST(self, stream):
    if hasattr(stream, 'write'):
      writer=csv.writer(stream, delimiter=' ')
    else:
      stream=open(stream, 'w')
      writer=csv.writer(stream, delimiter=' ')
    if self.comment:
      stream.write(''.join(['# %s\n' % l for l in self.comment.split('\n')]))
    writer.writerow(['%s[%s]' % t for t in zip(self.col_names, self.col_types)])
    for row in self.rows:
      row=list(row)
      for i, c in enumerate(row):
        if c==None:
          row[i]='NA'
      writer.writerow(row)
  
     
  def GetNumpyMatrix(self, *args):
    '''
    Returns a numpy matrix containing the selected columns from the table as 
    columns in the matrix.
    Only columns of type int or float are supported. NA values in the table
    will be converted to None values.
    '''
    try:
      import numpy as np
      
      if len(args)==0:
        raise RuntimeError("At least one column must be specified.")
      
      idxs = []
      for arg in args:
        idx = self.GetColIndex(arg)
        col_type = self.col_types[idx]
        if col_type!='int' and col_type!='float':
          raise TypeError("Numpy matrix can only be generated from numeric column types")
        idxs.append(idx)
      m = np.matrix([list(self[i]) for i in idxs]) 
      return m.T
    
    except ImportError:
      LogError("Function needs numpy, but I could not import it.")
      raise
    
  def GetOptimalPrefactors(self, ref_col, *args, **kwargs):
    '''
    This returns the optimal prefactor values (i.e. a, b, c, ...) for the
    following equation
    
    .. math::
      :label: op1
      
      a*u + b*v + c*w + ... = z
    
    where u, v, w and z are vectors. In matrix notation
    
    .. math::
      :label: op2
      
      A*p = z
    
    where A contains the data from the table (u,v,w,...), p are the prefactors 
    to optimize (a,b,c,...) and z is the vector containing the result of
    equation :eq:`op1`.
    
    The parameter ref_col equals to z in both equations, and \*args are columns
    u, v and w (or A in :eq:`op2`). All columns must be specified by their names.
    
    **Example:**
    
    .. code-block:: python
    
      tab.GetOptimalPrefactors('colC', 'colA', 'colB')
    
    The function returns a list of containing the prefactors a, b, c, ... in 
    the correct order (i.e. same as columns were specified in \*args).
    
    Weighting:
    If the kwarg weights="columX" is specified, the equations are weighted by
    the values in that column. Each row is multiplied by the weight in that row,
    which leads to :eq:`op3`:
    
    .. math::
      :label: op3
      
      weight*a*u + weight*b*v + weight*c*w + ... = weight*z
    
    Weights must be float or int and can have any value. A value of 0 ignores
    this equation, a value of 1 means the same as no weight. If all weights are
    the same for each row, the same result will be obtained as with no weights.
    
    **Example:**
    
    .. code-block:: python
    
      tab.GetOptimalPrefactors('colC', 'colA', 'colB', weights='colD')
    
    '''
    try:
      import numpy as np
  
      if len(args)==0:
        raise RuntimeError("At least one column must be specified.")
      
      b = self.GetNumpyMatrix(ref_col)
      a = self.GetNumpyMatrix(*args)
      
      if len(kwargs)!=0:
        if kwargs.has_key('weights'):
          w = self.GetNumpyMatrix(kwargs['weights'])
          b = np.multiply(b,w)
          a = np.multiply(a,w)
          
        else:
          raise RuntimeError("specified unrecognized kwargs, use weights as key")
      
      k = (a.T*a).I*a.T*b
      return list(np.array(k.T).reshape(-1))
    
    except ImportError:
      LogError("Function needs numpy, but I could not import it.")
      raise

  def PlotEnrichment(self, score_col, class_col, score_dir='-', 
                     class_dir='-', class_cutoff=2.0,
                     style='-', title=None, x_title=None, y_title=None,
                     clear=True, save=None):
    '''
    Plot an enrichment curve using matplotlib.
    
    For more information about parameters, see :meth:`ComputeEnrichment`
    '''
    
    try:
      import matplotlib.pyplot as plt
    
      enrx, enry = self.ComputeEnrichment(score_col, class_col, score_dir,
                                          class_dir, class_cutoff)
      
      if not title:
        title = 'Enrichment of %s'%score_col
        
      if not x_title:
        x_title = '% database'
        
      if not y_title:
        y_title = '% positives'
        
      if clear:
        plt.clf()
        
      plt.plot(enrx, enry, style)
      
      plt.title(title, size='x-large', fontweight='bold')     
      plt.ylabel(y_title, size='x-large')
      plt.xlabel(x_title, size='x-large')
      
      if save:
        plt.savefig(save)
      
      return plt
    except ImportError:
      LogError("Function needs matplotlib, but I could not import it.")
      raise
    
  def ComputeEnrichment(self, score_col, class_col, score_dir='-', 
                        class_dir='-', class_cutoff=2.0):
    '''
    Computes the enrichment of one column (e.g. score) over all data points.
    
    For this it is necessary, that the datapoints are classified into positive
    and negative points. This can be done in two ways:
    
     - by using one 'bool' column which contains True for positives and False
       for negatives
     - by specifying an additional column, a cutoff value and the columns
       direction. This will generate the classification on the fly
       
       * if class_dir=='-': values in the classification column that are 
                            less than or equal to class_cutoff will be counted
                            as positives
       * if class_dir=='+': values in the classification column that are 
                            larger than or equal to class_cutoff will be counted
                            as positives

    During the calculation, the table will be sorted according to score_dir,
    where a '-' values means smallest values first and therefore, the smaller
    the value, the better.
    
    '''
    
    ALLOWED_DIR = ['+','-']
    
    score_idx = self.GetColIndex(score_col)
    score_type = self.col_types[score_idx]
    if score_type!='int' and score_type!='float':
      raise TypeError("Score column must be numeric type")
    
    class_idx = self.GetColIndex(class_col)
    class_type = self.col_types[class_idx]
    if class_type!='int' and class_type!='float' and class_type!='bool':
      raise TypeError("Classifier column must be numeric or bool type")
    
    if (score_dir not in ALLOWED_DIR) or (class_dir not in ALLOWED_DIR):
      raise ValueError("Direction must be one of %s"%str(ALLOWED_DIR))
    
    self.Sort(score_col, score_dir)
    
    x = [0]
    y = [0]
    enr = 0
    for i,row in enumerate(self.rows):
      class_val = row[class_idx]
      if class_val!=None:
        if class_type=='bool':
          if class_val==True:
            enr += 1
        else:
          if (class_dir=='-' and class_val<=class_cutoff) or (class_dir=='+' and class_val>=class_cutoff):
            enr += 1
      x.append(i+1)
      y.append(enr)
    x = [float(v)/x[-1] for v in x]
    y = [float(v)/y[-1] for v in y]
    return x,y
    
  def ComputeEnrichmentAUC(self, score_col, class_col, score_dir='-', 
                           class_dir='-', class_cutoff=2.0):
    '''
    Computes the area under the curve of the enrichment using the trapezoidal
    rule.
    
    For more information about parameters, see :meth:`ComputeEnrichment`
    '''
    try:
      import numpy as np
      
      enrx, enry = self.ComputeEnrichment(score_col, class_col, score_dir,
                                          class_dir, class_cutoff)
      
      return np.trapz(enry, enrx)
    except ImportError:
      LogError("Function needs numpy, but I could not import it.")
      raise

  def ComputeROC(self, score_col, class_col, score_dir='-',
                 class_dir='-', class_cutoff=2.0):
    '''
    Computes the receiver operating characteristics of one column (e.g. score)
    over all data points.

    For this it is necessary, that the datapoints are classified into positive
    and negative points. This can be done in two ways:

     - by using one 'bool' column (*class_col*) which contains True for positives
       and False for negatives
     - by using a non-bool column (*class_col*), a cutoff value (*class_cutoff*)
       and the classification columns direction (*class_dir*). This will generate
       the classification on the fly

       * if *class_dir*=='-': values in the classification column that are
                            less than or equal to *class_cutoff* will be counted
                            as positives
       * if *class_dir*=='+': values in the classification column that are
                            larger than or equal to *class_cutoff* will be
                            counted as positives

    During the calculation, the table will be sorted according to *score_dir*,
    where a '-' values means smallest values first and therefore, the smaller
    the value, the better.

    If *class_col* does not contain any positives (i.e. value is True (if column
    is of type bool) or evaluated to True (if column is of type int or float
    (depending on *class_dir* and *class_cutoff*))) the ROC is not defined and
    the function will return *None*.
    '''

    ALLOWED_DIR = ['+','-']

    score_idx = self.GetColIndex(score_col)
    score_type = self.col_types[score_idx]
    if score_type!='int' and score_type!='float':
      raise TypeError("Score column must be numeric type")

    class_idx = self.GetColIndex(class_col)
    class_type = self.col_types[class_idx]
    if class_type!='int' and class_type!='float' and class_type!='bool':
      raise TypeError("Classifier column must be numeric or bool type")

    if (score_dir not in ALLOWED_DIR) or (class_dir not in ALLOWED_DIR):
      raise ValueError("Direction must be one of %s"%str(ALLOWED_DIR))

    self.Sort(score_col, score_dir)

    x = [0]
    y = [0]
    tp = 0
    fp = 0
    old_score_val = None

    for i,row in enumerate(self.rows):
      class_val = row[class_idx]
      score_val = row[score_idx]
      if class_val!=None:
        if old_score_val==None:
          old_score_val = score_val
        if score_val!=old_score_val:
          x.append(fp)
          y.append(tp)
          old_score_val = score_val
        if class_type=='bool':
          if class_val==True:
            tp += 1
          else:
            fp += 1
        else:
          if (class_dir=='-' and class_val<=class_cutoff) or (class_dir=='+' and class_val>=class_cutoff):
            tp += 1
          else:
            fp += 1
    x.append(fp)
    y.append(tp)
    
    # if no false positives or false negatives values are found return None
    if x[-1]==0 or y[-1]==0:
      return None
    
    x = [float(v)/x[-1] for v in x]
    y = [float(v)/y[-1] for v in y]
    return x,y

  def ComputeROCAUC(self, score_col, class_col, score_dir='-',
                    class_dir='-', class_cutoff=2.0):
    '''
    Computes the area under the curve of the receiver operating characteristics
    using the trapezoidal rule.
    
    For more information about parameters, see :meth:`ComputeROC`
    '''
    try:
      import numpy as np

      roc = self.ComputeROC(score_col, class_col, score_dir,
                            class_dir, class_cutoff)

      if not roc:
        return None
      return np.trapz(roc[1], roc[0])
    except ImportError:
      LogError("Function needs numpy, but I could not import it.")
      raise

  def PlotROC(self, score_col, class_col, score_dir='-',
              class_dir='-', class_cutoff=2.0,
              style='-', title=None, x_title=None, y_title=None,
              clear=True, save=None):
    '''
    Plot an ROC curve using matplotlib.
    
    For more information about parameters, see :meth:`ComputeROC`
    '''

    try:
      import matplotlib.pyplot as plt

      roc = self.ComputeROC(score_col, class_col, score_dir,
                                   class_dir, class_cutoff)
      
      if not roc:
        return None

      enrx, enry = roc

      if not title:
        title = 'ROC of %s'%score_col

      if not x_title:
        x_title = 'false positive rate'

      if not y_title:
        y_title = 'true positive rate'

      if clear:
        plt.clf()

      plt.plot(enrx, enry, style)

      plt.title(title, size='x-large', fontweight='bold')
      plt.ylabel(y_title, size='x-large')
      plt.xlabel(x_title, size='x-large')

      if save:
        plt.savefig(save)

      return plt
    except ImportError:
      LogError("Function needs matplotlib, but I could not import it.")
      raise

  def IsEmpty(self, col_name=None, ignore_nan=True):
    '''
    Checks if a table is empty.
    
    If no column name is specified, the whole table is checked for being empty,
    whereas if a column name is specified, only this column is checked.
    
    By default, all NAN (or None) values are ignored, and thus, a table
    containing only NAN values is considered as empty. By specifying the 
    option ignore_nan=False, NAN values are counted as 'normal' values.
    '''
    
    # table with no columns and no rows
    if len(self.col_names)==0:
      if col_name:
        raise ValueError('Table has no column named "%s"' % col_name)
      return True
    
    # column name specified
    if col_name:
      if self.Count(col_name, ignore_nan=ignore_nan)==0:
        return True
      else:
        return False
      
    # no column name specified -> test whole table
    else:
      for row in self.rows:
        for cell in row:
          if ignore_nan:
            if cell!=None:
              return False
          else:
            return False
    return True
    
  def Extend(self, tab, merge=None):
    """
    Append each row of *tab* to the current table. The data is appended based
    on the column names, thus the order of the table columns is *not* relevant,
    only the header names.
    
    If there is a column in *tab* that is not present in the current table,
    it is added to the current table and filled with *None* for all the rows
    present in the current table.
    
    If the type of any column in *tab* is not the same as in the current table
    a *TypeError* is raised.
    
    If merge is specified, the function looks for an existing row and adds the
    data there instead of appending a new row. If merge is set to an existing
    column name, all existing rows are searched for the first row where the
    value of column with the name specified in merge is equal to the value of
    the specified column in the new data. If such a row is found, all existing
    data of this row is overwritten with the new data. If no matching row is
    found, a new row is appended to the table.
    """
    # add column to current table if it doesn't exist
    for name,typ in zip(tab.col_names, tab.col_types):
      if not name in self.col_names:
        self.AddCol(name, typ)
    
    # check that column types are the same in current and new table
    for name in self.col_names:
      curr_type = self.col_types[self.GetColIndex(name)]
      new_type = tab.col_types[tab.GetColIndex(name)]
      if curr_type!=new_type:
        raise TypeError('cannot extend table, column %s in new '%name +\
                        'table different type (%s) than in '%new_type +\
                        'current table (%s)'%curr_type)
    
    num_rows = len(tab.rows)
    for i in range(0,num_rows):
      row = tab.rows[i]
      data = dict(zip(tab.col_names,row))
      self.AddRow(data, merge)
    

def Merge(table1, table2, by, only_matching=False):
  """
  Returns a new table containing the data from both tables. The rows are 
  combined based on the common values in the column(s) by. The option 'by' can
  be a list of column names. When this is the case, merging is based on
  multiple columns.
  For example, the two tables below

  ==== ====
  x     y            
  ==== ====
   1    10
   2    15
   3    20
  ==== ====
  
  ==== ====
  x     u
  ==== ====
    1  100
    3  200
    4  400
  ==== ====

  ===== ===== =====
  x      y     u
  ===== ===== =====
  1      10    100
  2      15    None
  3      20    200
  4      None  400
  ===== ===== =====
  
  when merged by column x, produce the following output:
  """
  def _key(row, indices):
    return tuple([row[i] for i in indices])
  def _keep(indices, cn, ct, ni):
    ncn, nct, nni=([],[],[])
    for i in range(len(cn)):
      if i not in indices:
        ncn.append(cn[i])
        nct.append(ct[i])
        nni.append(ni[i])
    return ncn, nct, nni
  col_names=list(table2.col_names)
  col_types=list(table2.col_types)
  new_index=[i for i in range(len(col_names))]
  if isinstance(by, str):
    common2_indices=[col_names.index(by)]
  else:
    common2_indices=[col_names.index(b) for b in by]
  col_names, col_types, new_index=_keep(common2_indices, col_names, 
                                        col_types, new_index)

  for i, name in enumerate(col_names):
    try_name=name
    counter=1
    while try_name in table1.col_names:
      counter+=1
      try_name='%s_%d' % (name, counter)
    col_names[i]=try_name
  common1={}
  if isinstance(by, str):
    common1_indices=[table1.col_names.index(by)]
  else:
    common1_indices=[table1.col_names.index(b) for b in by]
  for row in table1.rows:
    key=_key(row, common1_indices)
    if key in common1:
      raise ValueError('duplicate key "%s in first table"' % (str(key)))
    common1[key]=row
  common2={}
  for row in table2.rows:
    key=_key(row, common2_indices)
    if key in common2:
      raise ValueError('duplicate key "%s" in second table' % (str(key)))
    common2[key]=row
  new_tab=Table(table1.col_names+col_names, table1.col_types+col_types)
  for k, v in common1.iteritems():
    row=v+[None for i in range(len(table2.col_names)-len(common2_indices))]
    matched=False
    if k in common2:
      matched=True
      row2=common2[k]
      for i, index in enumerate(new_index):
        row[len(table1.col_names)+i]=row2[index]
    if only_matching and not matched:
      continue
    new_tab.AddRow(row)
  if only_matching:
    return new_tab
  for k, v in common2.iteritems():
    if not k in common1:
      v2=[v[i] for i in new_index]
      row=[None for i in range(len(table1.col_names))]+v2
      for common1_index, common2_index in zip(common1_indices, common2_indices):
        row[common1_index]=v[common2_index]
      new_tab.AddRow(row)
  return new_tab

  
