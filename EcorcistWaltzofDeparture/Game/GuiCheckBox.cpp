#include "GuiCheckBox.h"
#include "../App.h"
#include "../Scene.h"
#include "../GuiManager.h"
#include "../Audio.h"
#include "../Window.h"

GuiCheckBox::GuiCheckBox(uint32 id, SDL_Rect bounds, const char* tex) : GuiControl(GuiControlType::CHECKBOX, id)
{
    this->bounds = bounds;
    this->text = tex;
}

GuiCheckBox::~GuiCheckBox()
{
}

bool GuiCheckBox::Update(float dt)
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

            if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_REPEAT)
            {
                state = GuiControlState::PRESSED;
            }

            // If mouse button pressed -> Generate event!
            if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_UP)
            {
                state = GuiControlState::SELECTED;
            }
        }
        else state = GuiControlState::NORMAL;
    }
    switch (state)
    {
    case GuiControlState::DISABLED:
        break;

    case GuiControlState::NORMAL:

       if (checked)
           app->render->DrawRectangle(bounds, 0, 255, 0, 128, true, false);

       if (!checked)
           app->render->DrawRectangle(bounds, 0, 255, 0, 128, false, false);
      /*  if (!checked) render->DrawTexture(texture, bounds.x, bounds.y, &SDL_Rect({ 0,0,20,20 }), 0, 0, 0, 0, false);
        else          render->DrawTexture(texture, bounds.x, bounds.y, &SDL_Rect({ 60,0,20,20 }), 0, 0, 0, 0, false);*/
        break;

    case GuiControlState::FOCUSED:

        app->render->DrawRectangle(bounds, 255, 0, 0, 128, true, false);


       /* if (!checked) render->DrawTexture(texture, bounds.x, bounds.y, &SDL_Rect({ 20,0,20,20 }), 0, 0, 0, 0, false);
        else          render->DrawTexture(texture, bounds.x, bounds.y, &SDL_Rect({ 80,0,20,20 }), 0, 0, 0, 0, false);*/
        break;

    case GuiControlState::PRESSED:

        app->render->DrawRectangle(bounds, 255, 0, 0, 128, true, false);
     /*   if (!checked) render->DrawTexture(texture, bounds.x, bounds.y, &SDL_Rect({ 40,0,20,20 }), 0, 0, 0, 0, false);
        else          render->DrawTexture(texture, bounds.x, bounds.y, &SDL_Rect({ 100,0,20,20 }), 0, 0, 0, 0, false);*/
        break;

    case GuiControlState::SELECTED:

        app->render->DrawRectangle(bounds, 255, 0, 0, 128, true, false);


        if (checked)
            checked = false;
        else if(!checked)
            checked = true;

        //if (id == 1)
        //{
        //    checked = !checked;
        //}
        //else if (id == 2)
        //{

        //    checked = !checked;
        //}
        NotifyObserver();
        break;

    default:
        break;
    }
    if (state != GuiControlState::DISABLED)
    app->render->DrawText(text.GetString(), bounds.x + 43, bounds.y, bounds.w + 60, bounds.h);
    return false;
}

bool GuiCheckBox::Draw(Render* render)
{
  
    // Draw the right button depending on state
   
       
    
    return false;
}

bool GuiCheckBox::DrawDebug(Render* render)
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