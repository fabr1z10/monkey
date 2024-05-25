import monkey
from . import settings
from . import scripts
from .tokenizer import Tokenizer




    #go_right()

keymap = dict()
keymap[0] = {
  32: ' ',
  44: ',',
  45: '\'',
  46: '.',
  65: 'A',
  66: 'B',
  67: 'C',
  68: 'D',
  69: 'E',
  70: 'F',
  71: 'G',
  72: 'H',
  73: 'I',
  74: 'J',
  75: 'K',
  76: 'L',
  77: 'M',
  78: 'N',
  79: 'O',
  80: 'P',
  81: 'Q',
  82: 'R',
  83: 'S',
  84: 'T',
  85: 'U',
  86: 'V',
  87: 'W',
  88: 'X',
  89: 'Y',
  90: 'Z',
  47: '-',
  48: '0',
  49: '1',
  50: '2',
  51: '3',
  52: '4',
  53: '5',
  54: '6',
  55: '7',
  56: '8',
  57: '9',
  58: ':',
  59: ';',
  93: '+',
  161: '<',
  268: 'ⓢ',
  332: '*',
  333: '-',
  334: '+',
}
keymap[1] = {
  32: ' ',
  44: ';',
  46: ':',
  65: '⒜',
  66: '⒝',
  67: '⒞',
  68: '⒟',
  69: '⒠',
  70: '⒡',
  71: '⒢',
  72: '⒣',
  73: '⒤',
  74: '⒥',
  75: '⒦',
  76: '⒧',
  77: '⒨',
  78: '⒩',
  79: '⒪',
  80: '⒫',
  81: '⒬',
  82: '⒭',
  83: '⒮',
  84: '⒯',
  85: '⒰',
  86: '⒱',
  87: '⒲',
  88: '♣',
  89: '⒴',
  48: '=',
  49: '!',
  50: '"',
  51: '£',
  52: '$',
  53: '%',
  54: '&',
  55: '/',
  56: '(',
  57: ')',
  93: '*',
  161: '>',
  268: '♡',
  262: '⇨',
  263: '⇦',
  264: '⇩',
  265: '⇧'
}
keymap[2] = {     # ctrl
  48: '⓪',
  49: '①',
  50: '②',
  51: '③',
  52: '④',
  53: '⑤',
  54: '⑥',
  55: '⑦',
  56: '⑧',
  57: '⑨'
}

def addLine(i, text=''):
  print('added line @ index',i)
  settings.lines.insert(i, scripts.Line(text))
  #l = monkey.Text('text', 'c64', text.rstrip().ljust(settings.COL_COUNT, ' '))
  #settings.lines.insert(i, [text, l.id])
  #monkey.get_node(settings.editor_id).add(l)
  scripts.reposition()

def updateLine(i, text):
  cl = settings.lines[i]
  cl.text = text
  line = monkey.get_node(cl.id)
  line.updateText(cl.text.rstrip().ljust(settings.COL_COUNT, ' '))
  go_right()

def rmLine(i):
  monkey.get_node(settings.lines[i].id).remove()
  del settings.lines[i]
  scripts.reposition()


def active_line():
  return settings.doc_top + settings.cy


# cursor blink routine
def f(pal):
  def g():
    if len(settings.lines) <= 0:
      return
    #print(settings.lines[settings.cy])
    if settings.question_id:
      line = monkey.get_node(settings.question_id)
      line.getRenderer().setQuadPalette(settings.qcx, pal)
    else:
      line = monkey.get_node(settings.lines[settings.doc_top+settings.cy].id)
      line.getRenderer().setQuadPalette(settings.cx, pal)
  return g

def go_right():
  print('called goright')
  cl = settings.lines[settings.cy + settings.doc_top]
  if settings.cx < len(cl.text):
    line = monkey.get_node(cl.id)
    line.getRenderer().setQuadPalette(settings.cx, 'default')
    settings.cx += 1
    if settings.cx >= settings.COL_COUNT:
      settings.cx = 0
      addLine(settings.doc_top + settings.cy + 1)
      settings.cy += 1
      if settings.cy >= settings.LINE_COUNT:
        settings.cy -= 1
        settings.doc_top+=1
        scripts.reposition()


def go_left():
  line = monkey.get_node(settings.lines[settings.cy].id)
  line.getRenderer().setQuadPalette(settings.cx, 'default')
  settings.cx -= 1
  if settings.cx < 0:
    if settings.cy > 0:
      settings.cy -= 1
      settings.cx = len(settings.lines[settings.cy].text)
    else:
      settings.cx = 0

def go_up():
  monkey.get_node(settings.lines[settings.cy].id).getRenderer().setQuadPalette(settings.cx, 'default')
  if active_line() > 0:
    if settings.cy > 0:
      settings.cy -= 1
      settings.cx = min(settings.cx, len(settings.lines[settings.cy].text))
    else:
      settings.doc_top -= 1
      scripts.reposition()
