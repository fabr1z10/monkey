import monkey
from . import data
from . import settings

def makeText(id):
    msg = monkey.Text('text', 'c64', data.strings[id], pal=4)
    msg.set_position(0, 200, 0)
    return msg


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
        p = data.items[settings.player]
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

def push_doormat(script, *args):
    def f():
        data.doormat = 'closed'
        doormat = monkey.get_node(data.tag_to_id['doormat'])
        doormat.setAnimation('closed')
        doormat.getMouseArea().setShape(monkey.shapes.AABB(0,88,0,8))

    if data.doormat == 'open':
        script.add(monkey.actions.CallFunc(f))


