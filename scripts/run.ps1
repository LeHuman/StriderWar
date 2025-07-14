$dosbox_path = "C:/DOSBox-X/dosbox-x.exe"

# TODO: define DOSBOX and rebuild here
cmake --build build --config Debug --target all

cp .\build\DOSTest.exe .\dev\DOSTest.exe

& $dosbox_path ('dev\DOSTest.exe', '-conf', 'scripts\dosbox-x.conf', '-exit') 
