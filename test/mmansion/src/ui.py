import monkey

def on_enter_verb(node):
    node.get_model().set_palette(2)


def on_leave_verb(node):
    node.get_model().set_palette(1)
