import monkey
import settings
import scripts

def get_shorthand(node, s: str):
    n = s.split(',')
    if n[0] == 'p':
        z = float(n[6]) if len(n) > 6 else 0
        pos = [float(n[1]),float(n[2]),z]
        node.add(platform(pos=pos, size=(float(n[3]), float(n[4])), tile=getattr(settings.Tiles, n[5])))
    elif n[0] =='bc':
        for i in range(1, len(n), 2):
            pos = [float(n[i]), float(n[i+1]), 0]
            node.add(brick_coin(pos=pos, hits=1, sprite='tiles/brick_bonus'))
    elif n[0] == 'b':
        for i in range(1, len(n), 2):
            pos = [float(n[i]), float(n[i+1]), 0]
            node.add(brick(pos=pos, hits=1, sprite='tiles/brick'))
    elif n[0] == 'bb':
        for i in range(1, len(n), 2):
            pos = [float(n[i]), float(n[i+1]), 0]
            node.add(brick_bonus(pos=pos, sprite='tiles/brick_bonus'))
    elif n[0] == 'ts':
        id = n[1]
        width = float(n[2])
        height = float(n[3])
        for i in range(4, len(n), 2):
            pos = [float(n[i]), float(n[i+1]), 0]
            node.add(tiled(id=id, pos=pos, size=(width, height)))
    elif n[0] == 'foe':
        id = n[1]
        for i in range(2, len(n), 2):
            pos = [float(n[i]), float(n[i+1]), 0]
            node.add(foe(id=id, pos=pos))










def player(x, y, speed, acceleration, jh, tja):
    node = monkey.get_sprite('tiles/mario')
    node.set_position(x * settings.tile_size, y * settings.tile_size, 0)
    node.add_component(monkey.components.Controller2D(size=[10, 10, 0], batch='lines'))
    node.add_component(monkey.components.SpriteCollider(settings.CollisionFlags.player,
        settings.CollisionFlags.foe, settings.CollisionTags.player, batch='lines'))
    node.add_component(monkey.components.PlayerWalk2D(max_speed=speed,
        acceleration=acceleration, jump_height=jh, time_to_jump_apex=tja))
    node.add_component(monkey.components.Follow(0))
    return node

def foe(**kwargs):
    foe_id = kwargs['id']
    char_info = settings.characters[foe_id]
    node = monkey.get_sprite(char_info['sprite'])
    pos = kwargs.get('pos', [0, 0, 0])
    node.set_position(pos[0] * settings.tile_size, pos[1] * settings.tile_size, pos[2])
    node.add_component(monkey.components.Controller2D(size=[10, 10, 0], batch='lines'))
    node.add_component(monkey.components.FoeWalk2D(max_speed=char_info['speed'],
        acceleration=char_info['acceleration'], jump_height=char_info['jump_height'],
        time_to_jump_apex=char_info['time_to_jump_apex'], dir=-1))
    node.add_component(monkey.components.SpriteCollider(settings.CollisionFlags.foe,
        settings.CollisionFlags.player, settings.CollisionTags.foe, batch='lines'))
    return node

    # node.add_component(monkey.mario_controller(size=(width, height, 0), batch='line'))
    #     node.add_component(monkey.dynamics())
    #     node.set_model(monkey.get_sprite(model), batch='main')
    #     return node
    #
    # def player2d(x, y, width, height, model, **kwargs):
    #     node = _char2d(x, y, width, height, model)
    #     node.tag = 'player'
    #     gravity = kwargs.get('gravity', globals.data.gravity)
    #     sm = monkey.state_machine()
    #     speed = kwargs.get('speed', 10)
    #     jump_height = kwargs.get('jump_height', 32)
    #     time_to_jump_apex = kwargs.get('time_to_jump_apex', 0.2)
    #     sm.add('walk', monkey.walk_2d_player(speed=speed, gravity=gravity, jump_height=jump_height,
    #                                          time_to_jump_apex=time_to_jump_apex))
    #     node.add_component(sm)
    #     node.add_component(monkey.sprite_collider(flag=0, mask=1, tag=0, batch='line'))
    #     return node


def platform(**kwargs):
    platform = monkey.Node()
    pos = kwargs.get('pos', [0, 0, 0])
    platform.set_position(pos[0] * settings.tile_size, pos[1] * settings.tile_size, pos[2])
    size = kwargs['size']
    width_px = size[0] * settings.tile_size
    height_px = size[1] * settings.tile_size
    platform.add_component(monkey.components.Collider(
        shape=monkey.shapes.AABB(0, width_px, 0, height_px), flag=2, mask=0, tag=0, batch='lines'))
    a = monkey.models.Quad('tiles')
    a.add(kwargs['tile'], repeat=size, size=(width_px, height_px))
    platform.set_model(a)
    return platform

def spawn(**kwargs):
    sp = monkey.Node()
    pos = kwargs.get('pos', [0, 0, 0])
    sp.set_position(pos[0] * settings.tile_size, pos[1] * settings.tile_size, pos[2])
    sp.add_component(monkey.components.Collider(shape=monkey.shapes.AABB(0,2,0,256), flag=settings.CollisionFlags.foe,
        mask=settings.CollisionFlags.player, tag=settings.CollisionTags.spawn, batch='lines'))
    sp.user_data['item'] = kwargs['item']
    return sp



