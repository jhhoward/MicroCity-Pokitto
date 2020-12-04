#include "Pokitto.h"
#include "Game.h"
#include "Interface.h"
#include "Font.h"
#include "Generated/Palette.inc.h"
#include <File>

Pokitto::Core pokitto;
const char* saveGameName = "MICROCITY.SAV";
const char saveHeader[4] = { 'C', 'T', 'Y', '1' };

uint8_t GetInput()
{
  uint8_t result = 0;
  
  if(pokitto.buttons.aBtn())
  {
    result |= INPUT_B;  
  }
  if(pokitto.buttons.bBtn())
  {
    result |= INPUT_A;  
  }
  if(pokitto.buttons.cBtn())
  {
    result |= INPUT_START;  
  }
  if(pokitto.buttons.upBtn())
  {
    result |= INPUT_UP;  
  }
  if(pokitto.buttons.downBtn())
  {
    result |= INPUT_DOWN;  
  }
  if(pokitto.buttons.leftBtn())
  {
    result |= INPUT_LEFT;  
  }
  if(pokitto.buttons.rightBtn())
  {
    result |= INPUT_RIGHT;  
  }

  return result;
}

void PutPixel(uint8_t x, uint8_t y, uint8_t colour)
{
    Pokitto::Display::drawPixel(x, y, colour);
}

void SaveCity()
{
    File file;
    if(file.openRW(saveGameName, true, false))
    {
        file.write(saveHeader, 4);
        file.write(&State, sizeof(GameState));
    }
}

bool LoadCity()
{
    File file;
    if(file.openRO(saveGameName))
    {
        char readHeader[4];
        file.read(readHeader, 4);
        for(int n = 0; n < 4; n++)
        {
            if(readHeader[n] != saveHeader[n])
            {
                return false;
            }
        }
        file.read(&State, sizeof(GameState));
        return true;
    }
    return false;
}

uint8_t* GetPowerGrid()
{
    // Just use display buffer as a temporary scratch space for power grid flood fill
    return (uint8_t*) Pokitto::Display::getBuffer();
}

uint8_t* GetScreenBuffer(void)
{
    return (uint8_t*) Pokitto::Display::getBuffer();
}


int main(){
    pokitto.begin();
    Pokitto::Display::persistence = true;
    Pokitto::Display::load565Palette(MicroCityPalette);

    while(GetInput() != 0)
    {
        pokitto.update();
    }
    
    InitGame();

    
    while( pokitto.isRunning() ){
        if( !pokitto.update() ) 
            continue;
        TickGame();
    }
    
    return 0;
}
