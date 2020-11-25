#include "Pokitto.h"

#include "Smile.h"
#include "Game.h"
#include "Interface.h"
#include "Generated/Palette.inc.h"

Pokitto::Core pokitto;

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
    fileOpen("city.sav", FILE_MODE_READWRITE);
    
    if(fileOK() == 0)
    {
        fileSetPosition(0);
        
        fileWriteBytes((uint8_t*)&State, sizeof(GameState));
    
        fileClose();
    }
}

bool LoadCity()
{
    fileOpen("city.sav", FILE_MODE_READWRITE);

    if(fileOK() != 0)
    {
        return false;
    }
    
    fileSetPosition(0);
    uint16_t bytesRead = fileReadBytes((uint8_t*)&State, sizeof(GameState));
    fileClose();
    
    return bytesRead == sizeof(GameState);
}

static uint8_t buffer[100];

uint8_t* GetPowerGrid()
{
    return buffer;
}


int main(){
    pokitto.begin();
    Pokitto::Display::persistence = true;
    Pokitto::Display::load565Palette(MicroCityPalette);
    //PD::invisiblecolor = 0;
    
    InitGame();

    
    while( pokitto.isRunning() ){
        if( !pokitto.update() ) 
            continue;
        TickGame();
    }
    
    return 0;
}
