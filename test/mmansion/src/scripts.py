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

def restart():
    print('SUCALOOOO')



def say(script, *args):
    id = data.tag_to_id['player']
    script.add(monkey.actions.CallFunc(lambda: monkey.get_node(id).sendMessage(id="animate", anim="talk")))
    for x in args:
        message(script, x)
    script.add(monkey.actions.CallFunc(lambda: monkey.get_node(id).sendMessage(id="animate", anim="idle")))

def change_room(script, *args):
    script.add(monkey.actions.CallFunc(lambda: restart()))



