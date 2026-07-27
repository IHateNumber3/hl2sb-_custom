//========== Copyleft © 2011, Team Sandbox, Some rights reserved. ===========//
//
// Purpose:
//
//===========================================================================//

#include "cbase.h"
#include "filesystem.h"
#include "luamanager.h"
#include "tier0/icommandline.h"
#include "zip/miniz.h"
#include <stdio.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// Weapon and entity classes discovered in mounted addons this session. The
// HL2:SB++ spawn menu is built entirely from settings/spawnlists/*.kv manifest
// files and never enumerates registered SWEPs/SENTs, so a mounted addon's
// content would load but never appear in the menu. We collect the classes here
// and, once Lua has registered them, emit generated spawnlist tabs that mirror
// each class's own SWEP.Category / ENT.Category (like GMod does) instead of
// dumping everything under a single "addons" tab.
static CUtlVector<CUtlString> g_AddonWeaponClasses;
static CUtlVector<CUtlString> g_AddonEntityClasses;

static void CollectClassInto( CUtlVector<CUtlString> &vec, const char *rest )
{
	// rest points just past the "lua/weapons/" or "lua/entities/" prefix, e.g.
	// "weapon_x.lua" or "weapon_x/shared.lua". Extract the class (weapon_x).
	if ( rest[0] == '\0' )
		return;

	char cls[128];
	int	 i = 0;
	for ( ; rest[i] && rest[i] != '/' && rest[i] != '.' && i < (int)sizeof( cls ) - 1; i++ )
		cls[i] = rest[i];
	cls[i] = '\0';
	if ( cls[0] == '\0' )
		return;

	FOR_EACH_VEC( vec, k )
	{
		if ( Q_stricmp( vec[k].Get(), cls ) == 0 )
			return; // already have it (folder SWEP/SENT has several files)
	}
	vec.AddToTail( CUtlString( cls ) );
}

static void CollectAddonFileClass( const char *fileRelName )
{
	// fileRelName is a GMA-relative path like "lua/weapons/weapon_x/shared.lua"
	// or "lua/entities/sent_x.lua".
	if ( Q_strnicmp( fileRelName, "lua/weapons/", 12 ) == 0 )
		CollectClassInto( g_AddonWeaponClasses, fileRelName + 12 );
	else if ( Q_strnicmp( fileRelName, "lua/entities/", 13 ) == 0 )
		CollectClassInto( g_AddonEntityClasses, fileRelName + 13 );
}

// Push registry.get(cls) (registry is "weapon" or "entity") onto the Lua stack.
// Returns true with the class table left on the stack, false with the stack
// balanced.
static bool PushAddonClassTable( const char *registry, const char *cls )
{
	if ( !L )
		return false;

	lua_getglobal( L, registry );
	if ( !lua_istable( L, -1 ) )
	{
		lua_pop( L, 1 );
		return false;
	}
	lua_getfield( L, -1, "get" );
	if ( !lua_isfunction( L, -1 ) )
	{
		lua_pop( L, 2 );
		return false;
	}
	lua_remove( L, -2 ); // drop the registry table, keep the get() function
	lua_pushstring( L, cls );
	if ( luasrc_pcall( L, 1, 1, 0 ) != 0 )
	{
		lua_pop( L, 1 ); // error object
		return false;
	}
	if ( !lua_istable( L, -1 ) )
	{
		lua_pop( L, 1 );
		return false;
	}
	return true; // class table left on stack
}

static void ReadTableString( int tableIdx, const char *field, const char *def, char *out, int outLen )
{
	lua_getfield( L, tableIdx, field );
	const char *s = lua_isstring( L, -1 ) ? lua_tostring( L, -1 ) : NULL;
	Q_strncpy( out, ( s && s[0] ) ? s : def, outLen );
	lua_pop( L, 1 );
}

// One spawnable item in the menu.
struct AddonSpawnItem
{
	CUtlString cls;
	CUtlString printName;
};

// One category -> the items declared under it.
struct AddonSpawnCategory
{
	CUtlString				  name;
	CUtlVector<AddonSpawnItem> items;
};

