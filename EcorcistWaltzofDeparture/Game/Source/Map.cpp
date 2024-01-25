
#include "App.h"
#include "Render.h"
#include "Textures.h"
#include "Map.h"
#include "Physics.h"
#include "../Morgan.h"
#include "Defs.h"
#include "Log.h"
#include "Window.h"
#include <math.h>
#include "SDL_image/include/SDL_image.h"
#include "Optick/include/optick.h"
#include "Scene.h"


Map::Map() : Module(), mapLoaded(false)
{
    name.Create("map");
}

// Destructor
Map::~Map()
{}



// Called before render is available
bool Map::Awake(pugi::xml_node& config)
{
    LOG("Loading Map Parser");
    bool ret = true;

  /*  mapFileName = config.child("mapfile").attribute("path").as_string();
    mapFolder = config.child("mapfolder").attribute("path").as_string();*/

    return ret;
}

bool Map::Start() {
    if (active) {
        //Calls the functon to load the map, make sure that the filename is assigned
        SString mapPath = path;
        mapPath += name;
        Load(mapPath);

        //Initialize pathfinding 
        pathfinding = new PathFinding();

        //Initialize the navigation map
        uchar* navigationMap = NULL;
        CreateNavigationMap(mapData.width, mapData.height, &navigationMap);
        pathfinding->SetNavigationMap((uint)mapData.width, (uint)mapData.height, navigationMap);
        RELEASE_ARRAY(navigationMap);
    }
    return true;
}

bool Map::Update(float dt)
{
    if (mapLoaded == false)
        return false;

    ListItem<MapLayer*>* mapLayerItem;
    mapLayerItem = mapData.layers.start;

    // Optick frame for the entire Update function
    OPTICK_EVENT("Map::Update");

    while (mapLayerItem != NULL) {
        if (mapLayerItem->data->properties.GetProperty("Draw") != NULL &&
            mapLayerItem->data->properties.GetProperty("Front") == NULL &&
            mapLayerItem->data->properties.GetProperty("Draw")->value) {
            
            // check if layer has parallax property
            bool parallax = false;
            if (mapLayerItem->data->properties.GetProperty("Parallax") != NULL) {
                parallax = true;
            }

            SDL_Rect const camera = app->render->camera;

            iPoint cameraPos = iPoint(0, 0);
            iPoint cameraSize = iPoint(mapLayerItem->data->width, mapLayerItem->data->height);

            // Map drawing optimization to only draw portion visible by the camera
            if (app->scene->currentLvl == 0) {
                cameraPos = WorldToMap(-camera.x / app->win->GetScale(), ((camera.y) * -1) / app->win->GetScale());
                cameraSize = iPoint(16, 14);
            }
            else if(!parallax) {
                cameraPos = WorldToMap(-camera.x / app->win->GetScale(), ((camera.y) * -1) / app->win->GetScale());
                cameraSize = iPoint(16, 14);
            }

            for (int x = cameraPos.x; x < cameraPos.x + cameraSize.x; x++) {
                for (int y = cameraPos.y; y < cameraPos.y + cameraSize.y; y++) {
                    

                    int gid = mapLayerItem->data->Get(x, y);
                    TileSet* tileset = GetTilesetFromTileId(gid);

                    SDL_Rect r = tileset->GetTileRect(gid);
                    iPoint pos = MapToWorld(x, y);
                    if (app->scene->noir == false) {
                        if (parallax) {
                            // with parallax things are rendered at slower speed , they are in the back. The drawing of the foreground elements is in the postUpdate
                            app->render->DrawTexture(tileset->texture, pos.x, pos.y, false, &r, 255, 0.84f);
                        }
                        else {
                            app->render->DrawTexture(tileset->texture, pos.x, pos.y, false, &r);
                        }
                    }
                    else {
                        // when the noir variable is true all tiles except those of the "back" layer are drawn in black
                        if (parallax) {
                            app->render->DrawTexture(tileset->texture, pos.x, pos.y, false, &r, 255, 0.84f);
                        }
                        else {
                            if (mapLayerItem->data->name == "back" && gid != 7) {
                                app->render->DrawTexture(tileset->texture, pos.x, pos.y, false, &r,255);
                            }
                            else{ app->render->DrawTexture(tileset->texture, pos.x, pos.y, false, &r,255,1,0,0,0); }
                        }
                    }
                }
            }
        }
        else if (!firstLoad ) {
            if ( mapLayerItem->data->properties.GetProperty("Angelical") != NULL) {

                for (int x = 0; x < mapLayerItem->data->width; x++) {
                    for (int y = 0; y < mapLayerItem->data->height; y++) {
                       


                        int gid = mapLayerItem->data->Get(x, y);

                        if (gid == 1282) {
                            Entity* ye = app->entityManager->CreateEntity(EntityType::ANGEL);
                            ye->Awake();
                            ye->position.x = (x * 40) - 300;
                            ye->position.y = y * 40;
                            ye->Start();
                        }
                    }
                }
                firstLoad = true;
            }
           
        }
        if (!chickens) {
            if (mapLayerItem->data->name == "chickens") {
                for (int x = 0; x < mapLayerItem->data->width; x++) {
                    for (int y = 0; y < mapLayerItem->data->height; y++) {
                        // spawn fried chickens (healing items) in accordance to the chicken layer

                        int gid = mapLayerItem->data->Get(x, y);

                        if (gid != 0) {
                            Entity* ye = app->entityManager->CreateEntity(EntityType::HEALINGORB);
                            ye->parameters = app->scene->scene_parameter.child("healingOrb");
                            ye->Awake();
                            ye->position.x = (x * 40) - 20;
                            ye->position.y = (y * 40) - 20;
                            ye->Start();
                        }
                    }
                }
                chickens = true;
            }
        }

        mapLayerItem = mapLayerItem->next;
    }

    return true;
}

