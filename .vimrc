set path+=src/**
command! Ninja :wa|!ninja -C build/current
command! Tags !ctags -R *