static AddonSpawnCategory *FindOrAddCategory( CUtlVector<AddonSpawnCategory> &cats, const char *name )
{
	FOR_EACH_VEC( cats, i )
	{
		if ( Q_stricmp( cats[i].name.Get(), name ) == 0 )
			return &cats[i];
	}
	int idx = cats.AddToTail();
	cats[idx].name = name;
	return &cats[idx];
}

// Turn a category name into a safe, unique-ish filename component.
static void SanitizeForFilename( const char *in, char *out, int outLen )
{
	int j = 0;
	for ( int i = 0; in[i] && j < outLen - 1; i++ )
	{
		char c = in[i];
		if ( ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) || ( c >= '0' && c <= '9' ) )
			out[j++] = ( c >= 'A' && c <= 'Z' ) ? ( c - 'A' + 'a' ) : c;
		else if ( c == ' ' || c == '-' || c == '_' )
			out[j++] = '_';
	}
	out[j] = '\0';
	if ( j == 0 )
		Q_strncpy( out, "other", outLen );
}

// Resolve the spawn-icon material path for a class, GMod-preference order:
//  1. materials/vgui/entities/<class>.vmt  (VGUI material - pass without the
//     "materials/" prefix but WITH extension)
//  2. materials/entities/<class>.png       (loaded as a PNG file)
//  3. a stock fallback icon
static void ResolveClassIcon( const char *cls, char *icon, int iconLen )
{
	char probe[MAX_PATH];
	Q_snprintf( probe, sizeof( probe ), "materials/vgui/entities/%s.vmt", cls );
	if ( filesystem->FileExists( probe, "GAME" ) )
	{
		Q_snprintf( icon, iconLen, "vgui/entities/%s.vmt", cls );
		return;
	}
	Q_snprintf( icon, iconLen, "materials/entities/%s.png", cls );
	if ( !filesystem->FileExists( icon, "GAME" ) )
		Q_strncpy( icon, "materials/entities/weapon_crowbar.png", iconLen );
}

// Remove any spawnlist file this system generated on a previous mount so stale
// categories don't linger after addons change.
static void RemoveGeneratedSpawnlists()
{
	// Legacy single-tab file from older builds.
	if ( filesystem->FileExists( "settings/spawnlists/zz_addons.kv", "MOD" ) )
		filesystem->RemoveFile( "settings/spawnlists/zz_addons.kv", "MOD" );

	FileFindHandle_t fh;
	const char		*fn = g_pFullFileSystem->FindFirstEx( "settings/spawnlists/zz_addon_*.kv", "MOD", &fh );
	while ( fn )
	{
		char path[MAX_PATH];
		Q_snprintf( path, sizeof( path ), "settings/spawnlists/%s", fn );
		filesystem->RemoveFile( path, "MOD" );
		fn = g_pFullFileSystem->FindNext( fh );
	}
	g_pFullFileSystem->FindClose( fh );
}

