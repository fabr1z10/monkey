import monkey
import random
import settings
import game_state
import math
import shapely

from .utils import make_text, set_main_node_active, rm_node, is_within_bounds, \
    move_item_by, _goto_room, get_item, addNode, id_to_string
from .actions import removeNode


def addScore(score):
    game_state.score += score
    updateScore()
def updateScore():
    monkey.get_node(game_state.Ids.score_label).updateText(id_to_string(132))


def updateSound():
    monkey.get_node(game_state.Ids.sound_label).updateText(id_to_string(133))


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
    player = monkey.get_node(game_state.Ids.player)
    return (player.x, player.y)


def func_random(x0, x1, y0, y1):
    def f():
        return (random.randint(x0, x1), random.randint(y0, y1))

    return f


def zfunc_default(x, y):
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
        for i in range(0, len(bl) - 2, 2):
            if x >= bl[i] and x <= bl[i + 2]:
                yl = bl[i + 1] + ((bl[i + 3] - bl[i + 1]) / (bl[i + 2] - bl[i])) * (x - bl[i])
                if y < yl:
                    if md < 0 or md > (yl - y):
                        md = yl - y
                        iwall = cwall
    if iwall != -1:

        zwall = monkey.get_node(game_state.wallz[iwall]['id']).z
        # print('under wall', iwall, 'corrected by', game_state.wallz[iwall]['z'] + 0.5, zwall)
        z += game_state.wallz[iwall]['z'] + 1.0
    else:
        pass
        # print('no wall',z)

    return z


def look(*args):
    if args[0] in game_state.inventory:
        s = monkey.Script()
        show_item_detail(s, args[0])
        monkey.play(s)
    else:
        msg(args[1])


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
        # can_pick_up = item_desc.get('can_pick_up', False)
        # line = settings.items['items'][item].get('pick_up', 96)
        # if not can_pick_up:
        #    msg(id=line, x=item)
        # else:
        if is_within_bounds(item):
            # check if item can be picked up
            # if can_pick_up:
            game_state.inventory.add(item)
            game_state.collected.add(item)
            if item in game_state.nodes:
                monkey.get_node(game_state.nodes[item]).remove()
                settings.items['items'][item]['room'] = None
            msg(line)
            # else:
            # outside bounds
            #    msg(id=93, x=item)


def msg(*args, **kwargs):
    script = monkey.Script()
    for arg in args:
        message(script, arg, **kwargs)
    monkey.play(script)



def last_wrong_guess():
    monkey.get_node(game_state.nodes['gnome']).remove()
    s = monkey.Script()
    message(s, 131)
    monkey.play(s)


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
            script.add(monkey.actions.MoveBy(id=game_state.nodes['rock'], delta=(0, -12), time=1))
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
    node.add(factory.make_outline2_rect(136, 0, 42, 47, color='AA0000', z=2))
    spr = monkey.get_sprite(idesc['image'])
    spr.set_position(157, 22, 3)
    node.add(spr)
    script.add(monkey.actions.Add(id=game_state.Ids.text_node, node=node))
    wk = monkey.actions.WaitForKey()
    wk.add(settings.Keys.enter, func=rm_node(node.id))
    script.add(wk)


def goto_room(playe, other, room, pos, dir):
    settings.previous_room = settings.room
    settings.room = room
    settings.pos = pos
    settings.dir = dir
    monkey.close_room()


def goto_room_x(playe, other, room, pos, dir):
    settings.previous_room = settings.room
    settings.room = room
    settings.pos = pos
    settings.pos[0] = monkey.get_node(game_state.Ids.player).x
    settings.dir = dir
    monkey.close_room()


def goto_room_y(playee, other, room, pos, dir):
    settings.previous_room = settings.room

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
    s.add(monkey.actions.CallFunc(_goto_room('witchous', [100, 5], 's')))
    monkey.play(s)


def change_room(*args):
    script = monkey.Script()
    if len(args) > 3:
        message(script, args[3])
    # script.add(monkey.actions.CallFunc(_goto_room('treetop', [80,3], 'n')))
    script.add(monkey.actions.CallFunc(_goto_room(args[0], args[1], args[2])))
    monkey.play(script)


def pino():
    a = monkey.get_sprite('sprites/goat')
    a.set_position(160, 150, 0)
    a.add_component(monkey.components.NPCSierraFollow(game_state.Ids.player, z_func=settings.z_func))
    monkey.get_node(game_state.Ids.game_node).add(a)