iPoint Map::MapToWorld(int x, int y) const
{
    iPoint ret;

    ret.x = x * mapData.tileWidth;
    ret.y = y * mapData.tileHeight;

    return ret;
}



// Get relative Tile rectangle
SDL_Rect TileSet::GetTileRect(int gid) const
{
    SDL_Rect rect = { 0 };
    int relativeIndex = gid - firstgid;

    rect.w = tileWidth;
    rect.h = tileHeight;
    rect.x = margin + (tileWidth + spacing) * (relativeIndex % columns);
    rect.y = margin + (tileWidth + spacing) * (relativeIndex / columns);

    return rect;
}

TileSet* Map::GetTilesetFromTileId(int gid) const
{
    ListItem<TileSet*>* item = mapData.tilesets.start;
    TileSet* set = NULL;

    while (item)
    {
        set = item->data;
        if (gid < (item->data->firstgid + item->data->tilecount))
        {
            break;
        }
        item = item->next;
    }

    return set;
}

// Called before quitting
bool Map::CleanUp()
{
    LOG("Unloading map");

	ListItem<TileSet*>* item;
	item = mapData.tilesets.start;

    chickens = false;

	while (item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}
	mapData.tilesets.Clear();

    // Remove all layers
    ListItem<MapLayer*>* layerItem;
    layerItem = mapData.layers.start;

    while (layerItem != NULL)
    {
        RELEASE(layerItem->data);
        layerItem = layerItem->next;
    }

    return true;
}

