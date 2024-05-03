import monkey
from . import data
from . import settings
from . import ui

def makeText(id):
    msg = monkey.Text('text', 'c64', data.strings[id], pal=4)
    msg.set_position(0, 200, 0)
    return msg

def walkToItem(script, *args):
    item_info = data.items[args[0]]
    walkto = item_info['walk_to']
    walkdir = item_info.get('walk_dir', None)
    script.add(monkey.actions.Walk(data.tag_to_id['player'], walkto))
    if walkdir:
        script.add(monkey.actions.Turn(data.tag_to_id['player'], walkdir))

def addToInventory(item):
    def f():
        inv = data.inventory[settings.characters[settings.player]]
        if item in inv:
            return
        monkey.get_node(data.tag_to_id[item]).remove()
        data.inventory[settings.characters[settings.player]].append(item)
        ui.refresh_inventory()
    return f



def addText(msg):
    def f():
        textNode = monkey.get_node(settings.id_text)
        textNode.add(msg)
    return f

def message(script, x):
    msg = makeText(x)
    script.add(monkey.actions.CallFunc(addText(msg)))
    script.add(monkey.actions.Delay(1))
    script.add(monkey.actions.CallFunc(lambda: msg.remove()))

def goto_room(room, x, y, dir):
    def f():
        p = data.items[settings.characters[settings.player]]
        p['room'] = room
        p['pos'] = (x, y)
        p['direction'] = dir
        settings.room = room
        monkey.close_room()
    return f



def say(script, *args):
    id = data.tag_to_id['player']
    script.add(monkey.actions.CallFunc(lambda: monkey.get_node(id).sendMessage(id="animate", anim="talk")))
    for x in args:
        message(script, x)
    script.add(monkey.actions.CallFunc(lambda: monkey.get_node(id).sendMessage(id="animate", anim="idle")))

def _read(script):
    say(script, 24)

def _push(script):
    say(script, 25)

def _open(script):
    say(script, 26)

def _close(script):
    say(script, 27)



def pickup(script, item):
    script.add(monkey.actions.CallFunc(addToInventory(item)))




_pull = _push
_turnon = _close
_turnoff = _close
_use = _close



def change_room(script, *args):
    script.add(monkey.actions.CallFunc(goto_room(*args)))


def pull_doormat(script, *args):
    def f():
        data.doormat = 'open'
        doormat = monkey.get_node(data.tag_to_id['doormat'])
        doormat.setAnimation('open')
        doormat.getMouseArea().setShape(monkey.shapes.AABB(0,64,0,8))

    if data.doormat == 'open':
        say(script, 22)
    else:
        script.add(monkey.actions.CallFunc(f))

def change_door_state(script, *args):
    # pass tag, state, variable
    setattr(data, args[2], args[1])
    #node = monkey.get_node(data.tag_to_id[args[0]])
    script.add(monkey.actions.Animate(data.tag_to_id[args[0]], args[1]))


def push_doormat(script, *args):
    def f():
        data.doormat = 'closed'
        doormat = monkey.get_node(data.tag_to_id['doormat'])
        doormat.setAnimation('closed')
        doormat.getMouseArea().setShape(monkey.shapes.AABB(0,88,0,8))

    if data.doormat == 'open':
        script.add(monkey.actions.CallFunc(f))

def open_main_door(script, *args):
    if not data.maindoor_unlocked:
        say(script, 29)
    else:
        change_door_state(script, 'door_main', 'open', 'maindoor')

def unlock_main_door(script, *args):
    data.maindoor_unlocked = True
    change_door_state(script, 'door_main', 'open', 'maindoor')