// Emit one settings/spawnlists/zz_addon_<category>.kv per category, so mounted
// addon SWEPs/SENTs appear in the spawn menu under their own declared category
// (SWEP.Category / ENT.Category), exactly like GMod - not forced under an
// "addons" tab. Clicking an entry runs ent_create <class> (FCVAR_GAMEDLL, no
// sv_cheats needed) which spawns the registered scripted weapon/entity/NPC.
void WriteAddonSpawnlists()
{
	RemoveGeneratedSpawnlists();

	if ( g_AddonWeaponClasses.Count() == 0 && g_AddonEntityClasses.Count() == 0 )
		return;

	CUtlVector<AddonSpawnCategory> cats;

	// Weapons -> SWEP.Category (GMod defaults this to "Other").
	FOR_EACH_VEC( g_AddonWeaponClasses, k )
	{
		const char *cls = g_AddonWeaponClasses[k].Get();
		char		category[128];
		char		printName[128];
		Q_strncpy( category, "Other", sizeof( category ) );
		Q_strncpy( printName, cls, sizeof( printName ) );
		if ( PushAddonClassTable( "weapon", cls ) )
		{
			ReadTableString( -1, "Category", "Other", category, sizeof( category ) );
			ReadTableString( -1, "PrintName", cls, printName, sizeof( printName ) );
			lua_pop( L, 1 );
		}

		AddonSpawnCategory *pCat = FindOrAddCategory( cats, category );
		int					ii = pCat->items.AddToTail();
		pCat->items[ii].cls = cls;
		pCat->items[ii].printName = printName;
	}

	// Entities / NPCs -> ENT.Category (GMod defaults this to "Other").
	FOR_EACH_VEC( g_AddonEntityClasses, k )
	{
		const char *cls = g_AddonEntityClasses[k].Get();
		char		category[128];
		char		printName[128];
		Q_strncpy( category, "Other", sizeof( category ) );
		Q_strncpy( printName, cls, sizeof( printName ) );
		if ( PushAddonClassTable( "entity", cls ) )
		{
			ReadTableString( -1, "Category", "Other", category, sizeof( category ) );
			ReadTableString( -1, "PrintName", cls, printName, sizeof( printName ) );
			lua_pop( L, 1 );
		}

		AddonSpawnCategory *pCat = FindOrAddCategory( cats, category );
		int					ii = pCat->items.AddToTail();
		pCat->items[ii].cls = cls;
		pCat->items[ii].printName = printName;
	}

	filesystem->CreateDirHierarchy( "settings/spawnlists", "MOD" );

	int nWritten = 0;
	FOR_EACH_VEC( cats, c )
	{
		AddonSpawnCategory &cat = cats[c];

		char safe[128];
		SanitizeForFilename( cat.name.Get(), safe, sizeof( safe ) );

		char kPath[MAX_PATH];
		Q_snprintf( kPath, sizeof( kPath ), "settings/spawnlists/zz_addon_%s.kv", safe );

		FileHandle_t fh = g_pFullFileSystem->Open( kPath, "wt", "MOD" );
		if ( fh == FILESYSTEM_INVALID_HANDLE )
		{
			Warning( "Failed to write generated addon spawnlist: %s\n", kPath );
			continue;
		}

		// Tab name == the category itself, so the menu shows a natural tab.
		g_pFullFileSystem->FPrintf( fh, "\"%s\"\n{\n", cat.name.Get() );
		g_pFullFileSystem->FPrintf( fh, "\t\"page_icon\" \"materials/icon16/bricks.png\"\n" );

		FOR_EACH_VEC( cat.items, ii )
		{
			const AddonSpawnItem &item = cat.items[ii];

			char icon[MAX_PATH];
			ResolveClassIcon( item.cls.Get(), icon, sizeof( icon ) );

			g_pFullFileSystem->FPrintf( fh, "\t\"image_button\"\n\t{\n" );
			g_pFullFileSystem->FPrintf( fh, "\t\t\"name\" \"%s\"\n", item.printName.Get() );
			g_pFullFileSystem->FPrintf( fh, "\t\t\"image\" \"%s\"\n", icon );
			g_pFullFileSystem->FPrintf( fh, "\t\t\"command\" \"ent_create %s\"\n", item.cls.Get() );
			g_pFullFileSystem->FPrintf( fh, "\t}\n" );
		}

		g_pFullFileSystem->FPrintf( fh, "}\n" );
		g_pFullFileSystem->Close( fh );
		nWritten++;
	}

	DevMsg( "Wrote %d generated addon spawnlist tab(s) for %d weapon(s), %d entity(ies).\n",
			nWritten, g_AddonWeaponClasses.Count(), g_AddonEntityClasses.Count() );
}

