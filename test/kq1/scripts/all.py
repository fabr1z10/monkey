import monkey
import random
import settings
import game_state
import math
import shapely
from . import castle

from .utils import make_text, set_main_node_active, rm_node, is_within_bounds, \
    move_item_by, _goto_room, get_item, addNode


def interpret(s):
    if isinstance(s, list):
        return globals()[s[0]](*s[1:])
    return s

def rand(*args, **kwargs):
    return random.choice(args)

def cond(*args, **kwargs):
    return args[1] if eval(args[0]) else args[2]


def restart_room():
    monkey.close_room()

def func_follow_player():
    player= monkey.get_node(game_state.Ids.player)
    return (player.x, player.y)





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
        print('ADJUSTED BY: ',game_state.wallz[iwall])
    else:
        pass
        #print('no wall',z)

    return z

def look(*args):
    if args[0] in game_state.inventory:
        s = monkey.Script()
        show_item_detail(s, args[0])
        monkey.play(s)
    else:
        msg(id=args[1])


def open(item):
    s = monkey.Script()
    s.add(monkey.actions.Animate(id=game_state.nodes[item], anim='open', sync=True))
    settings.items['items'][item]['open'] = 1
    monkey.play(s)

def close(item):
    s = monkey.Script()
    s.add(monkey.actions.Animate(id=game_state.nodes[item], anim='open', sync=True, backwards=True))
    settings.items['items'][item]['open'] = 0
    monkey.play(s)

def pickup(item, line):
    if item in game_state.inventory:
        # trying to pickup something you already have
        msg(id=26)
    else:
        # check bounds
        item_desc = settings.items['items'][item]
        #can_pick_up = item_desc.get('can_pick_up', False)
        #line = settings.items['items'][item].get('pick_up', 96)
        #if not can_pick_up:
        #    msg(id=line, x=item)
        #else:
        if is_within_bounds(item):
            # check if item can be picked up
            # if can_pick_up:
            game_state.inventory.add(item)
            game_state.collected.add(item)
            if item in game_state.nodes:
                monkey.get_node(game_state.nodes[item]).remove()
                settings.items['items'][item]['room'] = None
            msg(id = line)
            #else:
            # outside bounds
            #    msg(id=93, x=item)



def msg(id, **kwargs):
    script = monkey.Script()
    message(script, id, **kwargs)
    monkey.play(script)

def msgi(item, id, **kwargs):
    script = monkey.Script()
    eid = id if item in game_state.inventory else 68
    message(script, eid, **kwargs)
    monkey.play(script)

def message(script, id, **kwargs):
    script.add(monkey.actions.CallFunc(function=set_main_node_active(monkey.NodeState.PAUSED)))
    msg = make_text(id, **kwargs)
    script.add(monkey.actions.Add(id=game_state.Ids.text_node, node=msg))
    wk = monkey.actions.WaitForKey()
    wk.add(settings.Keys.enter, func=rm_node(msg.id))
    script.add(wk)

def push_rock():
    if game_state.rock_moved:
        msg(id=17)
    else:
        if is_within_bounds('rock'):
            game_state.rock_moved = True
            script = monkey.Script()
            message(script, 13)
            script.add(monkey.actions.MoveBy(id=game_state.nodes['rock'], delta=(0,-12), time=1))
            move_item_by('rock', (0, -12, 0))
            monkey.play(script)
        else:
            msg(id=93, x='rock')

def show_item_detail(script, item_id):
    import factory
    idesc = settings.items['items'][item_id]['inventory']
    script.add(monkey.actions.CallFunc(function=set_main_node_active(monkey.NodeState.PAUSED)))
    node = monkey.Node()
    node.add(make_text(idesc.get('desc_inventory', idesc['desc'])))
    node.add(factory.make_solid_rect(136, 0, 42, 47, color='000000', z=2))
    node.add(factory.make_outline2_rect(136, 0, 42, 47, color='AA0000',z=2))
    spr = monkey.get_sprite(idesc['image'])
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

def open_door_castle():
    s = monkey.Script()
    message(s, 100)
    print(game_state.nodes)
    s.add(monkey.actions.Animate(id=game_state.nodes['door_castle'], anim='open', sync=True))
    s.add(monkey.actions.CallFunc(_goto_room('incastl1', [158, 2, 0], 'n')))
    monkey.play(s)

def open_door_witch_house():
    s = monkey.Script()
    s.add(monkey.actions.Animate(id=game_state.nodes['door'], anim='open', sync=True))
    s.add(monkey.actions.CallFunc(_goto_room('witchous', [100,5], 's')))
    monkey.play(s)

def change_room(*args):
    script = monkey.Script()
    if len(args) > 3:
        message(script, args[3])
    #script.add(monkey.actions.CallFunc(_goto_room('treetop', [80,3], 'n')))
    script.add(monkey.actions.CallFunc(_goto_room(args[0], args[1], args[2])))
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

