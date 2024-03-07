import monkey
import random
import settings
import game_state
from . import castle

from .utils import make_text, set_main_node_active, rm_node, is_within_bounds, \
    move_item_by, _goto_room



def restart_room():
    monkey.close_room()







def zfunc_default (x, y):
    z = 1.0 - y / 166.0
    for wall in game_state.wallz:
        if x >= wall[0] and x <= wall[2]:
            print(wall)
            yl = wall[1] + ((wall[3] - wall[1]) / (wall[2] - wall[0])) * (x - wall[0])
            print(x, y, yl, z)
            if y < yl:
                z += 1
    return z

def look(item):
    if not item:
        msg(id = 95)
    elif item in game_state.inventory:
        s = monkey.Script()
        show_item_detail(s, item)
        monkey.play(s)
    else:
        msg(id=settings.items['items'][item]['desc'] if item else 92)

def pickup(item):
    if not item:
        msg(id=95)
    elif item in game_state.inventory:
        # trying to pickup something you already have
        msg(id=26)
    else:
        # check bounds
        item_desc = settings.items['items'][item]
        can_pick_up = item_desc.get('can_pick_up', False)
        line = settings.items['items'][item].get('pick_up', 96)
        if not can_pick_up:
            msg(id=line, x=item)
        else:
            if is_within_bounds(item):
                # check if item can be picked up
                if can_pick_up:
                    game_state.inventory.add(item)
                    monkey.get_node(game_state.nodes[item]).remove()
                    msg(id = line)
            else:
                # outside bounds
                msg(id=93, x=item)



def msg(id, **kwargs):
    script = monkey.Script()
    message(script, id, **kwargs)
    monkey.play(script)


def message(script, id, **kwargs):
    script.add(monkey.actions.CallFunc(function=set_main_node_active(monkey.NodeState.PAUSED)))
    msg = make_text(id, **kwargs)
    script.add(monkey.actions.Add(id=game_state.Ids.text_node, node=msg))
    wk = monkey.actions.WaitForKey()
    wk.add(settings.Keys.enter, func=rm_node(msg.id))
    script.add(wk)

def push_rock(item):
    if game_state.rock_moved:
        msg(id=17)
    else:
        if is_within_bounds(item):
            game_state.rock_moved = True
            script = monkey.Script()
            message(script, 13)
            script.add(monkey.actions.MoveBy(id=game_state.nodes['rock'], delta=(0,-12), time=1))
            move_item_by(item, (0, -12, 0))
            monkey.play(script)
        else:
            msg(id=93, x=item)

def show_item_detail(script, item_id):
    import factory
    idesc = settings.items['items'][item_id]
    script.add(monkey.actions.CallFunc(function=set_main_node_active(monkey.NodeState.PAUSED)))
    node = monkey.Node()
    node.add(make_text(idesc.get('desc_inventory', idesc['desc'])))
    node.add(factory.make_solid_rect(136, 0, 42, 47, color='000000', z=2))
    node.add(factory.make_outline2_rect(136, 0, 42, 47, color='AA0000',z=2))
    spr = monkey.get_sprite(settings.items['items'][item_id]['img_inventory'])
    spr.set_position(157, 22, 3)
    node.add(spr)
    script.add(monkey.actions.Add(id=game_state.Ids.text_node, node=node))
    wk = monkey.actions.WaitForKey()
    wk.add(settings.Keys.enter, func=rm_node(node.id))
    script.add(wk)



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

def goto_room_y(playee, other, room, pos, dir):
    settings.room = room
    settings.pos = pos
    settings.pos[1] = monkey.get_node(game_state.Ids.player).y
    settings.dir = dir
    monkey.close_room()

def open_door_castle(item):
    s = monkey.Script()
    message(s, 100)
    print(game_state.nodes)
    s.add(monkey.actions.Animate(id=game_state.nodes['door_castle'], anim='open', sync=True))
    s.add(monkey.actions.CallFunc(_goto_room('incastl1', [158, 2, 0], 'n')))
    monkey.play(s)

def climb_oak(item):
    script = monkey.Script()
    message(script, 28)
    script.add(monkey.actions.CallFunc(_goto_room('treetop', [80,3], 'n')))
    monkey.play(script)

def _wolf():
    msg(id=37)
    a = monkey.get_sprite('wolf/wolf')
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

def caught_by_wolf(a,b):
    msg(38)
    c = monkey.get_sprite('wolf/wolf_fight')
    c.set_position(a.x,a.y,a.z)
    a.remove()
    b.remove()
    monkey.get_node(game_state.Ids.game_node).add(c)
    s = monkey.Script()
    s.add(monkey.actions.Delay(5))
    s.add(monkey.actions.Remove(c.id))
    message(s, 0)
    monkey.play(s)
