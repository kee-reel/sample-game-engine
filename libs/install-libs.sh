# GLEW
if [[ ! -e glew ]]; then
    git clone https://github.com/nigels-com/glew
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
    git clone https://github.com/glfw/glfw
    sudo pacman -S xorg-dev
    cd glfw
    cmake -S . -B build
    cd ..
fi
# GLM
if [[ ! -e glm ]]; then
    git clone https://github.com/g-truc/glm
fi
# STB
if [[ ! -e stb ]]; then
    git clone https://github.com/nothings/stb
fi
