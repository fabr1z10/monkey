import monkey
from . import settings

def bump_invisible_brick(node):
    collider = node.getLabelledComponent('collider')
    if collider.flag == 16:
        collider.flag = 2
        s = monkey.Script()
        s.add(monkey.actions.Animate(node.id, 'taken'))
        s.add(monkey.actions.MoveAccelerated(node.id, (0, 200, 0), (0, -1000, 0), y_min=node.user_data['y']))
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

def mushroom():
    node = monkey.get_sprite('tiles/mushroom')
    node.add_component(monkey.components.Controller2D(size=[10, 10, 0], batch='lines'))
    w = monkey.components.FoeWalk2D(state=monkey.NodeState.INACTIVE, tag='walker', max_speed=10, flip_h=False,
        acceleration=0.1, jump_height=settings.jump_height, time_to_jump_apex=settings.time_to_jump_apex)
    node.add_component(w)
    def on_touch_mushroom(mushroom, player):
        mushroom.remove()
        if settings.state == 0:
            settings.state = 1
            update_mario(player)
    collider = monkey.components.SpriteCollider(settings.Flags.FOE, settings.Flags.PLAYER,
        settings.Tags.FOE, batch='lines')
    collider.setResponse(settings.Tags.PLAYER, on_enter=on_touch_mushroom)
    node.add_component(collider)
    def f():
        w.setState(monkey.NodeState.ACTIVE)
    node.addBehavior('walk', f)
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