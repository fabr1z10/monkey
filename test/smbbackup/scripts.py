import settings
import monkey


def make_score(score, x, y):
    def f():
        a = monkey.get_sprite('tiles/score_'+score)
        a.set_position(x, y, 0)
        script = monkey.Script()
        script.add(monkey.actions.MoveBy(a.id, (0, 32), 1, 0))
        script.add(monkey.actions.Remove(a.id))
        monkey.get_node(settings.main_node).add(a)
        monkey.play(script)
    return f






def restart_room():
    monkey.close_room()


def bump_brick(node):
    s = monkey.Script()
    s.add(monkey.actions.MoveAccelerated(node.id, (0, 200, 0), (0, -1000, 0), y_min=node.user_data['y']))
    monkey.play(s)

def add_foe_walk(b):
    def f():
        b.get_switch().enable(0)
        #b.add_component(monkey.components.Controller2D(size=[10, 10, 0], batch='lines'))
        #b.add_component(monkey.components.FoeWalk2D(max_speed=50,
        #    acceleration=0.1, jump_height=64, time_to_jump_apex=0.5, dir=-1))
    return f

def bump_platform_bonus(node):
    if node.user_data['hit'] == 0:
        return
    node.user_data['hit'] -= 1
    bonus = node.user_data['bonus']
    import factory
    b = getattr(factory, bonus)()
    s = monkey.Script()
    b.set_position(node.x + settings.tile_size* 0.5, node.user_data['y'], node.z)
    s.add(monkey.actions.Animate(node.id, 'taken'))
    s.add(monkey.actions.MoveAccelerated(node.id, (0, 200, 0), (0, -1000, 0), y_min=node.user_data['y']))
    s.add(monkey.actions.Add(settings.main_node, b))
    s.add(monkey.actions.MoveBy(b.id, (0, 17), time=0.5))
    s.add(monkey.actions.CallFunc(add_foe_walk(b)))
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
    s.add(monkey.actions.CallFunc(make_score('100', node.x, node.user_data['y'] + settings.tile_size)))
    monkey.play(s)

def touch_spawn(p,s,x):
    s.remove()
    item = s.user_data['item']
    import factory
    factory.get_shorthand(monkey.get_node(settings.main_node), item)