//========== Copyleft � 2011, Team Sandbox, Some rights reserved. ===========//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#ifndef MOUNTADDONS_H
#define MOUNTADDONS_H
#ifdef _WIN32
#pragma once
#endif

void MountAddons();
void UnMountAddons();

// Regenerate the spawn-menu tabs for mounted-addon SWEPs/SENTs. Must be called
// AFTER luasrc_LoadWeapons()/luasrc_LoadEntities() so the Lua weapon/entity
// registries are populated and each class's real Category/PrintName can be read.
void WriteAddonSpawnlists();

#endif // MOUNTADDONS_H