def go_down():
  monkey.get_node(settings.lines[settings.cy].id).getRenderer().setQuadPalette(settings.cx, 'default')
  if active_line() < len(settings.lines)-1:
    if settings.cy < settings.LINE_COUNT-1:
      settings.cy += 1
      settings.cx = min(settings.cx, len(settings.lines[settings.cy].text))
    else:
      settings.doc_top += 1
      scripts.reposition()
def cico(key,action,mods):
  if action == 0:
    return
  print('CALLBACK!')
  # shortcut
  if key == 79 and mods == 4:       # alt + o
    openfile(None)
  if key == 83 and mods == 4:       # alt + s
    saveas(None)


  if settings.question_id:
    qid = monkey.get_node(settings.question_id)
    if key == 256: # esc
      monkey.get_node(settings.question_id).remove()
      monkey.get_node(settings.question_label_id).remove()
      settings.question_id = None
    elif key==262:
      qid.getRenderer().setQuadPalette(settings.qcx, 'default')
      settings.qcx = min(settings.qcx+1, len(settings.input_string))
    elif key==263:
      qid.getRenderer().setQuadPalette(settings.qcx, 'default')
      settings.qcx = max(0, settings.qcx-1)
    elif key==259:
      settings.input_string=settings.input_string[:-1]
      qid.updateText(settings.input_string.ljust(12, ' '))
      settings.qcx = max(0, settings.qcx - 1)
    elif key == 257: #enter
      monkey.get_node(settings.question_id).remove()
      monkey.get_node(settings.question_label_id).remove()
      settings.question_id = None
      getattr(scripts, settings.question_func)(settings.input_string)
    if mods in keymap:
      ch = keymap[mods].get(key, None)
      if ch:
        t = settings.input_string
        settings.input_string = t[:settings.qcx] + ch + t[settings.qcx + 1:]
        qid.updateText(settings.input_string.ljust(12, ' '))
        settings.qcx = min(settings.qcx + 1, 11)
    return

  if key == 262 and mods==0:
    if settings.in_string:
      pass
    else:
      go_right()
  elif key == 263 and mods==0:
    if settings.in_string:
      pass
    else:
      go_left()
  elif key == 265 and mods==0:
    go_up()
  elif key == 264 and mods==0:
    go_down()
  elif key == 269: # end
    monkey.get_node(settings.lines[settings.cy].id).getRenderer().setQuadPalette(settings.cx, 'default')
    settings.cy = min(len(settings.lines)-1, settings.LINE_COUNT-1)
    settings.cx = len(settings.lines[-1].text)
    settings.doc_top = max(0,len(settings.lines)-settings.LINE_COUNT)
    print('NOW',settings.cx,settings.cy,settings.doc_top)
    scripts.reposition()

  elif key == 259: # backspace
    cl = settings.lines[settings.cy + settings.doc_top]
    if settings.cx > 0:
      if settings.cx == len(cl.text):
        cl.update(cl.text[:-1])
      else:
        cl.update(cl.text[:settings.cx-1] + cl.text[settings.cx:])
      #line = monkey.get_node(cl[1])
      #line.updateText(cl[0].ljust(settings.col_count, ' '))
      go_left()
    elif settings.cx == 0 and settings.cy + settings.doc_top > 0:
      if len(cl.text) == 0:
        rmLine(settings.doc_top + settings.cy)
        settings.cy -= 1
        settings.cx = len(settings.lines[settings.cy + settings.doc_top].text)
  elif key == 290:      # F1
    for l in settings.lines:
      print(l.text, ' -- ', l.breaks)
  elif key == 257:      # enter
    cl = settings.lines[settings.cy+settings.doc_top]
    # stuff that goes to next line
    nl = cl.text[settings.cx:]
    monkey.get_node(cl.id).getRenderer().setQuadPalette(settings.cx, 'default')
    cl.update(cl.text[:settings.cx])
    # create a new line
    addLine(settings.doc_top + settings.cy+1, nl)
    settings.cy += 1
    if settings.cy >= settings.LINE_COUNT:
      settings.cy = settings.LINE_COUNT-1
      settings.doc_top += 1
    settings.cx = 0
    scripts.reposition()
  print('FUCK')
  if mods in keymap:
    ch = keymap[mods].get(key, None)
    if ch:
      cl = settings.lines[settings.cy + settings.doc_top]
      cl.update(cl.text[:settings.cx] + ch + cl.text[settings.cx + 1:])
      go_right()



def ciao(x,y):
  if settings.menu_id:
    monkey.get_node(settings.menu_id).remove()
    settings.menu_id = None

def close_menu():
  if settings.menu_id:
    monkey.get_node(settings.menu_id).remove()
    settings.menu_id = None

def make_question(text, length, func):
  b = monkey.Text('text', 'c64', text, pal='menu')
  b.set_position(0,8,1)
  settings.input_string = ''
  question = monkey.Text('text', 'c64', settings.input_string.ljust(length, ' '), pal='menu')
  question.set_position(8*(len(text)), 8, 1)
  settings.qcx = 0
  settings.question_label_id = b.id
  settings.question_id = question.id
  settings.question_func=func
  monkey.get_node(settings.id_main).add(b)
  monkey.get_node(settings.id_main).add(question)


