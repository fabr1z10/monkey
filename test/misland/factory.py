import monkey
import settings


def init():
    settings.rooms = monkey.read_data_file('rooms.yaml')

def create_room(room):
    room_info = settings.rooms[settings.room]

    size = room_info['size']
    print(' -- size:', size)
    game_area = (320, 200)
    viewport = (0, 0, 320, 200)
    mid_y = game_area[1] // 2
    cam = monkey.CamOrtho(game_area[0], game_area[1],
                              viewport=viewport,
                              bounds_x=(160, 160), bounds_y=(100, 100))
    room.add_camera(cam)
    room.add_batch('sprites', monkey.SpriteBatch(max_elements=10000, cam=0, sheet='sprites'))


    test = monkey.Text(batch='sprites', font='sierra',
                       text="Misfortune strikes and you have fallen into the moat. Your struggles and cries have attracted hungry alligators. They do not want to let you go.",
                       width = 29*8)
    test.set_position(160, 100, 0)
    root = room.root()
    root.add(test)
    #test = monkey.Text('sprites/sierra',
    #                   "We, at Sierra, wish to thank you for playing King's Quest. We are very sorry that you did not succeed and hope you will fare better next time.\nGood luck.",
    #                   width = 29*8)