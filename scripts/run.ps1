$dosbox_path = "C:/DOSBox-X/dosbox-x.exe"

# TODO: define DOSBOX and rebuild here
cmake --build build --target clean
cmake --build build --config Release --target all

cp .\build\StriderWar.exe .\dev\SWar.exe

& $dosbox_path ('dev\SWar.exe', '-conf', 'scripts\dosbox-x.conf', '-exit') 