// Load new map
bool Map::Load(SString mapFileName)
{
    bool ret = true;

    // L05: DONE 3: Implement LoadMap to load the map properties
    // retrieve the paremeters of the <map> node and save it into map data

    pugi::xml_document mapFileXML;
    pugi::xml_parse_result result = mapFileXML.load_file(mapFileName.GetString());

    if (result == NULL)
    {
        LOG("Could not load map xml file %s. pugi error: %s", mapFileName, result.description());
        ret = false;
    }
    else {

        //Fill mapData variable
        mapData.width = mapFileXML.child("map").attribute("width").as_int();
        mapData.height = mapFileXML.child("map").attribute("height").as_int();
        mapData.tileWidth = mapFileXML.child("map").attribute("tilewidth").as_int();
        mapData.tileHeight = mapFileXML.child("map").attribute("tileheight").as_int();

        // L09: DONE 2: Define a property to store the MapType and Load it from the map
        SString orientationStr = mapFileXML.child("map").attribute("orientation").as_string();
        if (orientationStr == "orthogonal") {
            mapData.orienttation = MapOrientation::ORTOGRAPHIC;
        }
        else if (orientationStr == "isometric") {
            mapData.orienttation = MapOrientation::ISOMETRIC;
        }
        else {
            LOG("Map orientation not found");
            ret = false;
        }

        if (ret == true) {
            ret = LoadObjectGroups(mapFileXML.child("map"));
        }

        // L05: DONE 4: Implement the LoadTileSet function to load the tileset properties
       // Iterate the Tileset
        for (pugi::xml_node tilesetNode = mapFileXML.child("map").child("tileset"); tilesetNode != NULL; tilesetNode = tilesetNode.next_sibling("tileset")) {

            TileSet* tileset = new TileSet();

            //Load Tileset attributes
            tileset->name = tilesetNode.attribute("name").as_string();
            tileset->firstgid = tilesetNode.attribute("firstgid").as_int();
            tileset->margin = tilesetNode.attribute("margin").as_int();
            tileset->spacing = tilesetNode.attribute("spacing").as_int();
            tileset->tileWidth = tilesetNode.attribute("tilewidth").as_int();
            tileset->tileHeight = tilesetNode.attribute("tileheight").as_int();
            tileset->columns = tilesetNode.attribute("columns").as_int();
            tileset->tilecount = tilesetNode.attribute("tilecount").as_int();

            //Load Tileset image
            SString mapTex = path;
            mapTex += tilesetNode.child("image").attribute("source").as_string();
            tileset->texture = app->tex->Load(mapTex.GetString());

            mapData.tilesets.Add(tileset);

        }



        // L06: DONE 3: Iterate all layers in the TMX and load each of them
        for (pugi::xml_node layerNode = mapFileXML.child("map").child("layer"); layerNode != NULL; layerNode = layerNode.next_sibling("layer")) {


            //Load the attributes and saved in a new MapLayer
            MapLayer* mapLayer = new MapLayer();
            mapLayer->id = layerNode.attribute("id").as_int();
            mapLayer->name = layerNode.attribute("name").as_string();
            mapLayer->width = layerNode.attribute("width").as_int();
            mapLayer->height = layerNode.attribute("height").as_int();

            //L08: DONE 6 Call Load Layer Properties
            LoadProperties(layerNode, mapLayer->properties);

            //Reserve the memory for the data 
            mapLayer->tiles = new uint[mapLayer->width * mapLayer->height];
            memset(mapLayer->tiles, 0, mapLayer->width * mapLayer->height);

            //Iterate over all the tiles and assign the values in the data array
            int i = 0;
            for (pugi::xml_node tileNode = layerNode.child("data").child("tile"); tileNode != NULL; tileNode = tileNode.next_sibling("tile")) {
                mapLayer->tiles[i] = tileNode.attribute("gid").as_uint();
                i++;
            }

            //add the layer to the map
            mapData.layers.Add(mapLayer);



        }


        
        if (ret == true)
        {
            LOG("Successfully parsed map XML file :%s", mapFileName.GetString());
            LOG("width : %d height : %d", mapData.width, mapData.height);
            LOG("tile_width : %d tile_height : %d", mapData.tileWidth, mapData.tileHeight);

            LOG("Tilesets----");

            ListItem<TileSet*>* tileset;
            tileset = mapData.tilesets.start;
            while (tileset != NULL) {
                //iterate the tilesets
                LOG("name : %s firstgid : %d", tileset->data->name.GetString(), tileset->data->firstgid);
                LOG("tile width : %d tile height : %d", tileset->data->tileWidth, tileset->data->tileHeight);
                LOG("spacing : %d margin : %d", tileset->data->spacing, tileset->data->margin);
                tileset = tileset->next;
            }

            LOG("Layers----");

            ListItem<MapLayer*>* mapLayer;
            mapLayer = mapData.layers.start;

          
        }

        // Find the navigation layer
        ListItem<MapLayer*>* mapLayerItem;
        mapLayerItem = mapData.layers.start;
        navigationLayer = mapLayerItem->data;

        //Search the layer in the map that contains information for navigation
        while (mapLayerItem != NULL) {
            if (mapLayerItem->data->properties.GetProperty("Navigation") != NULL && mapLayerItem->data->properties.GetProperty("Navigation")->value) {
                navigationLayer = mapLayerItem->data;
                break;
            }
            mapLayerItem = mapLayerItem->next;
        }

        //Resets the map
        if (mapFileXML) mapFileXML.reset();
    }

    mapLoaded = ret;
    return ret;
}