static void ExtractZIP( const char *zipPath, const char *cacheBase, const char *gamePath )
{
	if ( !zipPath || !cacheBase )
		return;

	const char *baseNamePtr = V_UnqualifiedFileName( zipPath ); // "popcorn_swep.zip"
	char		baseName[MAX_PATH];
	Q_strncpy( baseName, baseNamePtr ? baseNamePtr : zipPath, sizeof( baseName ) );

	char addonName[MAX_PATH];
	Q_strncpy( addonName, baseName, sizeof( addonName ) );
	char *dot = Q_strrchr( addonName, '.' );
	if ( dot )
		*dot = '\0';

	char relOutBase[MAX_PATH];
	Q_snprintf( relOutBase, sizeof( relOutBase ), "%s/zips/%s", cacheBase, addonName );
	Q_FixSlashes( relOutBase );

	filesystem->CreateDirHierarchy( relOutBase, "MOD" );

	mz_zip_archive zip;
	memset( &zip, 0, sizeof( zip ) );
	if ( !mz_zip_reader_init_file( &zip, zipPath, 0 ) )
	{
		Warning( "Failed to open ZIP: %s\n", zipPath );
		return;
	}

	int numFiles = (int)mz_zip_reader_get_num_files( &zip );
	for ( int i = 0; i < numFiles; ++i )
	{
		mz_zip_archive_file_stat fileStat;
		if ( !mz_zip_reader_file_stat( &zip, i, &fileStat ) )
			continue;

		if ( mz_zip_reader_is_file_a_directory( &zip, i ) )
			continue;

		const char *fileName = fileStat.m_filename;
		const char *firstSlash = strchr( fileName, '/' );
		if ( firstSlash )
		{
			fileName = firstSlash + 1;
		}

		if ( !fileName || fileName[0] == '\0' )
		{
			DevMsg( "Skipping empty path from: %s\n", fileStat.m_filename );
			continue;
		}

		DevMsg( "Extracting: %s -> %s/%s\n", fileStat.m_filename, relOutBase, fileName );

		char relOutPath[MAX_PATH];
		Q_snprintf( relOutPath, sizeof( relOutPath ), "%s/%s", relOutBase, fileName );
		Q_FixSlashes( relOutPath );

		char dirBuf[MAX_PATH];
		Q_strncpy( dirBuf, relOutPath, sizeof( dirBuf ) );
		char *lastSlash = Q_strrchr( dirBuf, '/' );
		if ( lastSlash )
		{
			*lastSlash = '\0'; // keep directory only
			filesystem->CreateDirHierarchy( dirBuf, "MOD" );
		}
		else
			filesystem->CreateDirHierarchy( relOutBase, "MOD" );

		size_t uncomp_size = 0;
		void  *p = mz_zip_reader_extract_to_heap( &zip, i, &uncomp_size, 0 );
		if ( !p )
		{
			Warning( "Failed to extract (to heap) %s from ZIP %s\n", fileStat.m_filename, zipPath );
			continue;
		}

		FileHandle_t fh = g_pFullFileSystem->Open( relOutPath, "wb", "MOD" );
		if ( fh == FILESYSTEM_INVALID_HANDLE )
		{
			Warning( "Failed to open output '%s' for writing in Source FS\n", relOutPath );
			mz_free( p );
			continue;
		}

		if ( uncomp_size > INT_MAX )
		{
			Warning( "File too large to write via Source FS: %s (size=%zu)\n", relOutPath, uncomp_size );
			g_pFullFileSystem->Close( fh );
			mz_free( p );
			continue;
		}

		int written = g_pFullFileSystem->Write( p, (int)uncomp_size, fh );
		if ( written != (int)uncomp_size )
		{
			Warning( "Write mismatch for %s (wrote %d, expected %zu)\n", relOutPath, written, uncomp_size );
		}
		g_pFullFileSystem->Close( fh );

		// free buffer from miniz
		mz_free( p );
	}

	mz_zip_reader_end( &zip );

	char absOutBase[MAX_PATH];
	Q_snprintf( absOutBase, sizeof( absOutBase ), "%s/%s", gamePath, relOutBase );
	Q_FixSlashes( absOutBase );

	filesystem->AddSearchPath( absOutBase, "MOD", PATH_ADD_TO_HEAD );
	filesystem->AddSearchPath( absOutBase, "GAME", PATH_ADD_TO_HEAD );

	DevMsg( "Mounted ZIP addon: %s -> %s\n", zipPath, absOutBase );
}

