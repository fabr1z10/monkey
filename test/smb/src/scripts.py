import monkey
from . import settings

def bump_invisible_brick(node):
    collider = node.getLabelledComponent('collider')
    if collider.flag == 16:
        collider.flag = 2
        s = monkey.Script()
        s.add(monkey.actions.Animate(node.id, 'taken'))
        s.add(monkey.actions.MoveAccelerated(node.id, (0, 200, 0), (0, -1000, 0), y_min=node.user_data['y']))
        bonus = node.user_data['bonus']
        b = globals()[bonus]()
        b.set_position(node.x + settings.tile_size * 0.5, node.user_data['y'], node.z)
        s.add(monkey.actions.Add(settings.main_node, b))
        s.add(monkey.actions.MoveBy(b.id, (0, 17), time=0.5))
        s.add(monkey.actions.CallFunc(lambda: b.setBehavior(
            'walk')))  # lambda: b.getTaggedComponent('walker').setState(monkey.NodeState.ACTIVE)))
        monkey.play(s)

        monkey.play(s)



def bump_brick(node):
    if settings.state == 0:
        s = monkey.Script()
        s.add(monkey.actions.MoveAccelerated(node.id, (0, 200, 0), (0, -1000, 0), y_min=node.user_data['y']))
        monkey.play(s)
    else:
        node.remove()
        v0 = [(-50,150,0),(-20,200,0),(20,200,0),(50,150,0)]
        for i in range(0, 4):
            p = monkey.get_sprite('tiles/brick_piece')
            parent = monkey.get_node(settings.main_node)
            p.set_position(node.x+0.5*settings.tile_size, node.y+0.5*settings.tile_size, 1)
            s = monkey.Script()
            s.add(monkey.actions.MoveAccelerated(p.id, v0[i], (0, -1000, 0), y_min=node.user_data['y']-64))
            s.add(monkey.actions.Remove(p.id))
            parent.add(p)
            monkey.play(s)



def bump_platform(node):
    if node.user_data['hit'] == 0:
        return
    node.user_data['hit'] -= 1
    # bonus
    bonus = monkey.get_sprite('tiles/coin')
    s = monkey.Script()
    bonus.set_position(node.x, node.user_data['y'], node.z)
    i = s.add(monkey.actions.Add(settings.main_node, bonus))
    if node.user_data['hit'] == 0:
        i =s.add(monkey.actions.Animate(node.id, 'taken'))
    s.add(monkey.actions.MoveAccelerated(node.id, (0, 200, 0), (0, -1000, 0), y_min=node.user_data['y']), after=[i])
    s.add(monkey.actions.MoveAccelerated(bonus.id, (0, 300, 0), (0, -1000, 0), y_min = node.user_data['y'] + settings.tile_size), after=[i])
    s.add(monkey.actions.Remove(bonus.id))
    #s.add(monkey.actions.CallFunc(make_score('100', node.x, node.user_data['y'] + settings.tile_size)))
    monkey.play(s)

def bump_platform_bonus(node):
    if node.user_data['hit'] == 0:
        return
    node.user_data['hit'] -= 1
    bonus = node.user_data['bonus']
    b = globals()[bonus]()
    s = monkey.Script()
    b.set_position(node.x + settings.tile_size* 0.5, node.user_data['y'], node.z)
    s.add(monkey.actions.Animate(node.id, 'taken'))
    s.add(monkey.actions.MoveAccelerated(node.id, (0, 200, 0), (0, -1000, 0), y_min=node.user_data['y']))
    s.add(monkey.actions.Add(settings.main_node, b))
    s.add(monkey.actions.MoveBy(b.id, (0, 17), time=0.5))
    s.add(monkey.actions.CallFunc(lambda: b.setBehavior('walk')))#lambda: b.getTaggedComponent('walker').setState(monkey.NodeState.ACTIVE)))
    monkey.play(s)

def update_mario(player):
    ms = settings.mario_states[settings.state]
    player.set_model(monkey.models.getSprite(ms['model']), batch='tiles')
    player.getLabelledComponent('controller2d').set_size(ms['size'], ms['center'])


def generic_bonus(model, callback, pal='default'):
    node = monkey.get_sprite('tiles/mushroom')
    node.setPalette(pal)
    node.add_component(monkey.components.Controller2D(size=[10, 10, 0], batch='lines'))
    w = monkey.components.FoeWalk2D(state=monkey.NodeState.INACTIVE, tag='walker', max_speed=10, flip_h=False,
        acceleration=0.1, jump_height=settings.jump_height, time_to_jump_apex=settings.time_to_jump_apex)
    node.add_component(w)
    collider = monkey.components.SpriteCollider(settings.Flags.FOE, settings.Flags.PLAYER,
        settings.Tags.FOE, batch='lines')
    collider.setResponse(settings.Tags.PLAYER, on_enter=callback)
    node.add_component(collider)
    def f():
        w.setState(monkey.NodeState.ACTIVE)
    node.addBehavior('walk', f)
    return node

def mushroom():
    def on_touch_mushroom(mushroom, player):
        mushroom.remove()
        if settings.state == 0:
            settings.state = 1
            update_mario(player)
    node = generic_bonus('tiles/mushroom', on_touch_mushroom)
    return node

def mushroom_1up():
    def on_touch_mushroom1up(mushroom, player):
        mushroom.remove()
    node = generic_bonus('tiles/mushroom', on_touch_mushroom1up, '6')
    return node

def flower():
    node = monkey.get_sprite('tiles/flower')
    node.add_component(monkey.components.Controller2D(size=[10, 10, 0], batch='lines'))
    def on_touch_flower(flower, player):
        flower.remove()
    collider = monkey.components.SpriteCollider(settings.Flags.FOE, settings.Flags.PLAYER,
        settings.Tags.FOE, batch='lines')
    collider.setResponse(settings.Tags.PLAYER, on_enter=on_touch_flower)
    node.add_component(collider)
    def f():
        pass
    node.addBehavior('walk', f)
    return node

def mushroom_flower():
    return mushroom() if settings.state == 0 else flower()
def playerVsFoeCallback(a,b,c):
    print('QUI')
    exit(1)

def on_enter_warp(g):
    def f(warp, player):
        settings.warp_id = g
        print('entering warp')
    return f


def on_leave_warp(warp, player):
    settings.warp_id = None
    print('leaving warp')

def goto_room(room, x, y):
    def f():
        settings.room = room
        settings.start_pos = (x, y)
        monkey.close_room()
    return f

def enter_warp():
    if settings.warp_id:
        print('--',settings.warp_id)
        monkey.get_node(settings.player_id).setBehavior('warp')
        s = monkey.Script()
        s.add(monkey.actions.MoveBy(settings.player_id, (0, -64), 2, 0))
        s.add(monkey.actions.CallFunc(goto_room(*settings.warp_id)))
        monkey.play(s)