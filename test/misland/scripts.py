import monkey
import game_state
import settings
import random

def restart_room():
    monkey.close_room()

def zfunc_default (x, y):
    return 1.0 - y / 166.0

def zfunc1(x, y):
    za = 1.0 if y < (108 + 0.593*(x-184)) and y<107 else 0
    return 1.0 - y / 166.0 + za









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



def rm_node(*args):
    def f():
        for id in args:
            monkey.get_node(id).remove()
        set_main_node_active(monkey.NodeState.ACTIVE)()
    return f

def set_main_node_active(value):
    def f():
        #monkey.get_node(settings.text_edit_node).active = value
        monkey.get_node(game_state.Ids.game_node).state = value
    return f
def message(script, id, last=True):
    script.add(monkey.actions.CallFunc(function=set_main_node_active(monkey.NodeState.PAUSED)))
    msg = make_text(id)
    script.add(monkey.actions.Add(id=game_state.Ids.text_node, node=msg))
    wk = monkey.actions.WaitForKey()
    wk.add(settings.Keys.enter, func=rm_node(msg.id))
    script.add(wk)

def message_item(script, item_id):
    import factory
    script.add(monkey.actions.CallFunc(function=set_main_node_active(monkey.NodeState.PAUSED)))

    node = monkey.Node()
    node.add(make_text(game_state.items[item_id]['desc']))
    node.add(factory.make_solid_rect(136, 0, 42, 47, color='000000'))
    node.add(factory.make_outline2_rect(136, 0, 42, 47, color='AA0000',z=1))
    spr = monkey.get_sprite(game_state.items[item_id]['sprite'])
    spr.set_position(157, 22, 1)
    node.add(spr)
    script.add(monkey.actions.Add(id=game_state.Ids.text_node, node=node))
    wk = monkey.actions.WaitForKey()
    wk.add(settings.Keys.enter, func=rm_node(node.id))
    script.add(wk)


    # msg = monkey.actions.Msg(font='kq1main/sierra', text=settings.strings[id], batch='ui', pos=(160, 100, 1.1),#(158, 83, 1.1),
    #     palette=1, parent=settings.text_node, margin=(0,0), halign=1, valign=1, max_width=230,
    #     on_create=make_text_border)
    # if last:
    #     msg.set_on_end(function=set_main_node_active(True))
    # script.add(msg)

def pippo(a, b, c):
    print('entering weppo')

def caught_by_wolf(a,b):
    msg(38)
    c = monkey.get_sprite('007/wolf_fight')
    c.set_position(a.x,a.y,a.z)
    a.remove()
    b.remove()
    monkey.get_node(game_state.Ids.game_node).add(c)
    s = monkey.Script()
    s.add(monkey.actions.Delay(5))
    s.add(monkey.actions.Remove(c.id))
    message(s, 0)
    monkey.play(s)



def _wolf():
    msg(37)
    a = monkey.get_sprite('007/wolf')
    a.set_position(180,10,0)
    a.add_component(monkey.components.NPCSierraController(game_state.Ids.player, 60, 1000, 50,  z_func=settings.z_func))
    a.add_component(monkey.components.Collider(settings.CollisionFlags.foe, settings.CollisionFlags.player, 1,
                                               monkey.shapes.AABB(-5, 5, -1, 1), batch='lines'))
    a.user_data = {
        'on_enter': ['caught_by_wolf']
    }
    monkey.get_node(game_state.Ids.game_node).add(a)


def create_wolf():
    script = monkey.Script()
    script.add(monkey.actions.Delay(random.randint(1, 10)))
    script.add(monkey.actions.CallFunc(_wolf))
    monkey.play(script)




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

def pickup(item, n1, n2):
    if item in game_state.inventory:
        msg(n2)
    else:
        game_state.inventory.add(item)
        msg(n1)

def push_rock():
    game_state.rock_moved = True
    script = monkey.Script()
    message(script, 13)
    script.add(monkey.actions.MoveBy(id=game_state.nodes['rock'], delta=(0,-12), time=1))
    monkey.play(script)

def switch(condition, a1, a2):
    a = a1 if eval(condition) else a2
    s = globals()[a[0]](*a[1:])


def rock_pos():
    if game_state.rock_moved == 1:
        return [236, 21, 0]
    return [236, 33, 0]


#lookout_look_tree = _msg(7)
#lookout_look_castle = _msg(4)

def goto_room(playe, other, room, pos, dir):
    settings.room = room
    settings.pos = pos
    settings.dir = dir
    monkey.close_room()

def goto_room_x(playe, other, room, pos, dir):
    settings.room = room
    settings.pos = pos
    settings.pos[0] = monkey.get_node(game_state.Ids.player).x
    settings.dir = dir
    monkey.close_room()

def goto_room_y(playe, other, room, pos, dir):
    settings.room = room
    settings.pos = pos
    settings.pos[1] = monkey.get_node(game_state.Ids.player).y
    print('fucami',settings.pos)
    settings.dir = dir
    monkey.close_room()


def _goto_room(room, pos, dir):
    def f():
        settings.room = room
        settings.pos = pos
        settings.dir = dir
        monkey.close_room()
    return f


def climb_tree():
    script = monkey.Script()
    message(script, 28)
    script.add(monkey.actions.CallFunc(_goto_room('treetop', [80,3], 'n')))
    monkey.play(script)

def get_dagger():
    if 'dagger' in game_state.inventory:
        msg(26)
    else:
        if game_state.rock_moved:
            game_state.inventory.add('dagger')
            msg(25)
        else:
            msg(27)

