import monkey
from . import settings
from . import data
from . import scripts


def refresh_action():
    print(data.tag_to_id)
    node = monkey.get_node(data.tag_to_id['label_action'])
    text = [ data.strings[settings.verbs[settings.action]['text']] ]
    if settings.item1:
        text.append(data.strings[data.items[settings.item1]['text']])
    if settings.item2:
        text.append(data.strings[settings.verbs[settings.action]['preposition']])
        text.append(data.strings[data.items[settings.item2]['text']])
    node.updateText(" ".join(text))


def on_enter_verb(node):
    node.get_model().set_palette(2)


def on_leave_verb(node):
    node.get_model().set_palette(1)

def on_click_verb(id):
    def f(node):
        settings.action = id
        settings.item1 = None
        settings.item2 = None
        refresh_action()
    return f

def on_enter_item(item):
    def f(node):
        if settings.item1 is None:
            settings.item1 = item
        else:
            settings.item2 = item
        refresh_action()
    return f

def on_leave_item(node):
    if settings.item2:
        settings.item2 = None
    else:
        settings.item1 = None
    refresh_action()

def execute_action(node):
    if not settings.item1:
        return
    if not settings.item2:
        # one item action
        item_info = data.items[settings.item1]
        walkto = item_info['walk_to']
        walkdir = item_info.get('walk_dir', None)
        script = monkey.Script(id="__player")
        script.add(monkey.actions.Walk(data.tag_to_id['player'], walkto))
        if walkdir:
            script.add(monkey.actions.Turn(data.tag_to_id['player'], walkdir))
        actions = item_info.get('actions', None)
        if actions and settings.action in actions:
            scr = actions[settings.action]
            f = getattr(scripts, scr[0])
            if f:
                f(script, *scr[1:])
        monkey.play(script)

