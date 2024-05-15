import monkey
from . import settings


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