def create_foe_script(f):
    script = monkey.Script()
    script.add(monkey.actions.Delay(random.randint(1, 10)))
    script.add(monkey.actions.CallFunc(f))
    monkey.play(script)


def create_wolf():
    create_foe_script(create_foe('wolf', 'wolf/wolf', 101, 76, 60, 'caught_by_wolf', 37))


def create_sorcerer():
    create_foe_script(create_foe('sorcerer', 'sprites/sorcerer', 88, 40, 50, 'caught_by_sorcerer', 146, anim_dir=False))


def create_fairy():
    if game_state.protective_spell == 1:
        return
    spell_script = monkey.Script()
    message(spell_script, 39)
    spell_script.add(monkey.actions.CallFunc(spellStart))
    spell_script.add(monkey.actions.Delay(5))
    message(spell_script, 40)
    spell_script.add(removeNode('fairy'))
    create_foe_script(create_foe('fairy', 'fairy/fairy', 128, 86, 10, None, -1,
                                 anim_dir=False, func_ai=func_random(108, 208, 50, 100), on_create=spell_script))


def caught_by_wolf(a, b):
    msg(38)
    c = monkey.get_sprite('wolf/wolf_fight')
    c.set_position(a.x, a.y, a.z)
    a.remove()
    b.remove()
    monkey.get_node(game_state.Ids.game_node).add(c)
    s = monkey.Script()
    s.add(monkey.actions.Delay(5))
    s.add(monkey.actions.Remove(c.id))
    message(s, 0)
    monkey.play(s)


def caught_by_sorcerer(a, b):
    s = monkey.Script()
    message(s, 147)
    s.add(monkey.actions.SierraEnable(game_state.Ids.player, False))
    s.add(monkey.actions.CallFunc(lambda: monkey.get_node(game_state.nodes['sorcerer']).
                                  sendMessage(id="setFunc", func=None)))
    s.add(monkey.actions.Walk(game_state.nodes['sorcerer'], (316, 50)))
    s.add(removeNode('sorcerer'))
    s.add(monkey.actions.Delay(10))
    message(s, 148)
    s.add(monkey.actions.SierraEnable(game_state.Ids.player, True))
    monkey.play(s)


def create_gnome():
    a = monkey.get_sprite('sprites/gnome')
    a.set_position(77, 53, 0)
    a.add_component(monkey.components.WalkableCharacter(10, anim_dir=False, idle_anim='walk',
                                                        walk_anim='walk', z_func=settings.z_func))
    monkey.get_node(game_state.Ids.game_node).add(a)
    game_state.nodes['gnome'] = a.id
    s = monkey.Script()
    message(s, 123)
    s.add(monkey.actions.WalkDynamic(a.id, gigio), loop=True)
    monkey.play(s)


# this function creates a function generating a foe following player. You can specify the initial position,
# the speed, the sprite and the callback function to call if the foe catches the player
def create_foe(id: str, sprite: str, x: float, y: float, speed: float, callback: str, msg: int, **kwargs):
    def f():
        # if anim_dir is True, the sprites need to h
        anim_dir = kwargs.get('anim_dir', True)
        func_ai = kwargs.get('func_ai', func_follow_player)
        call_every = kwargs.get('period', 1)
        on_create = kwargs.get('on_create', None)
        a = monkey.get_sprite(sprite)
        a.set_position(x, y, 0)
        walk_anim= kwargs.get('walk_anim', 'walk')
        idle_anim = kwargs.get('idle_anim', 'walk')
        a.add_component(monkey.components.NPCSierraFollow(func_ai, speed, call_every, z_func=settings.z_func,
                                                          anim_dir=anim_dir, walk_anim=walk_anim, idle_anim=idle_anim))
        collide = kwargs.get('collider', False)
        if collide:
            flag = kwargs.get('flag', settings.CollisionFlags.foe)
            mask = kwargs.get('mask', settings.CollisionFlags.player)
            a.add_component(monkey.components.Collider(flag, mask, 1,
                                                       monkey.shapes.AABB(-5, 5, -1, 1), batch='lines'))
            if callback:
                a.user_data = {
                    'on_enter': [callback]
                }
        game_state.nodes[id] = a.id
        monkey.get_node(game_state.Ids.game_node).add(a)
        if msg != -1:
            s = monkey.Script()
            message(s, msg)
            monkey.play(s)
        if on_create:
            monkey.play(on_create)

    return f


