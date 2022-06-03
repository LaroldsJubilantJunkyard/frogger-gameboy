
rmdir /s/q  "source\gen"
rmdir /s/q  "headers\gen"

mkdir "source\gen\graphics"
mkdir "headers\gen\graphics"


SET GBDK_HOME=C:/gbdk
SET PNG2ASSET=%GBDK_HOME%/bin/png2asset.exe

:: Tetrominos
%PNG2ASSET% graphics\Frog.png -c source\gen\graphics\Frog.c -px 0 -py 0 -sw 16 -sh 16 -spr8x16 -keep_palette_order -noflip

:: Backgrounds & Maps
%PNG2ASSET% graphics\GetReadyBackground.png -c source\gen\graphics\GetReadyBackground.c -map -use_map_attributes -noflip -keep_palette_order
%PNG2ASSET% graphics\GameOverBackground.png -c source\gen\graphics\GameOverBackground.c -map -use_map_attributes -noflip -keep_palette_order
%PNG2ASSET% graphics\GameplayBackground.png -c source\gen\graphics\GameplayBackground.c -map -use_map_attributes  -keep_palette_order -noflip
%PNG2ASSET% graphics\Obstacles.png -c source\gen\graphics\Obstacles.c -map -use_map_attributes  -keep_palette_order -noflip
%PNG2ASSET% graphics\Font.png -c source\gen\graphics\Font.c -map -use_map_attributes  -keep_palette_order -noflip
%PNG2ASSET% graphics\FrogCompleted.png -c source\gen\graphics\FrogCompleted.c -map -use_map_attributes  -keep_palette_order -noflip


:: move .h files to their proper location
FOR /R "source/gen/graphics/" %%X IN (*.h) DO (
    move "%%X" "%cd%/headers/gen/graphics/"
)