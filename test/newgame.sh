if [ $# -eq 0 ]
  then
    echo "No arguments supplied"
    exit 1
fi

mkdir $1
mkdir $1/src
mkdir $1/assets
cat > $1/src/__init__.py << EOF
from .settings import *
from .factory import *
EOF

cat > $1/main.py << EOF
#!/usr/bin/python3

import monkey
import src

game = monkey.engine()
game.start(src)
game.run()
game.shutdown()
EOF
 
cat > $1/factory.py << EOF
def create_room(room):
    pass
EOF

cat > $1/src/settings.py << EOF
import monkey

device_size = (256, 240)
title = 'New game'
shaders = [
    monkey.SHADER_BATCH_QUAD_PALETTE,
    monkey.SHADER_BATCH_LINES
]
EOF

cat > $1/src/factory.py << EOF
import monkey

def create_room(room):
  pass
EOF



chmod 755 $1/main.py
