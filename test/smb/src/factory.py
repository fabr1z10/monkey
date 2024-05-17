import monkey

from . import data
from . import settings
from . import scripts

def init():
  data.worlds = monkey.read_data_file('worlds.yaml')
  data.strings = monkey.read_data_file('strings.yaml')

def text(text, x, y):
  t = monkey.Text('ui', 'mario', text)
  t.set_position(x, y, 0)
  return t

def player(x, y, speed, acceleration, jh, tja):
  ms = settings.mario_states[settings.state]
  node = monkey.get_sprite(ms['model'])
  node.set_position(x * settings.tile_size, y * settings.tile_size, 0)
  node.add_component(monkey.components.Controller2D(size=ms['size'], batch='lines', label='controller2d', mask_up=2|16))
  node.add_component(monkey.components.SpriteCollider(settings.Flags.PLAYER, settings.Flags.FOE,
    settings.Tags.PLAYER, batch='lines'))
  node.add_component(monkey.components.PlayerWalk2D(max_speed=speed,
    acceleration=acceleration, jump_height=jh, time_to_jump_apex=tja))
  node.add_component(monkey.components.Follow(0))
  return node

def base(item, x, y):
  z = item.get('z', 0)
  node = monkey.Node()
  node.set_position(x * settings.tile_size, y * settings.tile_size, z)
  return node
# solid = item.get('solid', None)
# if 'quad' in item:

#   for i in range(0, len(loc), 4):



def platform(item, parent):
  loc = item['loc']
  for i in range(0, len(loc), 4):
    node = base(item, loc[i], loc[i+1])
    repx = loc[i+2]
    repy = loc[i+3]
    quad = item.get('quad', None)
    solid = item.get('solid', True)
    if quad:
      model = monkey.models.Quad('tiles')
      model.add(item['quad'], size=(quad[2] * repx, quad[3] * repy), repeat=(repx, repy))
      node.set_model(model)
    if solid:
      node.add_component(monkey.components.Collider(
        shape=monkey.shapes.AABB(0, repx*16, 0, repy*16), flag=2, mask=0, tag=0, batch='lines'))
    parent.add(node)


def brick_common(item, parent, f, **kwargs):
  flag = kwargs.get('flag', 2)
  loc = item['loc']
  sprite = item['sprite']
  #hits = item.get('hits', 1)
  for i in range(0, len(loc), 2):
    b = monkey.get_sprite(sprite)
    z = item.get('z', 0)
    x = loc[i] * settings.tile_size
    y = loc[i+1] * settings.tile_size
    b.set_position(x, y, z)
    b.add_component(monkey.components.Collider(
      shape=monkey.shapes.AABB(0, settings.tile_size, 0, settings.tile_size), flag=flag, mask=0, tag=0, batch='lines', label='collider'))
    b.user_data['y'] = y
    f(item, b)
    parent.add(b)

def invisible_brick(item, parent):
  def f(item, b):
    b.add_component(monkey.components.Platform(on_bump=scripts.bump_invisible_brick))
  brick_common(item, parent, f, flag=16)



def brick(item, parent):
  def f(item, b):
    b.add_component(monkey.components.Platform(on_bump=scripts.bump_brick))
  brick_common(item, parent, f)

def brick_bonus(item, parent):
  def f(item, b):
    bonus = item.get('bonus')
    b.add_component(monkey.components.Platform(on_bump=scripts.bump_platform_bonus))
    b.user_data['bonus'] = bonus
    b.user_data['hit'] = 1
  brick_common(item, parent, f)


def brick_coin(item, parent):
  def f(item, b):
    hits = item.get('hits', 1)
    b.add_component(monkey.components.Platform(on_bump=scripts.bump_platform))
    b.user_data['hit'] = hits
  brick_common(item, parent, f)

def tiled(item, parent):
   model = monkey.get_tiled(item['tiled'], **item['args'])
   solid = item.get('solid', None)
   z = item.get('z',0)
   loc = item['loc']
   for i in range(0, len(loc), 2):
     node = monkey.Node()
     node.set_position(loc[i] * 16, loc[i + 1] * 16, z)
     node.set_model(model)
     if solid:
       node.add_component(monkey.components.Collider(
         shape=monkey.shapes.AABB(0, solid[0]*16, 0, solid[1]*16), flag=2, mask=0, tag=0, batch='lines'))
     parent.add(node)


def create_room(room):
  world = data.worlds[settings.room]
  bg_color = world.get('bg_color', [0, 0, 0])
  world_size = world['size']
  name = world['name']
  room.set_clear_color(*bg_color)
  device_width = settings.device_size[0]
  device_height = settings.device_size[1]
  hdw = device_width // 2
  hdh = device_height // 2
  cam = monkey.CamOrtho(device_width, device_height,
    viewport=(0, 0, device_width, device_height),
    bounds_x=(hdw, world_size[0] - hdw), bounds_y=(hdh, world_size[1] - hdh))
  cam_ui = monkey.CamOrtho(device_width, device_height,
    viewport=(0, 0, device_width, device_height),
    bounds_x=(hdw, hdw), bounds_y=(hdh, hdh))
  room.add_camera(cam)
  room.add_camera(cam_ui)
  ce = monkey.CollisionEngine2D(80, 80)
  #ce.add_response(settings.Tags.PLAYER, settings.Tags.FOE, scripts.playerVsFoeCallback)
  room.add_runner(ce)
  room.add_runner(monkey.Scheduler())
  root = room.root()
  kb = monkey.components.Keyboard()
  kb.add(settings.restart_key, 1, 0, lambda: monkey.close_room())
  root.add_component(kb)
  settings.main_node = root.id



  room.add_batch('tiles', monkey.SpriteBatch(max_elements=10000, cam=0, sheet='tiles'))
  room.add_batch('lines', monkey.LineBatch(max_elements=2000, cam=0))
  room.add_batch('ui', monkey.SpriteBatch(max_elements=1000, cam=1, sheet='tiles'))
  root.add(text(data.strings['score'], 24, 232))
  root.add(text(str(data.score).zfill(6), 24, 224))
  root.add(text(data.strings['world'], 144, 232))
  root.add(text(data.strings['time'], 200, 232))
  root.add(text('*' + str(data.coins).zfill(2), 96, 224))
  root.add(text(name, 152, 224))
  items = world.get('items', [])
  root.add(player(settings.start_pos[0], settings.start_pos[1], settings.speed, 0.1,
    settings.jump_height, settings.time_to_jump_apex))
  for item in items:
    type = item['type']
    globals()[type](item, root)
    # z = item.get('z', 0)
    # solid = item.get('solid', None)
    # if 'quad' in item:
    #   loc = item['loc']
    #   for i in range(0, len(loc), 4):
    #     node = monkey.Node()
    #     node.set_position(loc[i] * 16, loc[i+1] * 16, z)
    #     repx = loc[i+2]
    #     repy = loc[i+3]
    #     quad = item.get('quad', [])
    #     if quad:
    #       model = monkey.models.Quad('tiles')
    #       model.add(item['quad'], size=(quad[2] * repx, quad[3] * repy), repeat=(repx, repy))
    #       node.set_model(model)
    #     if solid:
    #       node.add_component(monkey.components.Collider(
    #         shape=monkey.shapes.AABB(0, repx*16, 0, repy*16), flag=2, mask=0, tag=0, batch='lines'))
    #     root.add(node)
