import monkey
import game_state
import settings


def restart_room():
    monkey.close_room()

def _text(id,x,y, **kwargs):
    a=monkey.Text(batch='ui', font='sierra', anchor=kwargs.get('anchor',monkey.ANCHOR_CENTER), text=settings.strings[id])
    a.set_position(x,y,5)
    return a

def inventory_row_up():
    if game_state.inventory_selected > 1:
        monkey.get_node(game_state.inventory_nodes[game_state.inventory_selected]).get_model().set_palette(0)
        game_state.inventory_selected -= 2
        monkey.get_node(game_state.inventory_nodes[game_state.inventory_selected]).get_model().set_palette(2)

def inventory_row_down():
    if game_state.inventory_selected < len(game_state.inventory)-1:
        monkey.get_node(game_state.inventory_nodes[game_state.inventory_selected]).get_model().set_palette(0)
        game_state.inventory_selected = min(len(game_state.inventory)-1, game_state.inventory_selected + 2)
        monkey.get_node(game_state.inventory_nodes[game_state.inventory_selected]).get_model().set_palette(2)

def inventory_next():
    print(game_state.inventory_selected, len(game_state.inventory))
    if game_state.inventory_selected < len(game_state.inventory)-1:
        monkey.get_node(game_state.inventory_nodes[game_state.inventory_selected]).get_model().set_palette(0)
        game_state.inventory_selected += 1
        monkey.get_node(game_state.inventory_nodes[game_state.inventory_selected]).get_model().set_palette(2)

def inventory_previous():
    if game_state.inventory_selected > 0:
        monkey.get_node(game_state.inventory_nodes[game_state.inventory_selected]).get_model().set_palette(0)
        game_state.inventory_selected -= 1
        monkey.get_node(game_state.inventory_nodes[game_state.inventory_selected]).get_model().set_palette(2)

def inventory_exit():
    game_state.inventory_nodes = []
    inv = monkey.get_node(game_state.Ids.inventory_node).remove()
    monkey.get_node(game_state.Ids.game_node).active = True
    monkey.get_node(game_state.Ids.text_node).active = True




def show_inventory():
    monkey.get_node(game_state.Ids.game_node).active = False
    monkey.get_node(game_state.Ids.text_node).active = False
    inv = monkey.Node()
    game_state.Ids.inventory_node = inv.id
    #inv.active=True
    inv.add(_text(19, 160, 196))
    inv.add(_text(20,160,4))
    kb = monkey.Node()
    k1 = monkey.components.Keyboard()
    k1.add(settings.Keys.up, 1, 0, inventory_row_up)
    k1.add(settings.Keys.down, 1, 0, inventory_row_down)
    k1.add(settings.Keys.right, 1, 0, inventory_next)
    k1.add(settings.Keys.left, 1, 0, inventory_previous)
    k1.add_fallback(inventory_exit)
    #k1.add(settings.Keys.inventory, 1, 0, scripts.show_inventory)
    kb.add_component(k1)
    inv.add(kb)
    y = 180
    if game_state.inventory:
        i = 0
        for item in game_state.inventory:
            inode = _text(item, 8 if i % 2 == 0 else 312, y, anchor=monkey.ANCHOR_TOPLEFT if i % 2 == 0 else monkey.ANCHOR_TOPRIGHT)
            inv.add(inode)
            game_state.inventory_nodes.append(inode.id)
            i += 1
            if i % 2 == 0:
                y -= 8
        monkey.get_node(game_state.inventory_nodes[0]).get_model().set_palette(2)
    else:
        inv.add(_text(21, 160, y))
    monkey.get_node(game_state.Ids.root).add(inv)



def make_text(string_id):
    msg = monkey.Text(batch='sprites', font='sierra', anchor=monkey.ANCHOR_CENTER,
                      text=settings.strings[string_id],
                      width=29 * 8, pal=1)
    msg.set_position(160, 100, 2)
    border = monkey.Node()
    border.set_model(monkey.models.from_shape('tri',
                                              monkey.shapes.AABB(-10, msg.size[0] + 10, -msg.size[1] - 5, 5),
                                              (1, 1, 1, 1),
                                              monkey.FillType.Solid))
    border.set_position(0, 0, -0.1)
    border2 = monkey.Node()
    border2.set_model(monkey.models.from_shape('lines',
                                               monkey.shapes.AABB(-5, msg.size[0] + 5, -msg.size[1] - 3, 3),
                                               monkey.from_hex('AA0000'),
                                               monkey.FillType.Outline))
    border2.set_position(0, 0, -0.01)
    border3 = monkey.Node()
    border3.set_model(monkey.models.from_shape('lines',
                                               monkey.shapes.AABB(-6, msg.size[0] + 6, -msg.size[1] - 3, 3),
                                               monkey.from_hex('AA0000'),
                                               monkey.FillType.Outline))
    border3.set_position(0, 0, -0.01)
    print('ccc',border.id)
    print('ccd',border2.id)
    print('cce',border3.id)
    msg.add(border)
    msg.add(border2)
    msg.add(border3)
    return msg



def rm_node(id):
    def f():
        print(id)
        monkey.get_node(id).remove()
        set_main_node_active(True)()
    return f

def set_main_node_active(value):
    def f():
        #monkey.get_node(settings.text_edit_node).active = value
        monkey.get_node(game_state.Ids.game_node).active=value
    return f
def message(script, id, last=True):
    script.add(monkey.actions.CallFunc(function=set_main_node_active(False)))

    msg = make_text(id)
    script.add(monkey.actions.Add(id=game_state.Ids.text_node, node=msg))
    wk = monkey.actions.WaitForKey()
    wk.add(settings.Keys.enter, func=rm_node(msg.id))
    script.add(wk)

    # msg = monkey.actions.Msg(font='kq1main/sierra', text=settings.strings[id], batch='ui', pos=(160, 100, 1.1),#(158, 83, 1.1),
    #     palette=1, parent=settings.text_node, margin=(0,0), halign=1, valign=1, max_width=230,
    #     on_create=make_text_border)
    # if last:
    #     msg.set_on_end(function=set_main_node_active(True))
    # script.add(msg)

def pippo(a, b, c):
    print('entering weppo')




def pippa(a, b):
    print('leaving jiko')

def _drown(player, x, y):
    script = monkey.Script()
    script.add(monkey.actions.SierraEnable(id=player.id, value=False))
    script.add(monkey.actions.Move(id=player.id, position=(x, y, player.z), speed=0))
    script.add(monkey.actions.Animate(id=player.id, anim='drown'))
    message(script, 1)
    script.add(monkey.actions.Delay(time=2))
    message(script, 0)
    monkey.play(script)


def drown(player, other):
    _drown(player, player.x, 4)

def drown2(player, other):
    _drown(player, 188, 38)

def msg(n):
    script = monkey.Script()
    message(script, n)
    monkey.play(script)

def push_rock():
    game_state.rock_moved = True
    script = monkey.Script()
    message(script, 13)
    script.add(monkey.actions.MoveBy(id=game_state.nodes['rock'], delta=(0,-12), time=1))
    monkey.play(script)

def switch(condition, a1, a2):
    a = a1 if eval(condition) else a2
    s = globals()[a[0]](*a[1:])




#lookout_look_tree = _msg(7)
#lookout_look_castle = _msg(4)

def goto_room(playe, other, room, pos, dir):
    settings.room=room
    settings.pos=pos
    settings.dir = dir
    monkey.close_room()