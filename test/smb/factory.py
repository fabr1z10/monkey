import monkey
import settings
import scripts

def player(x, y, speed, acceleration, jh, tja):
    node = monkey.get_sprite('tiles/mario')
    node.set_position(x * settings.tile_size, y * settings.tile_size, 0)
    node.add_component(monkey.components.Controller2D(size=[10, 10, 0], batch='lines'))
    node.add_component(monkey.components.PlayerWalk2D(max_speed=speed,
        acceleration=acceleration, jump_height=jh, time_to_jump_apex=tja))
    node.add_component(monkey.components.Follow(0))
    return node

def foe(x, y):
    node = monkey.get_sprite('tiles/goomba')
    node.set_position(x * settings.tile_size, y * settings.tile_size, 0)

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


def platform(ciao):
    platform = monkey.Node()
    pos = ciao.get('pos', [0, 0, 0])
    platform.set_position(pos[0] * settings.tile_size, pos[1] * settings.tile_size, pos[2])
    size = ciao['size']
    width_px = size[0] * settings.tile_size
    height_px = size[1] * settings.tile_size
    platform.add_component(monkey.components.Collider(
        shape=monkey.shapes.AABB(0, width_px, 0, height_px), flag=2, mask=0, tag=0, batch='lines'))
    a = monkey.models.Quad('tiles')
    a.add(ciao['tile'], repeat=size, size=(width_px, height_px))
    platform.set_model(a)
    return platform


def init():
    settings.rooms = monkey.read_data_file('rooms.yaml')



def sprite(ciao):
    b = monkey.get_sprite(ciao['sprite'])
    pos = ciao.get('pos', [0,0,0])
    b.set_position(pos[0] * settings.tile_size, pos[1] * settings.tile_size, pos[2])
    b.add_component(monkey.components.Collider(
        shape=monkey.shapes.AABB(0, settings.tile_size, 0, settings.tile_size), flag=2, mask=0, tag=0, batch='lines'))
    b.add_component(monkey.components.Platform(on_bump=scripts.bump_platform))
    b.user_data = {'y': pos[1] * settings.tile_size, 'hit': 4}
    return b

def create_room(room):
    room_info = settings.rooms[settings.room]
    world_size = room_info['world_size']
    cam = monkey.CamOrtho(256, 240,
                          viewport=(0,0,256,240),
                          bounds_x=(128, world_size[0]-128), bounds_y=(120, world_size[1]-120))
    room.add_camera(cam)
    ce = monkey.CollisionEngine2D(80, 80)
    #ce.add_response(0, 1, on_start=on_enter_hotspot, on_end=on_leave_hotspot)
    room.add_runner(ce)
    room.add_runner(monkey.Scheduler())
    root = room.root()

    settings.main_node = root.id
    kb = monkey.components.Keyboard()
    kb.add(settings.Keys.restart, 1, 0, scripts.restart_room)
    root.add_component(kb)

    room.add_batch('tiles', monkey.SpriteBatch(max_elements=10000, cam=0, sheet='tiles'))
    room.add_batch('lines', monkey.LineBatch(max_elements=200, cam=0))

    root.add(player(5, 5,  200, 0.1 ,settings.jump_height, 0.5))
    root.add(foe(8, 2))
    for item in room_info.get('items', []):
        f = globals().get(item['type'])
        if f:
            root.add(f(item))