bool Map::LoadMap(pugi::xml_node mapFile)
{
    bool ret = true;
    pugi::xml_node map = mapFile.child("map");

    if (map == NULL)
    {
        LOG("Error parsing map xml file: Cannot find 'map' tag.");
        ret = false;
    }
    else
    {
        //Load map general properties
        mapData.height = map.attribute("height").as_int();
        mapData.width = map.attribute("width").as_int();
        mapData.tileHeight = map.attribute("tileheight").as_int();
        mapData.tileWidth = map.attribute("tilewidth").as_int();
        mapData.type = MAPTYPE_UNKNOWN;
    }

    return ret;
}

bool Map::LoadObjectGroups(pugi::xml_node mapNode) {
    bool ret = true;

    for (pugi::xml_node objectNode = mapNode.child("objectgroup"); objectNode && ret; objectNode = objectNode.next_sibling("objectgroup"))
    {
        if (objectNode.attribute("id").as_int() == 9) {
            LOG("Kill");
            for (pugi::xml_node objectIt = objectNode.child("object"); objectIt != NULL; objectIt = objectIt.next_sibling("object")) {



                int x = objectIt.attribute("x").as_int();
                int y = objectIt.attribute("y").as_int();
                int width = objectIt.attribute("width").as_int();
                int height = objectIt.attribute("height").as_int();


                x += width / 2;
                y += height / 2;
                // Create death pit colliders
                PhysBody* c1 = app->physics->CreateRectangle(x, y, width, height, STATIC, ColliderType::PLATFORM);
                c1->ctype = ColliderType::INSTAKILL;
                killers.Add(c1);
            }
        }
        else if (objectNode.attribute("id").as_int() != 14 && objectNode.attribute("id").as_int() != 11) {
            for (pugi::xml_node objectIt = objectNode.child("object"); objectIt != NULL; objectIt = objectIt.next_sibling("object")) {



                int x = objectIt.attribute("x").as_int();
                int y = objectIt.attribute("y").as_int();
                int width = objectIt.attribute("width").as_int();
                int height = objectIt.attribute("height").as_int();


                x += width / 2;
                y += height / 2;
                // Create ground colliders
                PhysBody* c1 = app->physics->CreateRectangle(x, y, width, height, STATIC, ColliderType::PLATFORM);
                c1->ctype = ColliderType::PLATFORM;
                killers.Add(c1);
            }
        }
        // The last object layer is dedicated to spawning checkPoint objects
        else {
            for (pugi::xml_node objectIt = objectNode.child("object"); objectIt != NULL; objectIt = objectIt.next_sibling("object")) {



                int x = objectIt.attribute("x").as_int();
                int y = objectIt.attribute("y").as_int()-19;
                int width = objectIt.attribute("width").as_int();
                int height = objectIt.attribute("height").as_int();


                x += width / 2;
                y += height / 2;

                Entity* en = app->entityManager->CreateEntity(EntityType::CHECKPOINT);
                app->entityManager->entities.Add(en);
                en->position.x = x;
                en->position.y = y;
                en->parameters = app->scene->scene_parameter.child("checkPointData");

            }
        }
    }



    return ret;
}

bool Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
    bool ret = false;

    for (pugi::xml_node propertieNode = node.child("properties").child("property"); propertieNode; propertieNode = propertieNode.next_sibling("property"))
    {
        Properties::Property* p = new Properties::Property();
        p->name = propertieNode.attribute("name").as_string();
        p->value = propertieNode.attribute("value").as_bool(); // (!!) I'm assuming that all values are bool !!

        properties.list.Add(p);
    }

    return ret;
}

