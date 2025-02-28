#pragma once

#include "Module.h"
#include <list>
#include "Physics.h"
#include <vector>
#include <map>

// L09: TODO 5: Add attributes to the property structure
struct Properties
{
	struct Property
	{
		std::string name;
		bool value; //We assume that we are going to work only with bool for the moment
	};

	std::list<Property*> propertyList;

	~Properties()
	{
		for (const auto& property : propertyList)
		{
			delete property;
		}

		propertyList.clear();
	}

	// L09: DONE 7: Method to ask for the value of a custom property
	Property* GetProperty(const char* name)
	{
		for (const auto& property : propertyList) {
			if (property->name == name) {
				return property;
			}
		}

		return nullptr;
	}

};

struct MapLayer
{
	// L07: TODO 1: Add the info to the MapLayer Struct
	int id;
	std::string name;
	int width;
	int height;
	std::vector<int> tiles;
	Properties properties;

	// L07: TODO 6: Short function to get the gid value of i,j
	int Get(int i, int j) const
	{
		return tiles[(j * width) + i];
	}
};

// L06: TODO 2: Create a struct to hold information for a TileSet
// Ignore Terrain Types and Tile Types for now, but we want the image!

struct TileSet
{
	int firstGid;
	std::string name;
	int tileWidth;
	int tileHeight;
	int spacing;
	int margin;
	int tileCount;
	int columns;
	SDL_Texture* texture;

	// L07: TODO 7: Implement the method that receives the gid and returns a Rect
	SDL_Rect GetRect(unsigned int gid) {
		SDL_Rect rect = { 0 };

		int relativeIndex = gid - firstGid;
		rect.w = tileWidth;
		rect.h = tileHeight;
		rect.x = margin + (tileWidth + spacing) * (relativeIndex % columns);
		rect.y = margin + (tileHeight + spacing) * (relativeIndex / columns);

		return rect;
	}

};

// L06: TODO 1: Create a struct needed to hold the information to Map node
struct MapData
{
	int width;
	int height;
	int tileWidth;
	int tileHeight;
	std::list<TileSet*> tilesets;

	// L07: TODO 2: Add the info to the MapLayer Struct
	std::list<MapLayer*> layers;
};

class Map : public Module
{
public:

	Map();

	// Destructor
	virtual ~Map();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	bool NewLevelCleanUp();

	// Load new map
	bool Load(std::string path, std::string mapFileName);

	// L07: TODO 8: Create a method that translates x,y coordinates from map positions to world positions
	Vector2D MapToWorld(int x, int y) const;

	// L09: TODO 2: Implement function to the Tileset based on a tile id
	TileSet* GetTilesetFromTileId(int gid) const;

	// L09: TODO 6: Load a group of properties 
	bool LoadProperties(pugi::xml_node& node, Properties& properties);

	MapLayer* GetNavigationLayer();

	Vector2D WorldToMap(int x, int y);

	int GetWidth() {
		return mapData.width;
	}

	int GetHeight() {
		return mapData.height;
	}

	std::vector<Vector2D> GetEnemyList() {
		return posEnemy;
	}

	std::vector<Vector2D> GetChestList() {
		return posChest;
	}

	std::vector<Vector2D> GetRandomEventList() {
		return posRandomEvent;
	}

	std::map<int, Vector2D> GetDoorList() {
		return posDoor;
	}

	std::map<int, Vector2D> GetBossList() {
		return posBoss;
	}

public:
	std::string mapFileName;
	std::string mapPath;

private:
	bool mapLoaded;
	MapData mapData;
	std::vector<PhysBody*> collisions;
	std::vector<Vector2D> posEnemy;
	std::vector<Vector2D> posChest;
	std::vector<Vector2D> posRandomEvent;
	std::map<int, Vector2D> posDoor;
	std::map<int, Vector2D> posBoss;
};