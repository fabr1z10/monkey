import monkey

def create_room(room):
    root = room.root()
    # print(' -- size:', size)
    # game_area = (316, 166)
    viewport = (2, 25, 316, 166)
    # mid_y = game_area[1] // 2
    cam = monkey.CamOrtho(256, 240,
                          viewport=(0,0,256,240),
                          bounds_x=(128, 128), bounds_y=(120, 120))
    room.add_camera(cam)
    room.add_batch('sprites', monkey.SpriteBatch(max_elements=10000, cam=0, sheet='petscii'))

    a = monkey.Text('sprites', 'c64', 'Hello world', anchor=monkey.ANCHOR_CENTER)
    a.set_position(128, 120, 0)
    root.add(a)