# GLEW
if [[ ! -e glew ]]; then
    sudo pacman -S build-essential libxmu-dev libxi-dev libgl-dev
    cd glew/auto
    make
    cd ..
    make
    sudo make install
    make clean
    cd ..
fi
# GLFW
if [[ ! -e glfw ]]; then
    sudo pacman -S xorg-dev
    cd glfw
    cmake -S . -B build
    cd ..
fi