def pino():
    a = monkey.get_sprite('sprites/goat')
    a.set_position(160,150,0)
    a.add_component(monkey.components.NPCSierraFollow(game_state.Ids.player, z_func=settings.z_func))
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
    game_state.nodes['goat'] = a.id
    monkey.get_node(game_state.Ids.game_node).add(a)
def create_goat_e():
    setup3d()
    if game_state.goat_east == 1:
        _goat(114,57)


def goat_move(goat, value):
    game_state.goat_east = value
    del game_state.nodes['goat']
    goat.remove()


def goat_east(goat, a):
    goat_move(goat, 1)

def goat_west(goat, a):
    goat_move(goat, 0)

def recompute_walkarea():
    game_state.walkArea.recompute()

def _gateMove(x, y, line):
    settings.items['items']['gate']['pos'][0] = 32 + x
    settings.items['items']['gate']['baseline'] =[30 + x,12,86+x,12]
    a = monkey.get_node(game_state.nodes['gate'])

    s = monkey.Script()
    message(s, id=line)
    s.add(monkey.actions.MoveBy(id=a.id, delta=(y, 0), time=1))
    s.add(monkey.actions.CallFunc(recompute_walkarea))
    monkey.play(s)

def open_gate():
    if game_state.gate_open == 0:
        game_state.gate_open = 1
        _gateMove(60, 60, 106)

def close_gate():
    if game_state.gate_open == 1:
        game_state.gate_open = 0
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
    #print(out_edges)
    if game_state.goat_follow == 1:
        a = monkey.get_sprite('sprites/goat')
        p = monkey.get_node(game_state.Ids.player)
        game_state.nodes['goat'] = a.id
        a.set_position(p.x, p.y, 0)
        a.add_component(monkey.components.NPCSierraFollow(func_follow_player, 50, 1, z_func=settings.z_func))
        monkey.get_node(game_state.Ids.game_node).add(a)



def start_swim(player, other):
    player.set_model(monkey.models.getSprite('sprites/graham_swim'), batch='sprites')

def end_swim(player, other):
    player.set_model(monkey.models.getSprite('sprites/graham'), batch='sprites')


def open_walnut():
    msg(id=66)
    item = get_item('walnut')
    item['inventory']['desc'] = 67
    item['inventory']['image'] = 'sprites/item_gold_walnut'
    item['name'] = 69

def show_carrot(item):
    # check if goat is here
    if 'goat' in game_state.nodes:
        player = monkey.get_node(game_state.Ids.player)
        goat = monkey.get_node(game_state.nodes['goat'])
        if monkey.shapes.check_los((player.x, player.y), (goat.x, goat.y), 2):
            goat.remove()
            msg(id=113)
            player.set_model(monkey.models.getSprite('sprites/graham_goat'), batch='sprites')
        else:
            msg(id=114)
    else:
        msg(id=112)
    return


    #a = monkey.models.MultiSprite()
    #a.addSprite(monkey.models.getSprite('sprites/graham'))
    #a.addSprite(monkey.models.getSprite('sprites/goat'))
    #player.set_model(a, batch='sprites')

def drown(player, other, x, y, line):
    _drown(player, x, y)

def drownx(player, other, y, line):
    _drown(player, player.x, y, line)

def gigio():
    return (random.randint(51,100), random.randint(100,114))

def goat_attack():
    s = monkey.Script()
    message(s, 121)
    s.add(monkey.actions.CallFunc(lambda: monkey.get_node(game_state.nodes['troll_block']).remove()))
    s.add(monkey.actions.CallFunc(lambda: monkey.get_node(game_state.nodes['goat']).sendMessage(id="setFunc", func=None)))
    monkey.play(s)


def enter_troll_bridge(player, other):
    other.remove()
    troll = monkey.get_sprite('sprites/troll')
    troll.set_position(75,100,0)
    troll.add_component(monkey.components.WalkableCharacter(100, anim_dir=False, idle_anim='walk_s', walk_anim='walk_s'))

    addNode(troll)

    s = monkey.Script()
    message(s, 119)
    if game_state.goat_follow == 1:
        s.add(monkey.actions.CallFunc(goat_attack))

    s.add(monkey.actions.WalkDynamic(troll.id, gigio), loop=True)

    monkey.play(s)

    #msg(id=119)

def _drown(player, x, y, line):
    script = monkey.Script()
    script.add(monkey.actions.SierraEnable(id=player.id, value=False))
    script.add(monkey.actions.Move(id=player.id, position=(x, y, 1-y/166), speed=0))
    script.add(monkey.actions.Animate(id=player.id, anim='drown'))
    message(script, line)
    script.add(monkey.actions.Delay(time=2))
    message(script, 0)
    monkey.play(script)