//-----------------------------------------------------------------------------
// GMA (Garry's Mod addon) support.
//
// A .gma is not a zip and the Source filesystem has no reader for it, so
// AddSearchPath() on a .gma silently does nothing. Instead we parse the GMAD
// container and stream every file out into cache/gma/<addon>/, then mount that
// folder just like an extracted zip. Format (little-endian, matches iOS):
//   char[4] "GMAD" | u8 version | u64 steamid | u64 timestamp
//   [version>1] required-content: NUL strings until an empty one
//   string name | string description | string author | i32 addonVersion
//   index: repeat{ u32 fileNumber(0=end) | string name | i64 size | u32 crc }
//   then every file's bytes concatenated in index order, then u32 addonCRC.
//-----------------------------------------------------------------------------
#define GMAD_MAX_VERSION 3
#define GMAD_MAX_FILES   65536

static bool GMA_ReadRaw( FILE *f, void *out, size_t n )
{
	return fread( out, 1, n, f ) == n;
}

// Reads a NUL-terminated string. Pass out==NULL to skip/discard it.
static bool GMA_ReadString( FILE *f, char *out, size_t outSize )
{
	size_t i = 0;
	for ( ;; )
	{
		int c = fgetc( f );
		if ( c == EOF )
			return false;
		if ( c == 0 )
			break;
		if ( out && i + 1 < outSize )
			out[i++] = (char)c;
	}
	if ( out )
		out[i] = '\0';
	return true;
}

struct gma_entry_t
{
	char	  name[MAX_PATH];
	long long size;
};

