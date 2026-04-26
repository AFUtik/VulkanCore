#include "Assets.hpp"

void Assets::load()
{ 
    auto tileset = std::make_unique<Tileset>(RESOURCES_PATH+"img/tileset.png", 16, 16, 1);

    tilesetMap.emplace("BaseTileset", tileset.get());
    tilesets.push_back(std::move(tileset));

	//TextureUtils::save(&tileset->texture, RESOURCES_PATH+"img/paddedTileset.png");

	atlasBuilder.reserve(64);
	atlasBuilder.setPadding(1);

	auto atlas = std::make_unique<TextureAtlas>();
	atlas->texture = Texture(64, 64, TextureChannels::RGBA);
    
    Texture stone(RESOURCES_PATH+"img/stone.png");
	Texture tuff (RESOURCES_PATH+"img/tuff.png");
	Texture sand (RESOURCES_PATH+"img/sand.png");

	atlasBuilder.pack(atlas.get(), &stone, "stone");
	atlasBuilder.pack(atlas.get(), &tuff,  "tuff");
	atlasBuilder.pack(atlas.get(), &sand,  "sand");
	atlasBuilder.build(atlas.get());

    atlasMap.emplace("BaseAtlas", atlas.get());
    atlases.push_back(std::move(atlas)); 

	//TextureUtils::save(&atlas.texture, RESOURCES_PATH+"img/atlas.png");
}

void Assets::reload() 
{

}