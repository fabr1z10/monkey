import monkey
from . import settings

def bump_brick(node):
    s = monkey.Script()
    s.add(monkey.actions.MoveAccelerated(node.id, (0, 200, 0), (0, -1000, 0), y_min=node.user_data['y']))
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
    s.add(monkey.actions.CallFunc(lambda: b.getTaggedComponent('walker').setState(monkey.NodeState.ACTIVE)))
    monkey.play(s)

def mushroom():
    node = monkey.get_sprite('tiles/mushroom')
    node.add_component(monkey.components.Controller2D(size=[10, 10, 0], batch='lines'))
    node.add_component(monkey.components.FoeWalk2D(state=monkey.NodeState.INACTIVE, tag='walker', max_speed=10,
        acceleration=0.1, jump_height=settings.jump_height, time_to_jump_apex=settings.time_to_jump_apex))
    return node