def spellEnd():
    game_state.protective_spell = 0
    msg(41)


def spellStart():
    print('************************************************************************************')
    game_state.protective_spell = 1
    monkey.getClock().addEvent(True, True, 15, spellEnd)




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

def saxx(k,p,m):
    exit(1)
    print('ok')


def create_goat():
    setup3d()
    if game_state.goat_follow == 0 and game_state.goat_east == 0:
        create_foe('goat', 'sprites/goat', 226, 78, 50, None, -1, collider=True, idle_anim='idle',
            anim_dir=True, mask=settings.CollisionFlags.foe_hotspot, period=100, func_ai=func_random(0, 316, 0, 120))()

def create_goat_e():
    setup3d()
    if game_state.goat_follow == 0 and game_state.goat_east == 1:
        create_foe('goat', 'sprites/goat', 114, 57, 50, None, -1, collider=True, idle_anim='idle',
            anim_dir=True, mask=settings.CollisionFlags.foe_hotspot, period=100, func_ai=func_random(0, 316, 0, 120))()
        #a = monkey.get_node(game_state.nodes['goat'])

        #_goat(114, 57)


def goat_move(goat, value):
    game_state.goat_east = value
    del game_state.nodes['goat']
    goat.remove()


def goat_east(hotspot, goat):
    goat_move(goat, 1)


def goat_west(hotspot, goat):
    goat_move(goat, 0)


def recompute_walkarea():
    game_state.walkArea.recompute()


def _gateMove(x, y, line):
    settings.items['items']['gate']['pos'][0] = 32 + x
    settings.items['items']['gate']['baseline'] = [30 + x, 12, 86 + x, 12]
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


# lookin_stump = msg(1)


def setup3d():
    updateScore()
    updateSound()
    # sort all walls
    in_edges = dict()
    out_edges = dict()
    z = dict()
    for i in range(0, len(game_state.wallz)):
        in_edges[i] = []
        out_edges[i] = []
        z[i] = 0
    for i in range(0, len(game_state.wallz) - 1):
        wi = game_state.wallz[i]['baseline']
        lsi = shapely.geometry.LineString([[wi[i], wi[i + 1]] for i in range(0, len(wi), 2)])
        for j in range(i + 1, len(game_state.wallz)):
            wj = game_state.wallz[j]['baseline']
            lsj = shapely.geometry.LineString([[wj[i], wj[i + 1]] for i in range(0, len(wj), 2)])
            print('checking walls', i, j)
            if lsj.intersects(shapely.geometry.LineString([[wi[0], wi[1]], [wi[0], 0]])) or \
                    lsj.intersects(shapely.geometry.LineString([[wi[-2], wi[-1]], [wi[-2], 0]])):
                print(j, 'below', i)
                in_edges[j].append(i)
                out_edges[i].append(j)
            elif lsi.intersects(shapely.geometry.LineString([[wj[0], wj[1]], [wj[0], 0]])) or \
                    lsi.intersects(shapely.geometry.LineString([[wj[-2], wi[-1]], [wj[-2], 0]])):
                print(game_state.wallz[0], 'below', game_state.wallz[1])
                in_edges[i].append(j)
                out_edges[j].append(i)
    print(in_edges)
    # items with 0 in edges
    sources = [x for x in range(0, len(game_state.wallz)) if not in_edges[x]]
    l = sources
    while l:
        cw = l.pop(0)
        for x in out_edges[cw]:
            if z[x] < z[cw] + 1:
                z[x] = z[cw] + 1
                l.append(x)
    print(sources)
    print(z)
    for i in range(0, len(game_state.wallz)):
        node = monkey.get_node(game_state.wallz[i]['id'])
        monkey.get_node(game_state.wallz[i]['id']).set_position(node.x, node.y, 2 * z[i] + 1)
        game_state.wallz[i]['z'] = 2 * z[i] + 1
    # print(out_edges)
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
    if 'walnut' in game_state.inventory:
        msg(66)
        item = get_item('walnut')
        item['inventory']['desc'] = 67
        item['inventory']['image'] = 'sprites/item_gold_walnut'
        item['name'] = 69
    else:
        msg(68)


