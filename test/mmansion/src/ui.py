import monkey
from . import settings
from . import data
from . import scripts


# returns whether current player has provided item
def player_has_item(item):
    return item in data.inventory[settings.characters[settings.player]]

def reset_action():
    settings.action = settings.default_verb
    settings.item1 = None
    settings.item2 = None
    settings.preposition = None


def getItemScript(item, action, other=None):
    itemInfo = data.items[item]
    actions = itemInfo.get('actions', None)
    scr = None
    if actions:
        script_id = action
        if other:
            script_id += '_' + other
        scr = actions.get(script_id, None)
    if scr:
        return getattr(scripts, scr[0]), scr[1:]
    return None


def refresh_action():
    node = monkey.get_node(data.tag_to_id['label_action'])
    if settings.action is None:
        node.updateText("")
        return
    #print(data.tag_to_id)
    text = [ data.strings[settings.verbs[settings.action]['text']] ]
    if settings.item1:
        text.append(data.strings[data.items[settings.item1]['text']])
    if settings.preposition:
        text.append(data.strings[settings.preposition])
    if settings.item2:
        #text.append(data.strings[settings.verbs[settings.action]['preposition']])
        text.append(data.strings[data.items[settings.item2]['text']])
    node.updateText(" ".join(text))

def refresh_inventory():
    inv = monkey.get_node(settings.id_inv)
    inv.clear()
    current_player = settings.characters[settings.player]
    inventory_items = data.inventory.get(current_player, [])
    for i in range(0, settings.inventory_max_items):
        j = settings.inventory_start_index + i
        if j >= len(inventory_items):
            break
        x = settings.inv_x[i % 2]
        y = settings.inv_y[i // 2]
        #print(inventory_items[j], x, y, '....')
        t = monkey.Text('text', 'c64', data.strings[data.items[inventory_items[j]]['text']][:18], pal='purple')
        box_size = t.size
        t.add_component(monkey.components.MouseArea(monkey.shapes.AABB(0, box_size[0], -8, -8+box_size[1]), 0, 1,
            on_enter=on_enter_inventory_item(inventory_items[j]), on_leave=on_leave_inventory_item, on_click=execute_action, batch='line_ui'))
        t.set_position(x, y, 0)
        inv.add(t)
    show_down_arrow = settings.inventory_start_index + settings.inventory_max_items < len(inventory_items)
    show_up_arrow = settings.inventory_start_index > 0
    if show_down_arrow:
        down_arrow = monkey.Node()
        a = monkey.models.Quad('text')
        a.add([2,17,12,7])
        down_arrow.set_model(a)
        down_arrow.set_position(155,5,0)
        down_arrow.add_component(monkey.components.MouseArea(monkey.shapes.AABB(0,12,0,7), 0, 1,
            on_enter=on_enter_arrow, on_leave=on_leave_arrow, on_click=move_inv(2),batch='line_ui'))
        inv.add(down_arrow)
    if show_up_arrow:
        up_arrow = monkey.Node()
        a = monkey.models.Quad('text')
        a.add([2,17,12,7], flipv=True)
        up_arrow.set_model(a)
        up_arrow.set_position(155,13,0)
        up_arrow.add_component(monkey.components.MouseArea(monkey.shapes.AABB(0,12,0,7), 0, 1,
            on_enter=on_enter_arrow, on_leave=on_leave_arrow, on_click=move_inv(-2),batch='line_ui'))
        inv.add(up_arrow)


def on_enter_verb(node):
    node.setPalette('yellow')

def on_leave_verb(node):
    node.setPalette('green')

def on_enter_arrow(node):
    node.setPalette(5)

def on_leave_arrow(node):
    node.setPalette(0)

def move_inv(pos):
    def f(node):
        settings.inventory_start_index += pos
        refresh_inventory()
    return f



def select_kid(i):
    def f(node):
        curr = data.items[settings.characters[settings.player]]
        id = data.tag_to_id['player']
        player = monkey.get_node(id)
        curr['direction'] = player.getController().direction
        curr['pos'] = [player.x, player.y]
        print('saved ',settings.player, ' position to',curr['pos'])
        settings.player = i
        settings.room = data.items[settings.characters[i]]['room']
        monkey.close_room()
    return f

def newkid(node):
    settings.action = None
    settings.item1 = None
    settings.item2 = None
    settings.preposition = None
    refresh_action()
    textNode = monkey.get_node(settings.id_newkid)
    xc = [1, 40, 100]
    i=0
    for c in settings.characters:
        name = data.strings[data.items[c]['text']]
        t = monkey.Text('text', 'c64', name, pal='purple')
        box_size = t.size
        t.add_component(monkey.components.MouseArea(monkey.shapes.AABB(0, box_size[0], -8, -8 + box_size[1]), 0, 1,
            on_enter=on_enter_newkid, on_leave=on_leave_newkid, on_click=select_kid(i), batch='line_ui'))
        t.set_position(xc[i],53,0)
        textNode.add(t)
        i +=1


def on_click_verb(id):
    def f(node):
        monkey.get_node(settings.id_newkid).clear()
        settings.action = id
        settings.item1 = None
        settings.item2 = None
        settings.preposition = None
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

def on_enter_inventory_item(item):
    def f(node):
        node.setPalette('yellow')
        if settings.item1 is None:
            settings.item1 = item
        else:
            settings.item2 = item
        refresh_action()
    return f

def on_enter_newkid(node):
    node.setPalette('yellow')

def on_leave_newkid(node):
    node.setPalette('purple')


def on_leave_item(node):
    if settings.item2:
        settings.item2 = None
    else:
        if not settings.preposition:
            settings.item1 = None
    refresh_action()

def on_leave_inventory_item(node):
    node.setPalette('purple')
    if settings.item2:
        settings.item2 = None
    else:
        if not settings.preposition:
            settings.item1 = None
    refresh_action()

def check_delayed_func():
    player_name = settings.characters[settings.player]
    df = data.delayed_funcs[player_name]
    if df:
        df()
        data.delayed_funcs[player_name] = None


def execute_action(node):
    if not settings.item1:
        return
    inventory = data.inventory[settings.characters[settings.player]]
    check_delayed_func()
    script = monkey.Script(id="__player")

    if not settings.item2:
        # one item action
        item_info = data.items[settings.item1]
        if settings.item1 not in inventory:
            scripts.walkToItem(script, settings.item1)
        actions = item_info.get('actions', None)
        scr = getItemScript(settings.item1, settings.action)
        if scr:
            scr[0](script, *scr[1])
        else:
            objs = settings.verbs[settings.action].get('objects', 1)
            if objs == 1:
                # try the default script
                f = getattr(scripts, "_" + settings.action, None)
                if f:
                    f(script)
            else:
                print('FFFFF')
                settings.preposition = settings.verbs[settings.action]['preposition']
                refresh_action()
                return
        monkey.play(script)
        reset_action()
        refresh_action()
    else:
        if settings.action == 'give':
            scripts.walkToItem(script, settings.item2)
            if player_has_item(settings.item1):
                script.add(monkey.actions.CallFunc(scripts.move_item(settings.item1, settings.characters[settings.player], settings.item2)))
                script.add(monkey.actions.CallFunc(refresh_inventory))
        else:
            scr = getItemScript(settings.item1, settings.action, settings.item2)
            if not scr:
                scr = getItemScript(settings.item2, settings.action, settings.item1)
            if not scr:
                # try the default script
                f = getattr(scripts, "_" + settings.action, None)
                if f:
                    f(script)
            else:
                i1_in_inv = settings.item1 in inventory
                i2_in_inv = settings.item1 in inventory
                if i1_in_inv and not i2_in_inv:
                    scripts.walkToItem(script, settings.item2)
                elif i2_in_inv and not i1_in_inv:
                    scripts.walkToItem(script, settings.item1)
                elif not i1_in_inv and not i2_in_inv:
                    if not scr:
                        scripts.walkToItem(script, settings.item2)
                if scr:
                    scr[0](script, *scr[1])
        monkey.play(script)
        reset_action()
        refresh_action()