static void ExtractGMA( const char *gmaPath, const char *cacheBase, const char *gamePath )
{
	if ( !gmaPath || !cacheBase )
		return;

	FILE *f = fopen( gmaPath, "rb" );
	if ( !f )
	{
		Warning( "Failed to open GMA: %s\n", gmaPath );
		return;
	}

	// Header.
	char ident[4];
	if ( !GMA_ReadRaw( f, ident, 4 ) || Q_strncmp( ident, "GMAD", 4 ) != 0 )
	{
		Warning( "Not a valid GMA (bad ident): %s\n", gmaPath );
		fclose( f );
		return;
	}

	unsigned char version = 0;
	unsigned long long steamid = 0, timestamp = 0;
	if ( !GMA_ReadRaw( f, &version, 1 ) ||
		 !GMA_ReadRaw( f, &steamid, 8 ) ||
		 !GMA_ReadRaw( f, &timestamp, 8 ) )
	{
		Warning( "Truncated GMA header: %s\n", gmaPath );
		fclose( f );
		return;
	}

	if ( version > GMAD_MAX_VERSION )
	{
		Warning( "Unsupported GMA version %d (max %d): %s\n", version, GMAD_MAX_VERSION, gmaPath );
		fclose( f );
		return;
	}

	// Required-content list (discarded).
	if ( version > 1 )
	{
		char tmp[256];
		do
		{
			if ( !GMA_ReadString( f, tmp, sizeof( tmp ) ) )
			{
				Warning( "Truncated GMA required-content list: %s\n", gmaPath );
				fclose( f );
				return;
			}
		} while ( tmp[0] != '\0' );
	}

	int addonVersion = 0;
	if ( !GMA_ReadString( f, NULL, 0 ) ||	// addon name
		 !GMA_ReadString( f, NULL, 0 ) ||	// description (json)
		 !GMA_ReadString( f, NULL, 0 ) ||	// author
		 !GMA_ReadRaw( f, &addonVersion, 4 ) )
	{
		Warning( "Truncated GMA metadata: %s\n", gmaPath );
		fclose( f );
		return;
	}

	// File index.
	CUtlVector<gma_entry_t> entries;
	for ( ;; )
	{
		unsigned int fileNumber = 0;
		if ( !GMA_ReadRaw( f, &fileNumber, 4 ) )
		{
			Warning( "Truncated GMA file index: %s\n", gmaPath );
			fclose( f );
			return;
		}
		if ( fileNumber == 0 )
			break;

		gma_entry_t e;
		unsigned int crc = 0;
		if ( !GMA_ReadString( f, e.name, sizeof( e.name ) ) ||
			 !GMA_ReadRaw( f, &e.size, 8 ) ||
			 !GMA_ReadRaw( f, &crc, 4 ) )
		{
			Warning( "Truncated GMA index entry: %s\n", gmaPath );
			fclose( f );
			return;
		}

		if ( e.size < 0 )
		{
			Warning( "Bad GMA entry size for '%s' in %s\n", e.name, gmaPath );
			fclose( f );
			return;
		}

		entries.AddToTail( e );
		if ( entries.Count() > GMAD_MAX_FILES )
		{
			Warning( "GMA has too many files (>%d): %s\n", GMAD_MAX_FILES, gmaPath );
			fclose( f );
			return;
		}
	}

	// Output base: cache/gma/<addonName>
	char baseName[MAX_PATH];
	const char *baseNamePtr = V_UnqualifiedFileName( gmaPath );
	Q_strncpy( baseName, baseNamePtr ? baseNamePtr : gmaPath, sizeof( baseName ) );
	char *dot = Q_strrchr( baseName, '.' );
	if ( dot )
		*dot = '\0';

	char relOutBase[MAX_PATH];
	Q_snprintf( relOutBase, sizeof( relOutBase ), "%s/gma/%s", cacheBase, baseName );
	Q_FixSlashes( relOutBase );
	filesystem->CreateDirHierarchy( relOutBase, "MOD" );

	// The file block starts right here, files stored in index order.
	char chunk[65536];
	FOR_EACH_VEC( entries, idx )
	{
		gma_entry_t &e = entries[idx];

		// Reject path traversal so a malformed/hostile addon can't escape cache.
		if ( Q_strstr( e.name, ".." ) )
		{
			Warning( "Skipping unsafe GMA path: %s\n", e.name );
			// Still must consume its bytes to stay aligned.
			long long skip = e.size;
			while ( skip > 0 )
			{
				size_t want = ( skip > (long long)sizeof( chunk ) ) ? sizeof( chunk ) : (size_t)skip;
				if ( fread( chunk, 1, want, f ) != want )
					break;
				skip -= (long long)want;
			}
			continue;
		}

		char relOutPath[MAX_PATH];
		Q_snprintf( relOutPath, sizeof( relOutPath ), "%s/%s", relOutBase, e.name );
		Q_FixSlashes( relOutPath );

		char dirBuf[MAX_PATH];
		Q_strncpy( dirBuf, relOutPath, sizeof( dirBuf ) );
		char *lastSlash = Q_strrchr( dirBuf, '/' );
		if ( lastSlash )
		{
			*lastSlash = '\0';
			filesystem->CreateDirHierarchy( dirBuf, "MOD" );
		}

		FileHandle_t ofh = g_pFullFileSystem->Open( relOutPath, "wb", "MOD" );
		if ( ofh == FILESYSTEM_INVALID_HANDLE )
			Warning( "Failed to open GMA output '%s'\n", relOutPath );

		long long remaining = e.size;
		while ( remaining > 0 )
		{
			size_t want = ( remaining > (long long)sizeof( chunk ) ) ? sizeof( chunk ) : (size_t)remaining;
			if ( fread( chunk, 1, want, f ) != want )
			{
				Warning( "Truncated GMA data for '%s' in %s\n", e.name, gmaPath );
				remaining = 0;
				if ( ofh != FILESYSTEM_INVALID_HANDLE )
					g_pFullFileSystem->Close( ofh );
				fclose( f );
				return;
			}
			if ( ofh != FILESYSTEM_INVALID_HANDLE )
				g_pFullFileSystem->Write( chunk, (int)want, ofh );
			remaining -= (long long)want;
		}

		if ( ofh != FILESYSTEM_INVALID_HANDLE )
			g_pFullFileSystem->Close( ofh );

		DevMsg( "Extracted GMA file: %s\n", relOutPath );
	}

	fclose( f );

	char absOutBase[MAX_PATH];
	Q_snprintf( absOutBase, sizeof( absOutBase ), "%s/%s", gamePath, relOutBase );
	Q_FixSlashes( absOutBase );

	// Mounting the addon root on MOD/GAME is enough: the entity/weapon scanner
	// globs "lua/entities/*" and "lua/weapons/*" across every MOD search path
	// (see luamanager.cpp), so a mounted addon's SWEPs/SENTs get registered too.
	filesystem->AddSearchPath( absOutBase, "MOD", PATH_ADD_TO_HEAD );
	filesystem->AddSearchPath( absOutBase, "GAME", PATH_ADD_TO_HEAD );

	DevMsg( "Mounted GMA addon: %s -> %s (%d files)\n", gmaPath, absOutBase, entries.Count() );

	// Record any SWEPs/SENTs so they can be surfaced in the spawn menu once Lua
	// has registered them (see WriteAddonSpawnlists).
	FOR_EACH_VEC( entries, wi )
		CollectAddonFileClass( entries[wi].name );
}

