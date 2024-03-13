import monkey
import random
import settings
import game_state
import math
import shapely
from . import castle

from .utils import make_text, set_main_node_active, rm_node, is_within_bounds, \
    move_item_by, _goto_room,interpret, get_item



def restart_room():
    monkey.close_room()







def zfunc_default (x, y):
    z = 1.0 - y / 166.0
    a = 0
    md = -1
    iwall = -1
    cwall = -1
    for wall in game_state.wallz:
        cwall += 1
        bl = wall['baseline']
        if x < bl[0] or x > bl[-2]:
            continue
        for i in range(0, len(bl)-2, 2):
            if x >= bl[i] and x <= bl[i+2]:
                yl = bl[i+1] + ((bl[i+3] - bl[i+1]) / (bl[i+2] - bl[i])) * (x - bl[i])
                if y < yl:
                    if md < 0 or md > (yl - y):
                        md = yl - y
                        iwall = cwall
    if iwall != -1:

        zwall = monkey.get_node(game_state.wallz[iwall]['id']).z
        #print('under wall', iwall, 'corrected by', game_state.wallz[iwall]['z'] + 0.5, zwall)
        z += game_state.wallz[iwall]['z'] + 1.0
    else:
        pass
        #print('no wall',z)
    return z

def look(item):
    if not item:
        msg(id = 95)
    elif item in game_state.inventory:
        s = monkey.Script()
        show_item_detail(s, item)
        monkey.play(s)
    else:
        id = 92
        if item:
            desc = settings.items['items'][item].get('desc')
            if desc:
                id = interpret(desc)
        msg(id=id)

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
                    game_state.collected.add(item)
                    if item in game_state.nodes:
                        monkey.get_node(game_state.nodes[item]).remove()
                    settings.items['items'][item]['room'] = None
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

def create_fairy():
    script = monkey.Script()
    script.add(monkey.actions.Delay(random.randint(1, 10)))
    script.add(monkey.actions.CallFunc(_fairy))
    monkey.play(script)

def _fairy():
    a = monkey.get_sprite('fairy/fairy')
    a.set_position(126,86,0)
    a.add_component(monkey.components.NPCSierraController(game_state.Ids.player, 60, 1000, 10, z_func=settings.z_func,
                                                          walk_e='walk', walk_n='walk', walk_s='walk'))
    monkey.get_node(game_state.Ids.game_node).add(a)
    spell_script = monkey.Script()
    message(spell_script, 39)
    spell_script.add(monkey.actions.CallFunc(spellStart))
    spell_script.add(monkey.actions.Delay(2))
    message(spell_script, 40)
    spell_script.add(monkey.actions.Remove(a.id))
    monkey.play(spell_script)

def spellEnd():
    game_state.protective_spell = 0
    msg(41)

def spellStart():
    print ('************************************************************************************')
    game_state.protective_spell = 1
    monkey.getClock().addEvent(True, True, 15, spellEnd)

def create_goat():
    setup3d()
    if game_state.goat_east == 0:
        _goat(226,78)

def _goat(x, y):
    a = monkey.get_sprite('sprites/goat')
    a.set_position(x, y, 0)
    angle = random.uniform(0, 2.0 * math.pi)
    vector = [math.cos(angle), math.sin(angle)]
    a.add_component(monkey.components.NPCSierraController(game_state.Ids.player, 60, 1000, 10, z_func=settings.z_func,
                                                          flip=1,
                                                          direction=vector))
    a.add_component(monkey.components.Collider(settings.CollisionFlags.foe, settings.CollisionFlags.foe_hotspot, 0,
                                               monkey.shapes.AABB(-5, 5, -1, 1), batch='lines'))
    monkey.get_node(game_state.Ids.game_node).add(a)
def create_goat_e():
    setup3d()
    if game_state.goat_east == 1:
        _goat(114,57)


def goat_east(goat, a):
    game_state.goat_east = 1
    goat.remove()

def _gateMove(x, y, line):
    settings.items['items']['gate']['pos'][0] = 32 + x
    settings.items['items']['gate']['baseline'] =[30 + x,12,86+x,12]
    a = monkey.get_node(game_state.nodes['gate'])
    s = monkey.Script()
    message(s, id=line)
    s.add(monkey.actions.MoveBy(id=a.id, delta=(y, 0), time=1))
    monkey.play(s)

def open_gate(item):
    _gateMove(60, 60, 106)

def close_gate(item):
    _gateMove(0, -60, 107)

#lookin_stump = msg(1)


def setup3d():

    # sort all walls
    in_edges = dict()
    out_edges = dict()
    z = dict()
    for i in range(0, len(game_state.wallz)):
        in_edges[i] = []
        out_edges[i] = []
        z[i] = 0
    for i in range(0, len(game_state.wallz)-1):
        wi = game_state.wallz[i]['baseline']
        lsi = shapely.geometry.LineString([[wi[i], wi[i + 1]] for i in range(0, len(wi), 2)])
        for j in range(i+1, len(game_state.wallz)):
            wj = game_state.wallz[j]['baseline']
            lsj = shapely.geometry.LineString([[wj[i], wj[i + 1]] for i in range(0, len(wj), 2)])
            print('checking walls',i,j)
            if lsj.intersects(shapely.geometry.LineString([ [wi[0], wi[1]], [wi[0], 0]] )) or \
                lsj.intersects(shapely.geometry.LineString([ [wi[-2], wi[-1]], [wi[-2], 0]] )):
                print(j,'below',i)
                in_edges[j].append(i)
                out_edges[i].append(j)
            elif lsi.intersects(shapely.geometry.LineString([ [wj[0], wj[1]], [wj[0], 0]] )) or \
                lsi.intersects(shapely.geometry.LineString([ [wj[-2], wi[-1]], [wj[-2], 0]] )):
                print(game_state.wallz[0],'below',game_state.wallz[1])
                in_edges[i].append(j)
                out_edges[j].append(i)
    print(in_edges)
    # items with 0 in edges
    sources = [x for x in range(0, len(game_state.wallz)) if not in_edges[x]]
    l = sources
    while l:
        cw = l.pop(0)
        for x in out_edges[cw]:
            if z[x] < z[cw]+1:
                z[x] = z[cw]+1
                l.append(x)
    print(sources)
    print(z)
    for i in range(0, len(game_state.wallz)):
        node = monkey.get_node(game_state.wallz[i]['id'])
        monkey.get_node(game_state.wallz[i]['id']).set_position(node.x, node.y, 2*z[i]+1)
        game_state.wallz[i]['z'] = 2*z[i]+1
    print(out_edges)

def start_swim(player, other):
    player.set_model(monkey.models.getSprite('sprites/graham_swim'), batch='sprites')

def end_swim(player, other):
    player.set_model(monkey.models.getSprite('sprites/graham'), batch='sprites')


def open_walnut(item):
    msg(id=66)
    item = get_item('walnut')
    item['desc_inventory'] = 67
    item['img_inventory'] = 'sprites/item_gold_walnut'
    item['name'] = 69

def show_carrot(item):
    player = monkey.get_node(game_state.Ids.player)
    #a = monkey.models.MultiSprite()
    #a.addSprite(monkey.models.getSprite('sprites/graham'))
    #a.addSprite(monkey.models.getSprite('sprites/goat'))
    #player.set_model(a, batch='sprites')

    player.set_model(monkey.models.getSprite('sprites/graham_goat'), batch='sprites')

