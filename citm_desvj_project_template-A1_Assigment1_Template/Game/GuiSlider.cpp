#include "GuiSlider.h"
#include "../App.h"
#include "../Scene.h"
#include "../GuiManager.h"
#include "../Audio.h"
#include "../Window.h"

GuiSlider::GuiSlider(uint32 id, SDL_Rect bounds, const char* tex) : GuiControl(GuiControlType::SLIDER, id)
{
    this->bounds = bounds;
    this->unit = bounds.w / 100.0f;
    this->text = tex;
    if (id == 1)
    {
        //value = app->guimanager->musicVolume;
    }
    else if (id == 2)
    {
        //value = app->guimanager->fxVolume;
    }

    value = round(value);
    sliderPosx = ((value * unit) + bounds.x) - unit;
}

GuiSlider::~GuiSlider()
{
}

bool GuiSlider::Update( float dt)
{
    if (state != GuiControlState::DISABLED)
    {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        // Check collision between mouse and button bounds
        if ((mouseX > bounds.x) && (mouseX < (bounds.x + bounds.w)) &&
            (mouseY > bounds.y) && (mouseY < (bounds.y + bounds.h)))
        {
            state = GuiControlState::FOCUSED;

            unit = bounds.w / 100.0f;
            value = (mouseX - bounds.x) / unit;
            value = round(value);

            value2 = (sliderPosx - bounds.x) / unit;
            value2 = round(value2);


            if (value2 < 2)
                value2 = 0;

            if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_REPEAT)
            {
                state = GuiControlState::PRESSED;
                sliderPosx = ((value * unit) + bounds.x) - unit - 5;
            }

            // If mouse button pressed -> Generate event!
            if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_UP)
            {
                state = GuiControlState::SELECTED;
            }
        }
        else state = GuiControlState::NORMAL;

        switch (state)
        {
        case GuiControlState::DISABLED:
            break;

        case GuiControlState::NORMAL:
            //app->render->DrawTexture(texture, bounds.x, bounds.y, &SDL_Rect({ 0, 0, 116, 23 }), 0, 0, 0, 0, false);
            //app->render->DrawTexture(texture, sliderPosx, bounds.y, &SDL_Rect({ 116, 0, 16, 23 }), 0, 0, 0, 0, false);

            app->render->DrawRectangle(bounds, 0, 100, 50, 255, false, false);
            app->render->DrawRectangle({ sliderPosx, bounds.y, 20,20 }, 200, 200, 200, 255, true, false);

            if (sliderPosx - bounds.x >= 0)
            app->render->DrawRectangle({bounds.x,bounds.y, sliderPosx- bounds.x,20}, 0, 100, 50, 255, true, false);
            break;

        case GuiControlState::FOCUSED:

            app->render->DrawRectangle(bounds, 0, 100, 50, 255, false, false);
            app->render->DrawRectangle({ sliderPosx, bounds.y, 20,20 }, 200, 200, 200, 255, true, false);
            if (sliderPosx - bounds.x >= 0)
            app->render->DrawRectangle({ bounds.x,bounds.y, sliderPosx - bounds.x,20 }, 0, 100, 50, 255, true, false);
          // app->render->DrawTexture(texture, bounds.x, bounds.y, &SDL_Rect({ 0, 23, 116, 23 }), 0, 0, 0, 0, false);
          // app->render->DrawTexture(texture, sliderPosx, bounds.y, &SDL_Rect({ 116, 0, 16, 23 }), 0, 0, 0, 0, false);
            break;

        case GuiControlState::PRESSED:

            app->render->DrawRectangle(bounds, 0, 100, 50, 255, false, false);
            app->render->DrawRectangle({ sliderPosx, bounds.y,  20,20 }, 200, 200, 200, 255, true, false);
            if (sliderPosx - bounds.x >= 0)
            app->render->DrawRectangle({ bounds.x,bounds.y, sliderPosx - bounds.x,20 }, 0, 100, 50, 255, true, false);
           // app->render->DrawTexture(texture, bounds.x, bounds.y, &SDL_Rect({ 0, 23, 116, 23 }), 0, 0, 0, 0, false);
           // app->render->DrawTexture(texture, sliderPosx, bounds.y, &SDL_Rect({ 116, 23 , 16, 23 }), 0, 0, 0, 0, false);
            app->scene->volume = value2;
            break;

        case GuiControlState::SELECTED:
            if (id == 2)
                //app->audio->PlayFx(app->guimanager->pressButtonFx, 0);

                app->render->DrawRectangle(bounds, 0, 100, 50, 255, false, false);
            app->render->DrawRectangle({ sliderPosx, bounds.y, 20,20 }, 200, 200, 200, 255, true, false);
            if (sliderPosx - bounds.x >= 0)
            app->render->DrawRectangle({ bounds.x,bounds.y, sliderPosx - bounds.x,20 }, 0, 100, 50, 255, true, false);
          //  app->render->DrawTexture(texture, bounds.x, bounds.y, &SDL_Rect({ 0, 23, 116, 23 }), 0, 0, 0, 0, false);
          //  app->render->DrawTexture(texture, sliderPosx, bounds.y, &SDL_Rect({ 116, 23 , 16, 23 }), 0, 0, 0, 0, false);
            NotifyObserver();
            break;

        default:
            break;
        }
        app->render->DrawText(text.GetString(), bounds.x, bounds.y - 20, bounds.w, bounds.h);
    }

    app->scene->volume = value2;
    return false;
}

bool GuiSlider::Draw(Render* render)
{
    // Draw the right button depending on state
   
     
    
    return false;
}

bool GuiSlider::DrawDebug(Render* render)
{
    int scale = app->win->GetScale();

    SDL_Rect drawBounds = SDL_Rect({ bounds.x * scale, bounds.y * scale, bounds.w * scale, bounds.h * scale });

    switch (state)
    {
    case GuiControlState::DISABLED:
        render->DrawRectangle(drawBounds, 255, 0, 0, 128, true, false);
        break;
    case GuiControlState::FOCUSED:
        render->DrawRectangle(drawBounds, 0, 255, 0, 128, true, false);
        break;
    case GuiControlState::NORMAL:
        render->DrawRectangle(drawBounds, 0, 0, 255, 128, true, false);
        break;
    case GuiControlState::PRESSED:
        render->DrawRectangle(drawBounds, 255, 255, 0, 128, true, false);
        break;
    case GuiControlState::SELECTED:
        render->DrawRectangle(drawBounds, 0, 255, 255, 128, true, false);
        break;
    }

    return true;
}