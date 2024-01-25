#ifndef __GUISLIDER_H__
#define __GUISLIDER_H__

#include "../GuiControl.h"

#include "../Point.h"
#include "../SString.h"

class GuiSlider : public GuiControl
{
public:

    GuiSlider(uint32 id, SDL_Rect bounds, const char* tex);
    virtual ~GuiSlider();

    bool Update(float dt);
    bool Draw(Render* render);
    bool DrawDebug(Render* render);

    float value = 90;
    float value2 = 90;
    int sliderPosx;
    float unit;
    bool tittleToScene = true;
private:

    // GuiSlider specific properties
    // Maybe some animation properties for state change?

    int minValue;
    int maxValue;

    bool playFxOnce = true;
};

#endif // __GUISLIDER_H__