if [ $# -eq 0 ]
  then
    echo "No arguments supplied"
    exit 1
fi

mkdir $1
cat > $1/main.py << EOF
#!/usr/bin/python3

import monkey

game = monkey.engine()
game.start()
game.run()
game.shutdown()
EOF
 
cat > $1/factory.py << EOF
def create_room(room):
    pass
EOF

cat > $1/settings.py << EOF
import monkey

device_size = (256, 240)
room = 'hello_world'
title = 'Hello world!'
shaders = [monkey.SHADER_BATCH_QUAD_PALETTE]
EOF

chmod 755 $1/main.py