def show_carrot():
    # check if goat is here
    if 'goat' in game_state.nodes and game_state.goat_follow == 0:
        game_state.goat_follow = 1
        player = monkey.get_node(game_state.Ids.player)
        goat = monkey.get_node(game_state.nodes['goat'])
        #if monkey.shapes.check_los((player.x, player.y), (goat.x, goat.y), 2):
        #    goat.remove()
        #    msg(id=113)
        #    player.set_model(monkey.models.getSprite('sprites/graham_goat'), batch='sprites')
        #else:
        msg(113)
        goat.sendMessage(id='setFunc', func=func_follow_player)
    else:
        msg(112)


def drown(player, other, x, y, line):
    _drown(player, x, y, line)


def drownx(player, other, y, line):
    _drown(player, player.x, y, line)


def drowny(player, other, x, line):
    _drown(player, x, player.y, line)


def gigio():
    return (random.randint(51, 100), random.randint(100, 114))


def talk_man():
    msg(id=126)
    game_state.parser_override = 'guess_name'


def goat_attack():
    game_state.goat_follow = 0
    game_state.troll_gone = 1
    s = monkey.Script()
    message(s, 121)
    print(game_state.nodes)
    print('figa')
    s.add(removeNode('troll_block'))
    s.add(monkey.actions.CallFunc(lambda: game_state.walkArea.recompute()))
    s.add(
        monkey.actions.CallFunc(lambda: monkey.get_node(game_state.nodes['goat']).sendMessage(id="setFunc", func=None)))
    s.add(monkey.actions.Walk(game_state.nodes['goat'], (75, 106)))
    message(s, 122)
    s.add(monkey.actions.CallFunc(lambda: monkey.kill('troll')))
    s.add(removeNode('troll'))
    s.add(monkey.actions.Walk(game_state.nodes['goat'], (75, 0)))
    s.add(removeNode('goat'))

    # kill troll script!

    monkey.play(s)


def _addTroll(x, y, anim):
    troll = monkey.get_sprite('sprites/troll')
    troll.set_position(x, y, 0)
    troll.add_component(monkey.components.WalkableCharacter(50, anim_dir=False, idle_anim=anim, flip_horizontal=False,
                                                            walk_anim=anim, z_func=settings.z_func))
    game_state.nodes['troll'] = troll.id
    addNode(troll)
    return troll.id


def enter_troll_bridge_side(player, other):
    other.remove()
    id = _addTroll(50, 100, 'walk_e')
    s = monkey.Script(id='troll')
    message(s, 134)
    s.add(monkey.actions.WalkDynamic(id, gigio), loop=True)
    monkey.play(s)


def enter_troll_bridge(player, other):
    other.remove()
    id = _addTroll(75, 100, 'walk_s')

    s = monkey.Script(id='troll')
    message(s, 119)
    if game_state.goat_follow == 1:
        s.add(monkey.actions.CallFunc(goat_attack))
    s.add(monkey.actions.WalkDynamic(id, gigio), loop=True)
    monkey.play(s)

    # msg(id=119)


def _drown(player, x, y, line):
    script = monkey.Script()
    script.add(monkey.actions.SierraEnable(id=player.id, value=False))
    script.add(monkey.actions.Move(id=player.id, position=(x, y, 1 - y / 166), speed=0))
    script.add(monkey.actions.Animate(id=player.id, anim='drown'))
    message(script, line)
    script.add(monkey.actions.Delay(time=2))
    message(script, 0)
    monkey.play(script)


def bow():
    if settings.room != 'throne':
        msg(id=112)
    else:
        player = monkey.get_node(game_state.Ids.player)
        # if player.x < 120 or player.x > 130 or player.y < 60 or player.y > 80:
        #    msg(id=1)
        # else:
        # print('FOOFOFOFOF')
        addScore(3)
        s = monkey.Script()
        # player.sendMessage(id='pause')
        s.add(monkey.actions.SierraEnable(id=player.id, value=False))
        s.add(monkey.actions.Turn(id=player.id, dir='w'))
        s.add(monkey.actions.Animate(id=player.id, anim='bow', sync=True))
        s.add(monkey.actions.Turn(id=player.id, dir='w'))
        message(s, id=137)
        s.add(monkey.actions.SierraEnable(id=player.id, value=True))
        monkey.play(s)

def start_castle():
    if settings.previous_room == 'incastl1':
        s = monkey.Script()
        s.add(monkey.actions.Animate(game_state.nodes['door_castle'], 'open', False, True ))
        monkey.play(s)