Properties::Property* Properties::GetProperty(const char* name)
{
    ListItem<Property*>* item = list.start;
    Property* p = NULL;

    while (item)
    {
        if (item->data->name == name) {
            p = item->data;
            break;
        }
        item = item->next;
    }

    return p;
}

iPoint Map::WorldToMap(int x, int y) {

    iPoint ret(0, 0);

    if (mapData.orienttation == MapOrientation::ORTOGRAPHIC) {
        ret.x = x / mapData.tileWidth;
        ret.y = y / mapData.tileHeight;
    }

    if (mapData.orienttation == MapOrientation::ISOMETRIC) {
        float half_width = mapData.tileWidth / 2;
        float half_height = mapData.tileHeight / 2;
        ret.x = int((x / half_width + y / half_height) / 2);
        ret.y = int((y / half_height - (x / half_width)) / 2);
    }

    return ret;
}

int Map::GetTileWidth() {
    return mapData.tileWidth;
}

int Map::GetTileHeight() {
    return mapData.tileHeight;
}

// L13: Create navigationMap map for pathfinding
void Map::CreateNavigationMap(int& width, int& height, uchar** buffer) const
{
    bool ret = false;

    //Sets the size of the map. The navigation map is a unidimensional array 
    uchar* navigationMap = new uchar[navigationLayer->width * navigationLayer->height];
    //reserves the memory for the navigation map
    memset(navigationMap, 1, navigationLayer->width * navigationLayer->height);

    for (int x = 0; x < mapData.width; x++)
    {
        for (int y = 0; y < mapData.height; y++)
        {
            //i is the index of x,y coordinate in a unidimensional array that represents the navigation map
            int i = (y * navigationLayer->width) + x;

            //Gets the gid of the map in the navigation layer
            int gid = navigationLayer->Get(x, y);

            //If the gid is a blockedGid is an area that I cannot navigate, so is set in the navigation map as 0, all the other areas can be navigated
            //!!!! make sure that you assign blockedGid according to your map
            if (gid == blockedGid) navigationMap[i] = 0;
            else navigationMap[i] = 1;
        }
    }

    *buffer = navigationMap;
    width = mapData.width;
    height = mapData.height;

}

bool Map::PostUpdate() {
    ListItem<MapLayer*>* mapLayerItem;
    mapLayerItem = mapData.layers.start;

    // layes with the Front property are drawn on the post update to make sure they are drawn in front of entities

    while (mapLayerItem != NULL) {

        if ( mapLayerItem->data->properties.GetProperty("Front") != NULL && mapLayerItem->data->properties.GetProperty("Draw")->value) {

            bool parallax = false;
            if (mapLayerItem->data->properties.GetProperty("Parallax") != NULL) { parallax = true; }

            for (int x = 0; x < mapLayerItem->data->width; x++)
            {
                for (int y = 0; y < mapLayerItem->data->height; y++)
                {
                    int gid = mapLayerItem->data->Get(x, y);
                    TileSet* tileset = GetTilesetFromTileId(gid);

                    SDL_Rect r = tileset->GetTileRect(gid);
                    iPoint pos = MapToWorld(x, y);
                    
                    if (app->scene->noir == false) {
                        if (parallax) {
                            app->render->DrawTexture(tileset->texture, pos.x, pos.y, false, &r, 255, 1.08f);
                        }
                        else {
                            app->render->DrawTexture(tileset->texture, pos.x, pos.y, false, &r);
                        }
                    }
                    else {
                        if (parallax) {
                            app->render->DrawTexture(tileset->texture, pos.x, pos.y, false, &r, 255, 1.08f);
                        }
                        else {
                            if (mapLayerItem->data->name == "back") {
                                app->render->DrawTexture(tileset->texture, pos.x, pos.y, false, &r,255,1,255,255,255);
                            }
                            else { app->render->DrawTexture(tileset->texture, pos.x, pos.y, false, &r, 255, 1, 0, 0, 0); }
                        }
                    }
                }
            }



        }
        mapLayerItem = mapLayerItem->next;

    }

    return true;
}