def openfile(a):
  close_menu()
  make_question('OPEN FILENAME: ', 12, 'openf')

def saveas(a):
  close_menu()
  make_question('SAVE FILENAME: ', 12, 'save')

def save(a):
  if settings.working_file:
    close_menu()
    scripts.save(settings.working_file)
  else:
    saveas(a)





def figa(a):
  # if this menu is open close it
  if settings.menu_id:
    monkey.get_node(settings.menu_id).remove()
    settings.menu_id = None
    if settings.menu_type == 'file':
      return


  fmenu = monkey.Node()
  fmenu.set_position(8, settings.device_size[1]-8, 1)
  y = 0
  opts = [('NEW', saveas), ('OPEN', openfile), ('SAVE', save), ('SAVE AS', saveas), ('QUIT', saveas)]
  ml = max([len(x) for x in opts])
  for i in opts:
    menu_option = monkey.Text('text', 'c64', i[0].ljust(ml+2, ' '), pal='menu')
    box_size = menu_option.size
    shape = monkey.shapes.AABB(0, box_size[0], -8, -8 + box_size[1])
    menu_option.add_component(monkey.components.MouseArea(shape, 0, 0, on_click=i[1], batch='line_ui'))
    menu_option.set_position(0, y, 1)
    y -= 8
    fmenu.add(menu_option)
  monkey.get_node(settings.id_main).add(fmenu)
  settings.menu_id = fmenu.id
  settings.menu_type = 'file'





def init():
  settings.tokenizer = Tokenizer('assets/token.yaml')
  # root
  # settings.tokenRoot = scripts.TokenNode()
  # settings.tokens = monkey.read_data_file('token.yaml')
  # settings.invtokens = dict()
  # settings.invord = dict()
  # for token, value in settings.tokens['tokens'].items():
  #   settings.invtokens[value] = token
  # for token, value in settings.tokens['ord'].items():
  #   settings.invord[value] = token
  #
  # for token, value in settings.tokens['tokens'].items():
  #   print(token, value)
  #   node = settings.tokenRoot
  #   i = 0
  #   while i < len(token):
  #     if token[i] not in node.children:
  #       node.children[token[i]] = scripts.TokenNode()
  #     node = node.children[token[i]]
  #     if i == len(token) - 1:
  #       node.token = value
  #     i += 1
  #
  # inst = scripts.tokenize('FORI=54272TO54296:POKEI,0:NEXT:POKE54296,15:POKE54277,17:POKE54278,136')
  # print(' # bytes:' ,len(inst))
  # print(' '.join(f'{x:02x}' for x in inst))
  #exit(0)


def create_room(room):
  width = settings.device_size[0]
  height = settings.device_size[1]
  cam = monkey.CamOrtho(width, height,
    viewport=(0, 0, width, height),
    bounds_x=(width // 2, width // 2), bounds_y=(height // 2, height // 2))
  room.add_camera(cam)
  room.add_runner(monkey.Scheduler())
  mm = monkey.MouseManager()
  mm.setFunc(0, ciao)
  mm.addCamera(0)
  room.add_runner(mm)

  kb = monkey.components.Keyboard()
  kb.add_fallback(cico)
  room.add_batch('text', monkey.SpriteBatch(max_elements=10000, cam=0, sheet='petscii'))
  room.add_batch('line_ui', monkey.LineBatch(max_elements=1000, cam=0))

  root = room.root()
  root.add_component(kb)
  settings.id_main = root.id
  menubar =monkey.Text('text', 'c64', 'FILE', pal='menu')
  box_size = menubar.size
  shape=monkey.shapes.AABB(0, box_size[0], -8, -8 + box_size[1])
  menubar.add_component(monkey.components.MouseArea(shape, 0, 0, on_click=figa, batch='line_ui'))
  menubar.set_position(8, settings.device_size[1], 0)
  root.add(menubar)
  statusbar = monkey.Text('text','c64', settings.COL_COUNT*' ', pal='menu')
  statusbar.set_position(0,8,0)
  root.add(statusbar)
  editor = monkey.Node()
  root.add(editor)
  settings.editor_id = editor.id

  # initialize
  settings.lines = []
  settings.lines.append(scripts.Line())
  scripts.reposition()
  # for i in range(0, settings.line_count):
  #   row_index = settings.doc_top + i
  #   if row_index >= len(settings.lines):
  #     break
  #   print('riga:',settings.lines[row_index])
  #   line = monkey.Text('text', 'c64', settings.lines[row_index][0].ljust(settings.col_count, ' '))
  #   settings.lines[row_index][1] = line.id
  #   line.set_position(0, height-8 - i*8, 0)
  #   editor.add(line)
  s = monkey.Script()
  s.add(monkey.actions.CallFunc(f('bw')), loop=True)
  s.add(monkey.actions.Delay(0.333))
  s.add(monkey.actions.CallFunc(f('inverse')))
  s.add(monkey.actions.Delay(0.333))
  monkey.play(s)