def init():
    settings.characters = monkey.read_data_file('characters.yaml')
    settings.rooms = monkey.read_data_file('rooms.yaml')



def mushroom():
    b = monkey.get_sprite('tiles/mushroom')
    switch = monkey.components.Switch()
    b.add_component(switch)
    b.add_component(monkey.components.Controller2D(size=[10, 10, 0], batch='lines'))
    switch.add(monkey.components.FoeWalk2D(max_speed=50,
        acceleration=0.1, jump_height=64, time_to_jump_apex=0.5, dir=-1, flip_h=False, flip_platform_edge=False))
    b.add_component(switch)
    return b

def brick_coin(**kwargs):
    b = monkey.get_sprite(kwargs['sprite'])
    hit = kwargs.get('hit',1)
    pos = kwargs.get('pos', [0,0,0])
    b.set_position(pos[0] * settings.tile_size, pos[1] * settings.tile_size, pos[2])
    b.add_component(monkey.components.Collider(
        shape=monkey.shapes.AABB(0, settings.tile_size, 0, settings.tile_size), flag=2, mask=0, tag=0, batch='lines'))
    b.add_component(monkey.components.Platform(on_bump=scripts.bump_platform))
    b.user_data = {'y': pos[1] * settings.tile_size, 'hit': hit}
    return b

def brick_bonus(**kwargs):
    print(kwargs['sprite'])
    b = monkey.get_sprite(kwargs['sprite'])
    print('suca')
    pos = kwargs.get('pos', [0, 0, 0])
    b.set_position(pos[0] * settings.tile_size, pos[1] * settings.tile_size, pos[2])
    b.add_component(monkey.components.Collider(
        shape=monkey.shapes.AABB(0, settings.tile_size, 0, settings.tile_size), flag=2, mask=0, tag=0, batch='lines'))
    b.add_component(monkey.components.Platform(on_bump=scripts.bump_platform_bonus))
    b.user_data = {'y': pos[1] * settings.tile_size, 'hit': 1,'bonus': 'mushroom'}
    return b

def tiled(**kwargs):
    id = kwargs['id']
    pos = kwargs.get('pos', [0, 0, 0])
    size = kwargs.get('size', [0,0])
    b = id.find('(')
    if b != -1:
        print(id)
        args = id[b+1:id.find(')')].split(',')
    ar= {}
    i = 0
    for arg in args:
        ar['x' + str(i)] = int(arg)
        i+=1
    a = monkey.Node()
    a.set_position(pos[0] * settings.tile_size, pos[1] * settings.tile_size, pos[2])
    print(ar)
    if size[0] > 0:
        a.add_component(monkey.components.Collider(
            shape=monkey.shapes.AABB(0, size[0] * settings.tile_size, 0, size[1]*settings.tile_size), flag=2, mask=0, tag=0, batch='lines'))
    a.set_model(monkey.get_tiled(id[:b], **ar))
    return a



def brick(**kwargs):
    b = monkey.Node()
    model = monkey.models.Quad('tiles')
    model.add([368,0,8,8], repeat=[2,2], size=(16,16))
    b.set_model(model)
    pos = kwargs.get('pos', [0,0,0])
    b.set_position(pos[0] * settings.tile_size, pos[1] * settings.tile_size, pos[2])
    b.add_component(monkey.components.Collider(
        shape=monkey.shapes.AABB(0, settings.tile_size, 0, settings.tile_size), flag=2, mask=0, tag=0, batch='lines'))
    b.add_component(monkey.components.Platform(on_bump=scripts.bump_brick))
    b.user_data = {'y': pos[1] * settings.tile_size}
    return b

def create_room(room):
    room_info = settings.rooms[settings.room]
    #room.set_clear_color(92,148,252)
    room.set_clear_color(0, 0, 64)
    world_size = room_info['world_size']
    cam = monkey.CamOrtho(256, 240,
                          viewport=(0,0,256,240),
                          bounds_x=(128, world_size[0]-128), bounds_y=(120, world_size[1]-120))
    room.add_camera(cam)
    ce = monkey.CollisionEngine2D(80, 80)
    ce.add_response(settings.CollisionTags.player, settings.CollisionTags.spawn, on_start=scripts.touch_spawn)
    room.add_runner(ce)
    room.add_runner(monkey.Scheduler())
    root = room.root()

    settings.main_node = root.id
    kb = monkey.components.Keyboard()
    kb.add(settings.Keys.restart, 1, 0, scripts.restart_room)
    root.add_component(kb)

    room.add_batch('tiles', monkey.SpriteBatch(max_elements=10000, cam=0, sheet='tiles'))
    room.add_batch('lines', monkey.LineBatch(max_elements=2000, cam=0))

    root.add(player(5, 5,  200, 0.1 ,settings.jump_height, 0.5))
    #root.add(foe(8, 3))
    #a = monkey.models.Quad('tiles', '38,1,fh,38,1')
    #root.add(tiled(5,5,'tiles/pipe',n=3))
    for item in room_info.get('items', []):
        if isinstance(item, str):
            get_shorthand(root, item)
        else:
            f = globals().get(item['type'])
            if f:
                root.add(f(**item))