static void MountAddonFolder( const char *folder, const char *gamePath )
{
	FileFindHandle_t fh;
	char			 searchPath[MAX_PATH];
	Q_snprintf( searchPath, sizeof( searchPath ), "%s/*", folder );

	const char *fn = g_pFullFileSystem->FindFirstEx( searchPath, "MOD", &fh );
	while ( fn )
	{
		if ( fn[0] != '.' )
		{
			char fullPath[MAX_PATH];
			Q_snprintf( fullPath, sizeof( fullPath ), "%s/%s/%s", gamePath, folder, fn );
			Q_FixSlashes( fullPath );

			if ( g_pFullFileSystem->FindIsDirectory( fh ) )
			{
				DevMsg( "Mounting %s dir: %s\n", folder, fullPath );
				filesystem->AddSearchPath( fullPath, "MOD", PATH_ADD_TO_HEAD );
				filesystem->AddSearchPath( fullPath, "GAME", PATH_ADD_TO_HEAD );
			}
			else if ( Q_stristr( fn, ".vpk" ) )
			{
				DevMsg( "Mounting %s VPK: %s\n", folder, fullPath );
				filesystem->AddSearchPath( fullPath, "MOD", PATH_ADD_TO_HEAD );
				filesystem->AddSearchPath( fullPath, "GAME", PATH_ADD_TO_HEAD );
			}
			else if ( Q_stristr( fn, ".gma" ) )
			{
				DevMsg( "Mounting %s GMA: %s\n", folder, fullPath );
				ExtractGMA( fullPath, "cache", gamePath );
			}
			else if ( Q_stristr( fn, ".zip" ) )
			{
				DevMsg( "Mounting %s ZIP: %s\n", folder, fullPath );
				ExtractZIP( fullPath, "cache", gamePath );
			}
		}

		fn = g_pFullFileSystem->FindNext( fh );
	}

	g_pFullFileSystem->FindClose( fh );
}

void MountAddons()
{
	if ( CommandLine()->CheckParm( "-noaddons" ) )
	{
		DevMsg( "Addons mounting skipped due to -noaddons parameter.\n" );
		return;
	}

	char gamePath[MAX_PATH] = { 0 };
#ifdef CLIENT_DLL
	Q_strncpy( gamePath, engine->GetGameDirectory(), sizeof( gamePath ) );
#else
	engine->GetGameDir( gamePath, sizeof( gamePath ) );
#endif

	// NOO! I DON'T WANT TO DIE!
	filesystem->CreateDirHierarchy( "cache/zips", "MOD" );
	filesystem->CreateDirHierarchy( "cache/gma", "MOD" );

	filesystem->AddSearchPath( LUA_PATH_CACHE, "MOD", PATH_ADD_TO_HEAD );
	filesystem->AddSearchPath( LUA_PATH_CACHE, "GAME", PATH_ADD_TO_HEAD );

	g_AddonWeaponClasses.RemoveAll();
	g_AddonEntityClasses.RemoveAll();

	MountAddonFolder( "addons", gamePath );
	MountAddonFolder( "custom", gamePath );
	MountAddonFolder( "mods", gamePath );

	// NOTE: the spawn-menu tabs are written later, from WriteAddonSpawnlists(),
	// once luasrc_LoadWeapons()/LoadEntities() have registered the classes so we
	// can read each one's real SWEP.Category / ENT.Category. At mount time the
	// Lua registries don't exist yet.
}

extern void lcf_recursivedeletefile( const char *current );

void UnMountAddons()
{
	lcf_recursivedeletefile( "cache/zips" );
	lcf_recursivedeletefile( "cache/gma" );
}