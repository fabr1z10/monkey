import monkey


def pippo(a, b, c):
    print('entering weppo')


def pippa(a, b):
    print('leaving jiko')

def _drown(player, x, y):
    script = monkey.Script()
    script.add(monkey.actions.SierraEnable(id=player.id, value=False))
    script.add(monkey.actions.Move(id=player.id, position=(x, y, player.z), speed=0))
    script.add(monkey.actions.Animate(id=player.id, anim='drown'))
    monkey.play(script)


def drown(player, other, c):
    _drown(player, player.x, 4)