//========== Copyleft � 2011, Team Sandbox, Some rights reserved. ===========//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//


#define lsrcinit_cpp

#include "cbase.h"
#include "lua.hpp"

#include "luasrclib.h"
#include "lauxlib.h"
#include "lbaseplayer_shared.h"  // lua_pushplayer, for the LocalPlayer() global
#ifdef CLIENT_DLL
#include "c_baseplayer.h"        // C_BasePlayer::GetLocalPlayer()
#endif


static const luaL_Reg luasrclibs[] = {
  {LUA_BASEANIMATINGLIBNAME, luaopen_CBaseAnimating},
  {LUA_BASECOMBATWEAPONLIBNAME, luaopen_CBaseCombatWeapon},
  {LUA_BASEENTITYLIBNAME, luaopen_CBaseEntity},
  {LUA_BASEENTITYLIBNAME, luaopen_CBaseEntity_shared},
  {LUA_BASEPLAYERLIBNAME, luaopen_CBasePlayer},
  {LUA_BASEPLAYERLIBNAME, luaopen_CBasePlayer_shared},
  {LUA_EFFECTDATALIBNAME, luaopen_CEffectData},
  {LUA_GAMETRACELIBNAME, luaopen_CGameTrace},
#ifdef SBPP
  {LUA_NETLIBNAME, luaopen_net},
#endif
#ifndef CLIENT_DLL
  {LUA_EFFECTSLIBNAME, luaopen_Effects},
  {LUA_HL2MPPLAYERLIBNAME, luaopen_CHL2MP_Player},
#ifdef SBPP
  {LUA_NAVAREALIBNAME, luaopen_CNavArea},
  {LUA_NAVMESHLIBNAME, luaopen_navmesh},
  {LUA_MOVEDATALIBNAME, luaopen_CMoveData},
	{LUA_BASEFLEXLIBNAME, luaopen_CBaseFlex},
	{LUA_BASEANIMATINGOVERLAYLIBNAME, luaopen_CBaseAnimatingOverlay},
  
	{LUA_NEXTBOT_LOCOMOTIONLIBNAME, luaopen_NextBotLocomotion},
	{LUA_NEXTBOT_PATHLIBNAME, luaopen_CNextBotPath},
	{LUA_NEXTBOTLIBNAME, luaopen_CLuaNextBot},

#endif
#else
#ifdef SBPP
  {LUA_SPAWNMENULIBNAME, luaopen_sm},
	{LUA_BASEFLEXLIBNAME, luaopen_CBaseFlex},
	{LUA_BASEANIMATINGOVERLAYLIBNAME, luaopen_CBaseAnimatingOverlay},

  // material crap
  {LUA_IMESHLIBNAME, luaopen_IMesh},
  {"materials", luaopen_materials},
  {LUA_MESHBUILDERLIBNAME, luaopen_CMeshBuilder},
  {LUA_TEXTURELIBNAME, luaopen_ITexture},
  {LUA_MATRENDERCONTEXTLIBNAME, luaopen_IMatRenderContext},
  {LUA_MATERIALVARLIBNAME, luaopen_IMaterialVar},
#endif
#endif
  {LUA_AMMODEFLIB_NAME, luaopen_AmmoDef},
  {LUA_HL2MPPLAYERLIBNAME, luaopen_CHL2MP_Player_shared},
  {LUA_COLORLIBNAME, luaopen_Color},
  {LUA_CONCOMMANDLIBNAME, luaopen_ConCommand},
  {LUA_CONTENTSLIBNAME, luaopen_CONTENTS},
  {LUA_CONVARLIBNAME, luaopen_ConVar},
  {LUA_PASFILTERLIBNAME, luaopen_CPASFilter},
  {LUA_RECIPIENTFILTERLIBNAME, luaopen_CRecipientFilter},
  {LUA_TAKEDAMAGEINFOLIBNAME, luaopen_CTakeDamageInfo},
  {LUA_CVARLIBNAME, luaopen_cvar},
  {LUA_DBGLIBNAME, luaopen_dbg},
  {LUA_DEBUGOVERLAYLIBNAME, luaopen_debugoverlay},
  {LUA_ENGINELIBNAME, luaopen_engine},
#ifdef CLIENT_DLL
  // FIXME: obsolete? should be passing VPANELs, but passes Panel instead,
  // which always ends up being invalid (we can't access them by pointer)
  {LUA_ENGINEVGUILIBNAME, luaopen_enginevgui},
#endif
  {LUA_FCVARLIBNAME, luaopen_FCVAR},
  {LUA_FILESYSTEMLIBNAME, luaopen_filesystem},
#ifdef CLIENT_DLL
  {LUA_FONTFLAGLIBNAME, luaopen_FONTFLAG},
#endif
#ifndef CLIENT_DLL
  {LUA_ENTLISTLIBNAME, luaopen_gEntList},
#endif
  {LUA_GLOBALSLIBNAME, luaopen_gpGlobals},
#ifdef CLIENT_DLL
  {LUA_CLIENTSHADOWMGRLIBNAME, luaopen_g_pClientShadowMgr},
  {LUA_FONTLIBNAME, luaopen_HFont},
  {LUA_HSCHEMELIBNAME, luaopen_HScheme},
#endif
  {LUA_MATERIALLIBNAME, luaopen_IMaterial},
  {LUA_MOVEHELPERLIBNAME, luaopen_IMoveHelper},
  {LUA_INLIBNAME, luaopen_IN},
#ifndef CLIENT_DLL
  {LUA_NETCHANNELINFOLIBNAME, luaopen_INetChannelInfo},
#endif
  {LUA_INETWORKSTRINGTABLELIBNAME, luaopen_INetworkStringTable},
#ifdef CLIENT_DLL
  {LUA_INPUTLIBNAME, luaopen_input},
#endif
  {LUA_PHYSICSOBJECTLIBNAME, luaopen_IPhysicsObject},
  {LUA_PHYSICSSURFACEPROPSLIBNAME, luaopen_IPhysicsSurfaceProps},
  {LUA_PREDICTIONSYSTEMLIBNAME, luaopen_IPredictionSystem},
#ifdef CLIENT_DLL
  {LUA_ISCHEMELIBNAME, luaopen_IScheme},
#endif
//  {LUA_STEAMFRIENDSLIBNAME, luaopen_ISteamFriends},
  {LUA_KEYVALUESLIBNAME, luaopen_KeyValues},
  {LUA_MASKLIBNAME, luaopen_MASK},
  {LUA_MATHLIBLIBNAME, luaopen_mathlib},
  {LUA_MATRIXLIBNAME, luaopen_matrix3x4_t},
  {LUA_NETWORKSTRINGTABLELIBNAME, luaopen_networkstringtable},
#ifdef CLIENT_DLL
  {LUA_PANELLIBNAME, luaopen_Panel},
#endif
  {LUA_PHYSENVLIBNAME, luaopen_physenv},
#ifdef CLIENT_DLL
  {LUA_PREDICTIONLIBNAME, luaopen_prediction},
#endif
  {LUA_QANGLELIBNAME, luaopen_QAngle},
  {LUA_RANDOMLIBNAME, luaopen_random},
#ifdef CLIENT_DLL
  {LUA_SCHEMELIBNAME, luaopen_scheme},
#endif
//  {LUA_STEAMAPICONTEXTLIBNAME, luaopen_steamapicontext},
  {LUA_SURFLIBNAME, luaopen_SURF},
#ifdef CLIENT_DLL
  {LUA_SURFACELIBNAME, luaopen_surface},
#endif
  {LUA_UTILLIBNAME, luaopen_UTIL},
  {LUA_UTILLIBNAME, luaopen_UTIL_shared},
  {LUA_VECTORLIBNAME, luaopen_Vector},
#ifdef CLIENT_DLL
  {LUA_VGUILIBNAME, luaopen_vgui},
#endif
  {LUA_VMATRIXLIBNAME, luaopen_VMatrix},
  {LUA_COLLISION_GROUPNAME, luaopen_COLLISION_GROUP},
  {NULL, NULL}
};


// GMod compatibility prelude. Defines the pieces of the Garry's Mod standard
// library that this engine lacks, so Workshop SWEPs/SENTs load. It is strictly
// defensive: every definition is gated on the name being absent, so it never
// overwrites a real implementation provided by the engine or base content.
// Hard-to-emulate features degrade to safe no-ops (a mounted SWEP still
// registers and appears even if e.g. its constraints or undo entries do
// nothing yet).
static const char *s_pGModCompatPrelude = R"GLUACOMPAT(
-- type helpers
if isfunction == nil then function isfunction(v) return type(v) == "function" end end
if istable    == nil then function istable(v)    return type(v) == "table" end end
if isstring   == nil then function isstring(v)   return type(v) == "string" end end
if isnumber   == nil then function isnumber(v)   return type(v) == "number" end end
if isbool     == nil then function isbool(v)     return type(v) == "boolean" end end
if isvector   == nil then function isvector(v)   return type(v) == "Vector" end end
if isangle    == nil then function isangle(v)    return type(v) == "Angle" end end
if ispanel    == nil then function ispanel(v)    return type(v) == "Panel" end end
if isentity   == nil then
  function isentity(v)
    local t = type(v)
    return t == "Entity" or t == "Player" or t == "NPC" or t == "Vehicle" or t == "Weapon" or t == "NextBot"
  end
end

-- math / misc helpers
math.Clamp = math.Clamp or function(v, lo, hi) if v < lo then return lo elseif v > hi then return hi end return v end
math.Round = math.Round or function(v, d) local m = 10 ^ (d or 0) return math.floor(v * m + 0.5) / m end
math.Rand  = math.Rand  or function(a, b) return a + (b - a) * math.random() end
math.AngleDifference = math.AngleDifference or function(a, b) local d = (a - b) % 360 if d > 180 then d = d - 360 end return d end
if Lerp == nil then function Lerp(t, a, b) return a + (b - a) * t end end

-- string helpers
string.Replace = string.Replace or function(s, find, rep)
  if find == nil or find == "" then return s end
  local pat = (string.gsub(find, "([%-%.%+%[%]%(%)%$%^%%%?%*])", "%%%1"))
  local sub = (string.gsub(rep, "%%", "%%%%"))
  return (string.gsub(s, pat, sub))
end
string.GetFileFromFilename = string.GetFileFromFilename or function(p) p = string.gsub(p, "\\", "/") return string.match(p, "[^/]*$") or p end
string.NiceName = string.NiceName or function(s) return (string.gsub(tostring(s), "[_%.]", " ")) end

-- table helpers
table.Count = table.Count or function(t) local n = 0 for _ in pairs(t) do n = n + 1 end return n end

-- language
if language == nil then
  language = {}
  local phrases = {}
  function language.Add(k, v) phrases[k] = v end
  function language.GetPhrase(k) return phrases[k] or k end
  function language.GetLanguages() return {} end
end

-- cvars
if cvars == nil then
  cvars = {}
  local callbacks = {}
  function cvars.AddChangeCallback(name, fn, id) callbacks[name] = callbacks[name] or {} callbacks[name][id or (#callbacks[name] + 1)] = fn end
  function cvars.RemoveChangeCallback(name, id) if callbacks[name] then callbacks[name][id] = nil end end
  function cvars.Number(n, d) local c = GetConVar(n) return c and c:GetFloat() or (d or 0) end
  function cvars.String(n, d) local c = GetConVar(n) return c and c:GetString() or (d or "") end
  function cvars.Bool(n, d) local c = GetConVar(n) return c and c:GetBool() or (d or false) end
end

-- GMod spells the angle constructor "Angle"; this engine registers it as
-- "QAngle". Alias it so Angle(p, y, r) works.
if Angle == nil and QAngle ~= nil then Angle = QAngle end

-- hook: addon autorun can run before the base content's hook module has
-- loaded (notably on the server), so hook.Add was nil and crashed. Provide a
-- minimal implementation now and register it in package.loaded so the base
-- module("hook") reuses this very table instead of replacing it.
if hook == nil and (package == nil or package.loaded == nil or package.loaded.hook == nil) then
  local H, hooks = {}, {}
  function H.Add(event, name, fn) if event and name and fn then hooks[event] = hooks[event] or {} hooks[event][name] = fn end end
  function H.Remove(event, name) if hooks[event] then hooks[event][name] = nil end end
  function H.GetTable() return hooks end
  function H.Run(event, ...)
    local l = hooks[event]
    if not l then return end
    for _, fn in pairs(l) do
      local ok, r = pcall(fn, ...)
      if ok and r ~= nil then return r end
    end
  end
  function H.Call(event, gm, ...) return H.Run(event, ...) end
  hook = H
  if package and package.loaded then package.loaded.hook = H end
end

-- timer: GMod's timer library does not exist on this engine, so addons that
-- schedule delayed work (the fizzler dissolves ragdolls via timer.Simple, many
-- SWEPs use timer.Create) errored on a nil global. Implement it here, driven by
-- __sbpp_RunTimers() which the engine calls once per server frame (GameFrame).
if timer == nil then
  local simple, named = {}, {}
  local T = {}
  local function now() return CurTime and CurTime() or 0 end
  function T.Simple( delay, fn )
    if type( fn ) ~= "function" then return end
    simple[#simple + 1] = { at = now() + ( tonumber( delay ) or 0 ), fn = fn }
  end
  function T.Create( name, delay, reps, fn )
    if type( fn ) ~= "function" then return end
    named[name] = { delay = tonumber( delay ) or 0, reps = tonumber( reps ) or 0,
                    fn = fn, at = now() + ( tonumber( delay ) or 0 ), n = 0, paused = false }
  end
  T.Adjust = function( name, delay, reps, fn )
    local t = named[name]; if not t then return false end
    t.delay = tonumber( delay ) or t.delay
    if reps ~= nil then t.reps = tonumber( reps ) or t.reps end
    if fn ~= nil then t.fn = fn end
    return true
  end
  function T.Remove( name ) named[name] = nil end
  T.Destroy = T.Remove
  function T.Exists( name ) return named[name] ~= nil end
  function T.Start( name ) local t = named[name]; if t then t.paused = false; t.at = now() + t.delay end return t ~= nil end
  function T.Stop( name ) local t = named[name]; if t then t.paused = true end return t ~= nil end
  T.Pause = T.Stop
  function T.UnPause( name ) local t = named[name]; if t then t.paused = false end return t ~= nil end
  function T.Toggle( name ) local t = named[name]; if t then t.paused = not t.paused end return t ~= nil end
  function T.TimeLeft( name ) local t = named[name]; if not t then return nil end return t.at - now() end
  function T.RepsLeft( name ) local t = named[name]; if not t then return nil end return t.reps == 0 and 0 or ( t.reps - t.n ) end
  timer = T

  function __sbpp_RunTimers()
    local t = now()
    local i = 1
    while i <= #simple do
      local e = simple[i]
      if t >= e.at then table.remove( simple, i ); pcall( e.fn ) else i = i + 1 end
    end
    for name, e in pairs( named ) do
      if not e.paused and t >= e.at then
        e.n = e.n + 1
        pcall( e.fn )
        if e.reps ~= 0 and e.n >= e.reps then named[name] = nil
        else e.at = t + e.delay end
      end
    end
  end
end

-- sound registration (playback still uses the engine directly)
if sound == nil then sound = {} end
sound.Add = sound.Add or function() end
sound.AddSoundOverrides = sound.AddSoundOverrides or function() end

-- sound channel / attenuation / soundlevel enums (GMod exposes these as
-- globals; addons put them in sound.Add tables at file scope, so they must
-- have real values at load time).
if CHAN_WEAPON == nil then
  CHAN_REPLACE = -1; CHAN_AUTO = 0; CHAN_WEAPON = 1; CHAN_VOICE = 2
  CHAN_ITEM = 3; CHAN_BODY = 4; CHAN_STREAM = 5; CHAN_STATIC = 6
  CHAN_VOICE2 = 7; CHAN_VOICE_BASE = 8; CHAN_USER_BASE = 136
end
if ATTN_NORM == nil then
  ATTN_NONE = 0; ATTN_NORM = 0.8; ATTN_IDLE = 2; ATTN_STATIC = 1.25
  ATTN_RICOCHET = 2.5; ATTN_GUNFIRE = 0.27
end
if SNDLVL_NORM == nil then
  SNDLVL_NONE = 0; SNDLVL_IDLE = 60; SNDLVL_STATIC = 66; SNDLVL_NORM = 75
  SNDLVL_TALKING = 80; SNDLVL_GUNFIRE = 140
  SNDLVL_60dB = 60; SNDLVL_65dB = 65; SNDLVL_70dB = 70; SNDLVL_75dB = 75
  SNDLVL_80dB = 80; SNDLVL_85dB = 85; SNDLVL_90dB = 90; SNDLVL_95dB = 95
  SNDLVL_100dB = 100; SNDLVL_105dB = 105; SNDLVL_120dB = 120; SNDLVL_130dB = 130
end

-- util network-string helpers. The engine only compiles these into the
-- shared util table under the SBPP build flag, and the server realm ends up
-- without them, so an addon's file-scope util.AddNetworkString crashes.
-- Provide a working in-Lua registry when they are absent.
if util == nil then util = {} end
if util.AddNetworkString == nil then
  local s2i, i2s, n = {}, {}, 0
  function util.AddNetworkString(s) if s2i[s] then return s2i[s] end n = n + 1 s2i[s] = n i2s[n] = s return n end
  function util.NetworkStringToID(s) return s2i[s] or 0 end
  function util.NetworkIDToString(i) return i2s[i] end
end

-- CreateClientConVar (GMod): create-or-find a convar with archive/userinfo
-- flags derived from its boolean args. Maps onto the engine's CreateConVar.
if CreateClientConVar == nil and CreateConVar ~= nil then
  function CreateClientConVar(name, default, shouldsave, userinfo, helptext, minv, maxv)
    local flags = 0
    if shouldsave ~= false then flags = flags + (FCVAR_ARCHIVE or 0) end
    if userinfo then flags = flags + (FCVAR_USERINFO or 0) end
    return CreateConVar(name, tostring(default), flags, helptext or "")
  end
end

-- GMod entity/player method shims. These map onto the engine's native methods
-- and are attached to the C++ metatables (reached via the registry), so they
-- survive base content redefining global tables. Only added when absent.
do
  local reg = ( debug and debug.getregistry ) and debug.getregistry() or nil
  local ENT = reg and reg.CBaseEntity or nil
  local PLY = reg and reg.CBasePlayer or nil

  if ENT then
    if ENT.GetPhysicsObject == nil and ENT.VPhysicsGetObject ~= nil then
      function ENT:GetPhysicsObject() return self:VPhysicsGetObject() end
    end
    if ENT.GetMassCenter == nil then
      function ENT:GetMassCenter()
        local p = self.GetPhysicsObject and self:GetPhysicsObject() or nil
        if p and p.GetMassCenterLocalSpace then return p:GetMassCenterLocalSpace() end
        return Vector( 0, 0, 0 )
      end
    end
    if ENT.OBBMaxs == nil then function ENT:OBBMaxs() return Vector( 16, 16, 16 ) end end
    if ENT.OBBMins == nil then function ENT:OBBMins() return Vector( -16, -16, -16 ) end end

    -- damage / health / dissolve helpers (GMod names -> engine methods)
    if ENT.Health == nil and ENT.GetHealth ~= nil then function ENT:Health() return self:GetHealth() end end
    if ENT.EntIndex == nil and ENT.entindex ~= nil then function ENT:EntIndex() return self:entindex() end end
    if ENT.TakeDamageInfo == nil and ENT.TakeDamage ~= nil then
      function ENT:TakeDamageInfo( info )
        -- GMod's Entity:TakeDamageInfo maps onto the engine's TakeDamage. Death
        -- by DMG_DISSOLVE (the fizzler/combine-ball "evaporate") is handled
        -- generally by CBaseCombatCharacter::Event_Killed, so a lethal
        -- DMG_DISSOLVE hit dissolves the NPC on its own -- no special-casing
        -- here, which keeps this correct for every addon, not just one.
        self:TakeDamage( info )
      end
    end
    if ENT.Dissolve == nil and effect ~= nil and effect.Dissolve ~= nil then
      -- GMod addons call ent:Dissolve(...); wrap the engine's effect.Dissolve.
      -- Use the electrical type for NPCs (kills + dissolves via a ragdoll) and
      -- the normal type for everything else. A real sprite so it is visible.
      function ENT:Dissolve()
        local t = ( self.IsNPC and self:IsNPC() ) and 1 or 0
        effect.Dissolve( self, "sprites/blueglow1.vmt", CurTime and CurTime() or 0, t )
      end
    end

    -- Networked vars: stored per-entity (per realm). This is enough for
    -- server-side addon logic; values are NOT synced across client/server on
    -- this engine, so client-only reads see defaults.
    if ENT.SetNWEntity == nil then
      local function nwset( self, k, v ) local t = self.__nw if t == nil then t = {} self.__nw = t end t[k] = v end
      local function nwget( self, k ) local t = self.__nw if t == nil then return nil end return t[k] end
      function ENT:SetNWEntity( k, v ) nwset( self, "e_" .. k, v ) end
      function ENT:GetNWEntity( k, fb ) local v = nwget( self, "e_" .. k ) if v == nil then if fb ~= nil then return fb end return NULL end return v end
      function ENT:SetNWBool( k, v ) nwset( self, "b_" .. k, v and true or false ) end
      function ENT:GetNWBool( k, fb ) local v = nwget( self, "b_" .. k ) if v == nil then return fb or false end return v end
      function ENT:SetNWInt( k, v ) nwset( self, "i_" .. k, v ) end
      function ENT:GetNWInt( k, fb ) local v = nwget( self, "i_" .. k ) if v == nil then return fb or 0 end return v end
      function ENT:SetNWFloat( k, v ) nwset( self, "f_" .. k, v ) end
      function ENT:GetNWFloat( k, fb ) local v = nwget( self, "f_" .. k ) if v == nil then return fb or 0 end return v end
      function ENT:SetNWString( k, v ) nwset( self, "s_" .. k, tostring( v ) ) end
      function ENT:GetNWString( k, fb ) local v = nwget( self, "s_" .. k ) if v == nil then return fb or "" end return v end
      ENT.SetNW2Entity = ENT.SetNWEntity; ENT.GetNW2Entity = ENT.GetNWEntity
      ENT.SetNW2Bool   = ENT.SetNWBool;   ENT.GetNW2Bool   = ENT.GetNWBool
      ENT.SetNW2Int    = ENT.SetNWInt;    ENT.GetNW2Int    = ENT.GetNWInt
    end
  end

  if PLY then
    if PLY.EyePos == nil and PLY.EyePosition ~= nil then function PLY:EyePos() return self:EyePosition() end end
    if PLY.GetAimVector == nil then
      function PLY:GetAimVector()
        local f = Vector( 0, 0, 0 )
        if self.EyeVectors then self:EyeVectors( f ) end
        return f
      end
    end
    if PLY.GetEyeTrace == nil then
      function PLY:GetEyeTrace()
        local s = self:EyePosition()
        return util.TraceLine( { start = s, endpos = s + self:GetAimVector() * 32768, filter = self } )
      end
    end
    if PLY.Alive == nil and PLY.IsAlive ~= nil then function PLY:Alive() return self:IsAlive() end end
    if PLY.SelectWeapon == nil then
      function PLY:SelectWeapon( cls )
        if self.Weapon_Switch and self.GetWeapon then
          local w = self:GetWeapon( cls )
          if w then self:Weapon_Switch( w ) end
        end
      end
    end
  end

  -- weapon methods: SetHoldType drives player hold animations, which this
  -- engine does not expose; store it as a no-op so SWEP:Initialize completes.
  local WEP = reg and reg.CBaseCombatWeapon or nil
  if WEP then
    if WEP.SetHoldType == nil then function WEP:SetHoldType( t ) self.__holdtype = t end end
    if WEP.GetHoldType == nil then function WEP:GetHoldType() return self.__holdtype or "normal" end end
    -- fire-rate / effect helpers this engine doesn't expose; safe no-ops so
    -- SWEP:PrimaryAttack bodies run (rate limiting just won't apply).
    if WEP.SetNextPrimaryFire == nil then function WEP:SetNextPrimaryFire() end end
    if WEP.SetNextSecondaryFire == nil then function WEP:SetNextSecondaryFire() end end
    if WEP.ShootEffects == nil then function WEP:ShootEffects() end end
    if WEP.SendWeaponAnim == nil then function WEP:SendWeaponAnim() end end
  end

  -- physics object methods (GMod names -> engine names). Native already has
  -- ApplyForceCenter/ApplyForceOffset/ApplyTorqueCenter/GetMass/GetVelocity/
  -- GetVelocityAtPoint/IsMoveable/LocalToWorld/WorldToLocal/Wake/EnableMotion.
  local PHYS = reg and reg.IPhysicsObject or nil
  if PHYS then
    if PHYS.GetMassCenter == nil and PHYS.GetMassCenterLocalSpace ~= nil then
      function PHYS:GetMassCenter() return self:GetMassCenterLocalSpace() end
    end
    if PHYS.GetPos == nil and PHYS.GetPosition ~= nil then function PHYS:GetPos() return self:GetPosition() end end
    if PHYS.SetPos == nil and PHYS.SetPosition ~= nil then function PHYS:SetPos( p, a ) return self:SetPosition( p, a ) end end
  end

  -- ==== GMod name -> native Source method aliases ====
  -- Most engine methods exist under their Source (C++) names; GMod addons use
  -- different names. Alias the common ones (only when the GMod name is absent
  -- so base content / natives always win).
  local function alias( meta, gmod, native )
    if meta and meta[gmod] == nil and meta[native] ~= nil then meta[gmod] = meta[native] end
  end
  if ENT then
    alias( ENT, "GetPos", "GetAbsOrigin" );        alias( ENT, "SetPos", "SetAbsOrigin" )
    alias( ENT, "GetAngles", "GetAbsAngles" );     alias( ENT, "SetAngles", "SetAbsAngles" )
    alias( ENT, "GetClass", "GetClassname" )
    alias( ENT, "GetModel", "GetModelName" )
    alias( ENT, "GetVelocity", "GetAbsVelocity" ); alias( ENT, "SetVelocity", "SetAbsVelocity" )
    alias( ENT, "GetOwner", "GetOwnerEntity" );    alias( ENT, "SetOwner", "SetOwnerEntity" )
    alias( ENT, "GetParent", "GetMoveParent" )
    alias( ENT, "WorldToLocal", "WorldToEntitySpace" ); alias( ENT, "LocalToWorld", "EntityToWorldSpace" )
    alias( ENT, "SetKeyValue", "KeyValue" )
    alias( ENT, "Input", "AcceptInput" )
    -- type/boolean helpers already native: IsNPC, IsPlayer, IsWeapon, IsWorld,
    -- IsSolid, IsMarkedForDeletion, IsInWorld, IsDormant, GetName(entity).
    if ENT.GetForward == nil and ENT.GetVectors ~= nil then
      function ENT:GetForward() local f,r,u = Vector(0,0,0), Vector(0,0,0), Vector(0,0,0) self:GetVectors( f, r, u ) return f end
      function ENT:GetRight()   local f,r,u = Vector(0,0,0), Vector(0,0,0), Vector(0,0,0) self:GetVectors( f, r, u ) return r end
      function ENT:GetUp()      local f,r,u = Vector(0,0,0), Vector(0,0,0), Vector(0,0,0) self:GetVectors( f, r, u ) return u end
    end
    if ENT.SetColor == nil and ENT.SetRenderColor ~= nil then
      function ENT:SetColor( c )
        if c == nil then return end
        self:SetRenderColor( c.r or 255, c.g or 255, c.b or 255 )
        if self.SetRenderColorA and c.a then self:SetRenderColorA( c.a ) end
      end
    end
    if ENT.SetNoDraw == nil and ENT.AddEffects ~= nil and ENT.RemoveEffects ~= nil then
      function ENT:SetNoDraw( b ) if b then self:AddEffects( 32 ) else self:RemoveEffects( 32 ) end end -- EF_NODRAW
    end
    if ENT.GetTable == nil then function ENT:GetTable() self.__t = self.__t or {} return self.__t end end
  end
  if PLY then
    alias( PLY, "Nick", "GetPlayerName" ); alias( PLY, "Name", "GetPlayerName" )
    alias( PLY, "SteamID", "GetNetworkIDString" ); alias( PLY, "SteamID64", "GetNetworkIDString" )
    alias( PLY, "UserID", "GetUserID" )
    alias( PLY, "Team", "GetTeamNumber" )
    alias( PLY, "Give", "GiveNamedItem" )
    alias( PLY, "StripWeapon", "RemovePlayerItem" )
    alias( PLY, "GetEyeAngles", "EyeAngles" )
    if PLY.KeyDown == nil then
      function PLY:KeyDown( b )
        local n = self.m_nButtons or 0
        if bit and bit.band then return bit.band( n, b ) ~= 0 end
        return ( n % ( b * 2 ) ) >= b
      end
    end
    if PLY.IsAdmin == nil then function PLY:IsAdmin() return true end end       -- listen-server host
    if PLY.IsSuperAdmin == nil then function PLY:IsSuperAdmin() return true end end
  end
  if WEP then
    alias( WEP, "GetPrimaryClip", "Clip1" ); alias( WEP, "GetSecondaryClip", "Clip2" )
  end
end

-- movetype enums (Source MoveType_t values)
if MOVETYPE_VPHYSICS == nil then
  MOVETYPE_NONE = 0; MOVETYPE_ISOMETRIC = 1; MOVETYPE_WALK = 2; MOVETYPE_STEP = 3
  MOVETYPE_FLY = 4; MOVETYPE_FLYGRAVITY = 5; MOVETYPE_VPHYSICS = 6; MOVETYPE_PUSH = 7
  MOVETYPE_NOCLIP = 8; MOVETYPE_LADDER = 9; MOVETYPE_OBSERVER = 10; MOVETYPE_CUSTOM = 11
end

-- DamageInfo (GMod) is the engine's CTakeDamageInfo constructor.
if DamageInfo == nil and CTakeDamageInfo ~= nil then DamageInfo = CTakeDamageInfo end

-- DMG_* damage-type bits (Source shareddefs). DMG_DISSOLVE makes NPCs
-- auto-dissolve on death when applied via TakeDamage.
if DMG_DISSOLVE == nil then
  DMG_GENERIC = 0; DMG_CRUSH = 1; DMG_BULLET = 2; DMG_SLASH = 4; DMG_BURN = 8
  DMG_VEHICLE = 16; DMG_FALL = 32; DMG_BLAST = 64; DMG_CLUB = 128; DMG_SHOCK = 256
  DMG_SONIC = 512; DMG_ENERGYBEAM = 1024; DMG_NEVERGIB = 4096; DMG_ALWAYSGIB = 8192
  DMG_DROWN = 16384; DMG_PARALYZE = 32768; DMG_NERVEGAS = 65536; DMG_POISON = 131072
  DMG_RADIATION = 262144; DMG_ACID = 1048576; DMG_SLOWBURN = 2097152
  DMG_REMOVENORAGDOLL = 4194304; DMG_PHYSGUN = 8388608; DMG_PLASMA = 16777216
  DMG_AIRBOAT = 33554432; DMG_DISSOLVE = 67108864; DMG_BLAST_SURFACE = 134217728
  DMG_DIRECT = 268435456; DMG_BUCKSHOT = 536870912
end

-- ents library (entity creation / lookup), backed by native util.* helpers
if ents == nil then ents = {} end
if ents.Create == nil and CreateEntityByName ~= nil then ents.Create = function( cls ) return CreateEntityByName( cls ) end end
if ents.FindInSphere == nil then
  ents.FindInSphere = function( pos, radius )
    if util and util.EntitiesInSphere then return util.EntitiesInSphere( pos, radius ) end
    return {}
  end
end
if ents.FindInBox == nil and util ~= nil and util.EntitiesInBox ~= nil then
  ents.FindInBox = function( mins, maxs ) return util.EntitiesInBox( mins, maxs ) end
end
if ents.GetByIndex == nil and util ~= nil and util.EntityByIndex ~= nil then
  ents.GetByIndex = function( i ) return util.EntityByIndex( i ) end
end
if ents.FindByClass == nil then
  ents.FindByClass = function( cls )
    local out, i = {}, 1
    if not ents.GetByIndex then return out end
    for idx = 1, 8192 do
      local e = ents.GetByIndex( idx )
      if e and IsValid( e ) and e.GetClass and e:GetClass() == cls then out[i] = e i = i + 1 end
    end
    return out
  end
end

-- input button bits (Source IN_*)
if IN_ATTACK == nil then
  IN_ATTACK = 1; IN_JUMP = 2; IN_DUCK = 4; IN_FORWARD = 8; IN_BACK = 16
  IN_USE = 32; IN_CANCEL = 64; IN_LEFT = 128; IN_RIGHT = 256; IN_MOVELEFT = 512
  IN_MOVERIGHT = 1024; IN_ATTACK2 = 2048; IN_RUN = 4096; IN_RELOAD = 8192
  IN_ALT1 = 16384; IN_ALT2 = 32768; IN_SCORE = 65536; IN_SPEED = 131072
  IN_WALK = 262144; IN_ZOOM = 524288; IN_WEAPON1 = 1048576; IN_WEAPON2 = 2097152
  IN_BULLRUSH = 4194304; IN_GRENADE1 = 8388608; IN_GRENADE2 = 16777216
end

-- effect flags (EF_*), collision groups, solid types, move-collide
if EF_NODRAW == nil then
  EF_BONEMERGE = 1; EF_BRIGHTLIGHT = 2; EF_DIMLIGHT = 4; EF_NOINTERP = 8
  EF_NOSHADOW = 16; EF_NODRAW = 32; EF_NORECEIVESHADOW = 64; EF_ITEM_BLINK = 256
  EF_PARENT_ANIMATES = 512
end
if COLLISION_GROUP_NONE == nil then
  COLLISION_GROUP_NONE = 0; COLLISION_GROUP_DEBRIS = 1; COLLISION_GROUP_INTERACTIVE = 3
  COLLISION_GROUP_PLAYER = 5; COLLISION_GROUP_WEAPON = 12; COLLISION_GROUP_VEHICLE = 6
  COLLISION_GROUP_PLAYER_MOVEMENT = 8; COLLISION_GROUP_WORLD = 15
end
if SOLID_NONE == nil then
  SOLID_NONE = 0; SOLID_BSP = 1; SOLID_BBOX = 2; SOLID_OBB = 3; SOLID_OBB_YAW = 4
  SOLID_CUSTOM = 5; SOLID_VPHYSICS = 6
end
if MASK_SOLID == nil then
  MASK_ALL = 4294967295; MASK_SOLID = 33570827; MASK_SHOT = 1174421507
  MASK_PLAYERSOLID = 33636363; MASK_NPCSOLID = 33701899; MASK_OPAQUE = 16513
  MASK_SOLID_BRUSHONLY = 16395; MASK_SHOT_HULL = 100679691
end

-- player library (enumeration)
if player == nil then player = {} end
player.GetAll = player.GetAll or function() return ( util and util.GetAllPlayers and util.GetAllPlayers() ) or {} end
player.GetHumans = player.GetHumans or player.GetAll
player.GetCount = player.GetCount or function() return #player.GetAll() end

-- misc globals used by addons
if IsFirstTimePredicted == nil then function IsFirstTimePredicted() return true end end

-- ===== GMod stdlib: string / table / math (pure Lua, real implementations) =====
string.Trim = string.Trim or function( s, char )
  char = char and ( "%" .. char ) or "%s"
  return ( string.gsub( s, "^" .. char .. "*(.-)" .. char .. "*$", "%1" ) )
end
string.TrimLeft  = string.TrimLeft  or function( s, c ) c = c and ( "%" .. c ) or "%s" return ( string.gsub( s, "^" .. c .. "*", "" ) ) end
string.TrimRight = string.TrimRight or function( s, c ) c = c and ( "%" .. c ) or "%s" return ( string.gsub( s, c .. "*$", "" ) ) end
string.Left  = string.Left  or function( s, n ) return string.sub( s, 1, n ) end
string.Right = string.Right or function( s, n ) return string.sub( s, -n ) end
string.StartWith  = string.StartWith  or function( s, p ) return string.sub( s, 1, string.len( p ) ) == p end
string.StartsWith = string.StartsWith or string.StartWith
string.EndsWith   = string.EndsWith   or function( s, p ) return p == "" or string.sub( s, -string.len( p ) ) == p end
string.Explode = string.Explode or function( sep, s, usePatterns )
  local out, idx, plain = {}, 1, not usePatterns
  local pos = 1
  while true do
    local b, e = string.find( s, sep, pos, plain )
    if not b then out[idx] = string.sub( s, pos ) break end
    out[idx] = string.sub( s, pos, b - 1 ); idx = idx + 1; pos = e + 1
  end
  return out
end
string.Split  = string.Split  or function( s, sep ) return string.Explode( sep, s ) end
string.Implode = string.Implode or function( sep, tbl ) return table.concat( tbl, sep ) end
string.ToTable = string.ToTable or function( s ) local t = {} for i = 1, string.len( s ) do t[i] = string.sub( s, i, i ) end return t end
string.PatternSafe = string.PatternSafe or function( s ) return ( string.gsub( s, "([%-%.%+%[%]%(%)%$%^%%%?%*])", "%%%1" ) ) end
Format = Format or string.format

table.HasValue = table.HasValue or function( t, val ) for _, v in pairs( t ) do if v == val then return true end end return false end
table.KeyFromValue = table.KeyFromValue or function( t, val ) for k, v in pairs( t ) do if v == val then return k end end end
table.Add = table.Add or function( dst, src ) local n = #dst for _, v in ipairs( src ) do n = n + 1 dst[n] = v end return dst end
table.Merge = table.Merge or function( dst, src ) for k, v in pairs( src ) do if type( v ) == "table" and type( dst[k] ) == "table" then table.Merge( dst[k], v ) else dst[k] = v end end return dst end
table.Copy = table.Copy or function( t, lookup )
  if t == nil then return nil end
  local out = {}
  lookup = lookup or {}
  lookup[t] = out
  for k, v in pairs( t ) do
    if type( v ) == "table" then out[k] = lookup[v] or table.Copy( v, lookup ) else out[k] = v end
  end
  return out
end
table.GetKeys = table.GetKeys or function( t ) local keys, i = {}, 1 for k in pairs( t ) do keys[i] = k i = i + 1 end return keys end
table.Random = table.Random or function( t ) local keys = table.GetKeys( t ) local k = keys[ math.random( 1, #keys ) ] return t[k], k end
table.RemoveByValue = table.RemoveByValue or function( t, val ) local k = table.KeyFromValue( t, val ) if k == nil then return false end if type( k ) == "number" then table.remove( t, k ) else t[k] = nil end return k end
table.ForceInsert = table.ForceInsert or function( t, v ) if t == nil then t = {} end t[#t + 1] = v return t end
table.Reverse = table.Reverse or function( t ) local out, n = {}, #t for i = n, 1, -1 do out[n - i + 1] = t[i] end return out end
table.IsSequential = table.IsSequential or function( t ) local i = 0 for _ in pairs( t ) do i = i + 1 if t[i] == nil then return false end end return true end

math.Distance = math.Distance or function( x1, y1, x2, y2 ) local dx, dy = x2 - x1, y2 - y1 return math.sqrt( dx * dx + dy * dy ) end
math.Approach = math.Approach or function( cur, target, inc ) inc = math.abs( inc ) if cur < target then return math.min( cur + inc, target ) elseif cur > target then return math.max( cur - inc, target ) end return target end
math.NormalizeAngle = math.NormalizeAngle or function( a ) a = a % 360 if a > 180 then a = a - 360 end return a end
math.Remap = math.Remap or function( v, inMin, inMax, outMin, outMax ) return outMin + ( v - inMin ) / ( inMax - inMin ) * ( outMax - outMin ) end
math.TimeFraction = math.TimeFraction or function( s, e, c ) return ( c - s ) / ( e - s ) end
math.Truncate = math.Truncate or function( v, d ) local m = 10 ^ ( d or 0 ) return ( v < 0 and math.ceil or math.floor )( v * m ) / m end

-- ===== common global helpers =====
if tobool == nil then
  function tobool( v )
    if v == nil or v == false or v == 0 or v == "0" or v == "false" then return false end
    return true
  end
end
if IsColor == nil then function IsColor( v ) return type( v ) == "table" and v.r ~= nil and v.g ~= nil and v.b ~= nil end end
if VectorRand == nil then function VectorRand( mn, mx ) mn = mn or -1 mx = mx or 1 return Vector( math.Rand( mn, mx ), math.Rand( mn, mx ), math.Rand( mn, mx ) ) end end
if AngleRand == nil then function AngleRand() return Angle( math.Rand( -90, 90 ), math.Rand( -180, 180 ), math.Rand( -180, 180 ) ) end end
if SortedPairs == nil then
  function SortedPairs( t, desc )
    local keys = {}
    for k in pairs( t ) do keys[#keys + 1] = k end
    table.sort( keys, function( a, b ) if desc then return a > b else return a < b end end )
    local i = 0
    return function() i = i + 1 local k = keys[i] if k ~= nil then return k, t[k] end end
  end
end
if SafeRemoveEntity == nil then function SafeRemoveEntity( e ) if e and IsValid( e ) and e.Remove then e:Remove() end end end
if SafeRemoveEntityDelayed == nil and timer ~= nil then
  function SafeRemoveEntityDelayed( e, d ) if timer and timer.Simple then timer.Simple( d or 0, function() SafeRemoveEntity( e ) end ) end end
end
if table.IsEmpty == nil then function table.IsEmpty( t ) return next( t ) == nil end end
if table.Empty == nil then function table.Empty( t ) for k in pairs( t ) do t[k] = nil end end end

-- server/shared prop-management shims (no-op where unsupported)
if cleanup == nil then
  cleanup = {}
  function cleanup.Add() end
  function cleanup.Register() end
  function cleanup.GetTable() return {} end
  function cleanup.ReplaceEntities() end
end
if undo == nil then
  undo = {}
  function undo.Create() end
  function undo.AddEntity() end
  function undo.AddFunction() end
  function undo.SetPlayer() end
  function undo.SetCustomUndoText() end
  function undo.Finish() end
end
if constraint == nil then
  constraint = {}
  local function nilfn() return nil end
  constraint.Weld = nilfn
  constraint.NoCollide = nilfn
  constraint.Rope = nilfn
  constraint.Axis = nilfn
  constraint.Ballsocket = nilfn
  constraint.Elastic = nilfn
  constraint.Keepupright = nilfn
  function constraint.RemoveAll() end
  function constraint.RemoveConstraints() end
  function constraint.FindConstraint() return nil end
  function constraint.HasConstraints() return false end
end

-- spawnmenu tool-menu registration (the spawnmenu table itself is native)
if spawnmenu == nil then spawnmenu = {} end
spawnmenu.AddToolMenuOption = spawnmenu.AddToolMenuOption or function() end
spawnmenu.AddToolCategory = spawnmenu.AddToolCategory or function() end

-- client-only rendering / UI shims
if CLIENT then
  -- Material(name): GMod returns an IMaterial. Map onto the engine's native
  -- materials.FindMaterial so it yields a real, drawable material.
  if Material == nil then
    if materials ~= nil and materials.FindMaterial ~= nil then
      function Material(name) return materials.FindMaterial(name, nil, false) end
    else
      function Material() return setmetatable({}, { __index = function() return function() return 0 end end }) end
    end
  end
  if killicon == nil then
    killicon = {}
    function killicon.Add() end
    function killicon.AddFont() end
    function killicon.AddAlias() end
    function killicon.Exists() return false end
    function killicon.Draw() end
    function killicon.GetSize() return 0, 0 end
  end
  if surface ~= nil then
    surface.CreateFont = surface.CreateFont or function() end
    surface.GetTextureID = surface.GetTextureID or function() return -1 end
  end
  if cam == nil then
    cam = {}
    function cam.Start3D() end
    function cam.End3D() end
    function cam.Start2D() end
    function cam.End2D() end
  end
  if markup == nil then
    markup = {}
    function markup.Parse(text)
      local o = { text = tostring(text or "") }
      function o:Draw() end
      function o:GetWidth() return 0 end
      function o:GetHeight() return 0 end
      function o:Size() return 0, 0 end
      return o
    end
  end
  if Derma_DrawBackgroundBlur == nil then function Derma_DrawBackgroundBlur() end end
end

-- ===================================================================
-- GMod library layer (batch 2). Everything is gated on the name being
-- absent so engine natives / base content always win. Backed by real
-- engine natives (filesystem, weapon/entity registries, surface) where
-- they exist; pure Lua where self-contained.
-- ===================================================================

-- AccessorFunc force-type enums. GMod's real values are STRING=1, NUMBER=2,
-- BOOL=3 (garrysmod util.lua); addons pass these by name, so they must exist.
if FORCE_STRING == nil then FORCE_STRING = 1; FORCE_NUMBER = 2; FORCE_BOOL = 3 end

-- AccessorFunc: generate Get<Name>/Set<Name> on a table. Ubiquitous in
-- ENT/SWEP/tool definitions; its absence crashed addons at file scope.
-- Coercion mapping matches GMod exactly (1=tostring, 2=tonumber, 3=tobool);
-- an earlier version had these inverted, which silently corrupted any addon
-- that used a forced accessor.
if AccessorFunc == nil then
  function AccessorFunc( tab, key, name, force )
    if tab == nil then return end
    tab[ "Get" .. name ] = function( self ) return self[ key ] end
    tab[ "Set" .. name ] = function( self, v )
      if force == FORCE_STRING then v = tostring( v )
      elseif force == FORCE_NUMBER then v = tonumber( v )
      elseif force == FORCE_BOOL then v = tobool( v ) end
      self[ key ] = v
    end
  end
end

-- list: a global registry keyed by list name. GMod addons register
-- NPCs / weapons / tools / etc. via list.Set and menus read them back.
if list == nil then
  local lists = {}
  list = {}
  function list.GetForEdit( name ) local t = lists[ name ] if t == nil then t = {} lists[ name ] = t end return t end
  function list.GetTable( name ) return lists[ name ] or {} end
  function list.Get( name ) return table.Copy( lists[ name ] or {} ) end
  function list.Set( name, key, val ) list.GetForEdit( name )[ key ] = val end
  function list.Add( name, val ) local t = list.GetForEdit( name ) t[ #t + 1 ] = val end
  function list.HasEntry( name, key ) local t = lists[ name ] return t ~= nil and t[ key ] ~= nil end
end

-- baseclass + DEFINE_BASECLASS: resolve a base SWEP/ENT table by name,
-- backed by the engine's weapon.get / entity.get registries.
if baseclass == nil then
  local cache = {}
  baseclass = {}
  function baseclass.Set( name, tab ) cache[ name ] = tab end
  function baseclass.Get( name )
    if cache[ name ] then return cache[ name ] end
    local t
    if weapon and weapon.get then t = weapon.get( name ) end
    if t == nil and entity and entity.get then t = entity.get( name ) end
    if t == nil then t = {} end
    cache[ name ] = t
    return t
  end
end
if DEFINE_BASECLASS == nil then function DEFINE_BASECLASS( name ) return baseclass.Get( name ) end end

-- weapons / scripted_ents: some addons register directly instead of
-- relying on the SWEP/ENT auto-loader. Delegate to the engine registries.
if weapons == nil and weapon ~= nil then
  weapons = {}
  function weapons.Register( t, class ) if weapon.register then weapon.register( t, class ) end if baseclass then baseclass.Set( class, t ) end return t end
  function weapons.GetStored( class ) return weapon.get and weapon.get( class ) or nil end
  function weapons.Get( class ) local t = weapons.GetStored( class ) return t and table.Copy( t ) or nil end
  function weapons.GetList() return {} end
  function weapons.IsBasedOn() return false end
end
if scripted_ents == nil and entity ~= nil then
  scripted_ents = {}
  function scripted_ents.Register( t, class ) if entity.register then entity.register( t, class ) end if baseclass then baseclass.Set( class, t ) end return t end
  function scripted_ents.GetStored( class ) return entity.get and entity.get( class ) or nil end
  function scripted_ents.Get( class ) local t = scripted_ents.GetStored( class ) return t and table.Copy( t ) or nil end
  function scripted_ents.GetList() return {} end
  function scripted_ents.GetType() return "anim" end
  function scripted_ents.IsBasedOn() return false end
end

-- string path helper used by the file library and many addons.
string.GetPathFromFilename = string.GetPathFromFilename or function( p )
  p = string.gsub( p, "\\", "/" )
  return string.match( p, "^(.*/)[^/]*$" ) or ""
end

-- file library, backed by the native filesystem lib. GMod's default path
-- is "DATA" (garrysmod/data); mirror that as data/<name> under "MOD".
if file == nil and filesystem ~= nil then
  file = {}
  local function resolve( name, path )
    if path == nil or path == "DATA" then return "data/" .. name, "MOD" end
    return name, path
  end
  function file.Exists( name, path ) local r, p = resolve( name, path ) return filesystem.FileExists( r, p ) end
  function file.IsDir( name, path ) local r, p = resolve( name, path ) return filesystem.IsDirectory( r, p ) end
  function file.Size( name, path ) local r, p = resolve( name, path ) return filesystem.Size( r, p ) end
  function file.Delete( name ) filesystem.RemoveFile( "data/" .. name, "MOD" ) end
  function file.CreateDir( name ) filesystem.CreateDirHierarchy( "data/" .. name, "MOD" ) end
  function file.Read( name, path )
    local r, p = resolve( name, path )
    local sz = filesystem.Size( r, p )
    if not sz or sz < 0 then return nil end
    local f = filesystem.Open( r, "rb", p )
    if not f then return nil end
    local _, data = filesystem.Read( sz, f )
    filesystem.Close( f )
    return data
  end
  function file.Write( name, content )
    local dir = string.GetPathFromFilename( "data/" .. name )
    if dir ~= "" then filesystem.CreateDirHierarchy( dir, "MOD" ) end
    local f = filesystem.Open( "data/" .. name, "wb", "MOD" )
    if not f then return false end
    filesystem.Write( content, f )
    filesystem.Close( f )
    return true
  end
  function file.Append( name, content )
    local f = filesystem.Open( "data/" .. name, "ab", "MOD" )
    if not f then return false end
    filesystem.Write( content, f )
    filesystem.Close( f )
    return true
  end
  function file.Find( pattern, path )
    local r, p = resolve( pattern, path )
    local all = filesystem.Find( r, p ) or {}
    local base = string.GetPathFromFilename( r )
    local files, dirs = {}, {}
    for _, n in ipairs( all ) do
      if filesystem.IsDirectory( base .. n, p ) then dirs[ #dirs + 1 ] = n else files[ #files + 1 ] = n end
    end
    return files, dirs
  end
  function file.Time() return 0 end
end

-- util JSON + table helpers (pure Lua). Config-driven addons rely on these.
if util == nil then util = {} end
if util.TableToJSON == nil then
  local function encode( v, pretty, indent )
    local t = type( v )
    if t == "nil" then return "null"
    elseif t == "boolean" then return v and "true" or "false"
    elseif t == "number" then return tostring( v )
    elseif t == "string" then
      return '"' .. v:gsub( '[%z\1-\31\\"]', function( c )
        local m = { ['"'] = '\\"', ['\\'] = '\\\\', ['\n'] = '\\n', ['\r'] = '\\r', ['\t'] = '\\t' }
        return m[ c ] or string.format( '\\u%04x', string.byte( c ) )
      end ) .. '"'
    elseif t == "table" then
      local isArr, n = true, 0
      for k in pairs( v ) do n = n + 1 if type( k ) ~= "number" then isArr = false end end
      if isArr and n > 0 then
        local parts = {}
        for i = 1, n do parts[ i ] = encode( v[ i ], pretty ) end
        return "[" .. table.concat( parts, "," ) .. "]"
      else
        local parts, i = {}, 0
        for k, val in pairs( v ) do i = i + 1 parts[ i ] = encode( tostring( k ) ) .. ":" .. encode( val, pretty ) end
        return "{" .. table.concat( parts, "," ) .. "}"
      end
    end
    return "null"
  end
  function util.TableToJSON( t, pretty ) return encode( t, pretty ) end
end
if util.JSONToTable == nil then
  function util.JSONToTable( s )
    if type( s ) ~= "string" then return nil end
    local pos = 1
    local decodeValue
    local function skip() pos = ( s:find( "[^ \t\r\n]", pos ) ) or ( #s + 1 ) end
    local function decodeString()
      pos = pos + 1
      local buf = {}
      while pos <= #s do
        local c = s:sub( pos, pos )
        if c == '"' then pos = pos + 1 return table.concat( buf )
        elseif c == '\\' then
          local n = s:sub( pos + 1, pos + 1 )
          local m = { ['"'] = '"', ['\\'] = '\\', ['/'] = '/', n = '\n', r = '\r', t = '\t', b = '\b', f = '\f' }
          if n == 'u' then
            local hex = s:sub( pos + 2, pos + 5 )
            buf[ #buf + 1 ] = string.char( tonumber( hex, 16 ) % 256 )
            pos = pos + 6
          else buf[ #buf + 1 ] = m[ n ] or n pos = pos + 2 end
        else buf[ #buf + 1 ] = c pos = pos + 1 end
      end
      return table.concat( buf )
    end
    decodeValue = function()
      skip()
      local c = s:sub( pos, pos )
      if c == '"' then return decodeString()
      elseif c == '{' then
        local obj = {} pos = pos + 1 skip()
        if s:sub( pos, pos ) == '}' then pos = pos + 1 return obj end
        while true do
          skip()
          local key = decodeString()
          skip() pos = pos + 1 -- colon
          obj[ key ] = decodeValue()
          skip()
          local d = s:sub( pos, pos ) pos = pos + 1
          if d == '}' then break end
        end
        return obj
      elseif c == '[' then
        local arr, i = {}, 0 pos = pos + 1 skip()
        if s:sub( pos, pos ) == ']' then pos = pos + 1 return arr end
        while true do
          i = i + 1 arr[ i ] = decodeValue()
          skip()
          local d = s:sub( pos, pos ) pos = pos + 1
          if d == ']' then break end
        end
        return arr
      elseif c == 't' then pos = pos + 4 return true
      elseif c == 'f' then pos = pos + 5 return false
      elseif c == 'n' then pos = pos + 4 return nil
      else
        local num = s:match( "^%-?%d+%.?%d*[eE]?[%+%-]?%d*", pos )
        if num then pos = pos + #num return tonumber( num ) end
      end
    end
    local ok, res = pcall( decodeValue )
    if ok then return res end
    return nil
  end
end

-- more sorted-iteration / table helpers commonly used at file scope
if SortedPairsByValue == nil then
  function SortedPairsByValue( t, desc )
    local keys = {}
    for k in pairs( t ) do keys[ #keys + 1 ] = k end
    table.sort( keys, function( a, b ) if desc then return t[ a ] > t[ b ] else return t[ a ] < t[ b ] end end )
    local i = 0
    return function() i = i + 1 local k = keys[ i ] if k ~= nil then return k, t[ k ] end end
  end
end
if SortedPairsByMemberValue == nil then
  function SortedPairsByMemberValue( t, member, desc )
    local keys = {}
    for k in pairs( t ) do keys[ #keys + 1 ] = k end
    table.sort( keys, function( a, b )
      local va, vb = t[ a ][ member ], t[ b ][ member ]
      if desc then return va > vb else return va < vb end
    end )
    local i = 0
    return function() i = i + 1 local k = keys[ i ] if k ~= nil then return k, t[ k ] end end
  end
end
if RandomPairs == nil then
  function RandomPairs( t )
    local keys = {}
    for k in pairs( t ) do keys[ #keys + 1 ] = k end
    for i = #keys, 2, -1 do local j = math.random( i ) keys[ i ], keys[ j ] = keys[ j ], keys[ i ] end
    local i = 0
    return function() i = i + 1 local k = keys[ i ] if k ~= nil then return k, t[ k ] end end
  end
end
table.ForEach = table.ForEach or function( t, fn ) for k, v in pairs( t ) do fn( k, v ) end end
table.Inherit = table.Inherit or function( t, base ) for k, v in pairs( base ) do if t[ k ] == nil then t[ k ] = v end end t.BaseClass = base return t end
table.GetFirstKey = table.GetFirstKey or function( t ) local k = next( t ) return k end
table.GetFirstValue = table.GetFirstValue or function( t ) local k, v = next( t ) return v end
table.SortByMember = table.SortByMember or function( t, member, asc )
  table.sort( t, function( a, b ) if asc == false then return a[ member ] > b[ member ] else return a[ member ] < b[ member ] end end )
  return t
end
string.Comma = string.Comma or function( n )
  n = tostring( n )
  local out = n:reverse():gsub( "(%d%d%d)", "%1," ):reverse()
  return ( out:gsub( "^,", "" ) )
end
string.FormatTime = string.FormatTime or function( s )
  s = math.floor( s or 0 )
  return string.format( "%02i:%02i", math.floor( s / 60 ), s % 60 )
end
string.ToMinutesSeconds = string.ToMinutesSeconds or string.FormatTime

-- safety no-op libraries for subsystems this engine lacks. Gated so they
-- never clobber a real implementation; they only stop addons crashing at
-- load when they touch these at file scope.
if resource == nil then
  resource = {}
  function resource.AddFile() end
  function resource.AddSingleFile() end
  function resource.AddWorkshop() end
end
if duplicator == nil then
  duplicator = {}
  function duplicator.RegisterEntityClass() end
  function duplicator.RegisterEntityModifier() end
  function duplicator.RegisterBoneModifier() end
  function duplicator.Allow() end
  function duplicator.Copy() return {} end
  function duplicator.Paste() return {} end
  function duplicator.CreateEntityFromTable() return nil end
end
if numpad == nil then
  numpad = {}
  function numpad.Register() return "" end
  function numpad.OnUp() return 0 end
  function numpad.OnDown() return 0 end
  function numpad.Activate() end
  function numpad.Deactivate() end
end
if properties == nil then
  properties = {}
  function properties.Add() end
  function properties.GetAll() return {} end
end
if usermessage == nil then
  usermessage = {}
  function usermessage.Hook() end
  function usermessage.IncomingMessage() end
end
if umsg == nil then
  umsg = {}
  function umsg.Start() end
  function umsg.End() end
  function umsg.Bool() end function umsg.Char() end function umsg.Long() end
  function umsg.Short() end function umsg.String() end function umsg.Float() end
  function umsg.Entity() end function umsg.Vector() end function umsg.Angle() end
end

-- gamemode / GAMEMODE convenience
if gamemode == nil then
  gamemode = {}
  function gamemode.Call( name, ... ) if hook and hook.Call then return hook.Call( name, nil, ... ) end end
  function gamemode.Register() end
  function gamemode.Get() return _GAMEMODE or GAMEMODE end
end

-- concommand: GMod's console-command library. The engine ALREADY dispatches
-- registered console commands into Lua by calling the global concommand.Dispatch
-- (see CC_ConCommand in lconvar.cpp) with (player, cmd, argString). We keep the
-- name->callback map here and register each command with the native ConCommand()
-- so it actually fires. Addons call concommand.Add at file scope, so this must
-- exist before autorun runs.
if concommand == nil and ConCommand ~= nil then
  local cmds, complete = {}, {}
  -- tokenize an argument string, respecting double-quoted groups (GMod does).
  local function parseArgs( s )
    local t = {}
    if type( s ) ~= "string" then return t end
    local i, n = 1, #s
    while i <= n do
      local c = s:sub( i, i )
      if c == " " or c == "\t" then
        i = i + 1
      elseif c == '"' then
        local j = s:find( '"', i + 1, true )
        if j then t[ #t + 1 ] = s:sub( i + 1, j - 1 ) i = j + 1
        else t[ #t + 1 ] = s:sub( i + 1 ) break end
      else
        local j = s:find( "[ \t]", i )
        if j then t[ #t + 1 ] = s:sub( i, j - 1 ) i = j
        else t[ #t + 1 ] = s:sub( i ) break end
      end
    end
    return t
  end
  concommand = {}
  function concommand.Add( name, fn, autofn, help, flags )
    if type( name ) ~= "string" or type( fn ) ~= "function" then return end
    local key = string.lower( name )
    cmds[ key ] = fn
    complete[ key ] = autofn
    ConCommand( name, help or "", flags or 0 ) -- register with the engine
  end
  function concommand.Remove( name ) local k = string.lower( name ) cmds[ k ] = nil complete[ k ] = nil end
  function concommand.GetTable() return cmds end
  function concommand.AutoComplete( name, argstr )
    local fn = complete[ string.lower( name or "" ) ]
    if fn then return fn( name, argstr ) end
    return {}
  end
  -- Invoked by the engine when a registered command runs. Returning false makes
  -- the engine print "Unknown command"; true means we handled it.
  function concommand.Dispatch( ply, cmd, argstr )
    local fn = cmds[ string.lower( cmd or "" ) ]
    if fn == nil then return false end
    fn( ply, cmd, parseArgs( argstr ), argstr or "" )
    return true
  end
end

-- RunConsoleCommand( cmd, args... ): run a console command. The native engine
-- library exposes ServerCommand/ServerExecute, so route through it; if that is
-- unavailable (client realm), fall back to setting a matching convar directly.
if RunConsoleCommand == nil then
  function RunConsoleCommand( cmd, ... )
    local args = { ... }
    for i = 1, #args do args[ i ] = tostring( args[ i ] ) end
    local line = tostring( cmd )
    if #args > 0 then line = line .. " " .. table.concat( args, " " ) end
    if engine ~= nil and engine.ServerCommand ~= nil then
      engine.ServerCommand( line .. "\n" )
      if engine.ServerExecute ~= nil then engine.ServerExecute() end
      return
    end
    local c = GetConVar and GetConVar( cmd )
    if c and args[ 1 ] ~= nil then c:SetString( args[ 1 ] ) end
  end
end

-- util.PrecacheModel / util.PrecacheSound: GMod globals many SWEPs call at
-- file scope. Back them with the engine precache when available (pcall-guarded
-- so a bad-timing call never aborts addon load); models also auto-precache on
-- SetModel, so a no-op fallback is still safe.
if util.PrecacheModel == nil then
  function util.PrecacheModel( m ) if engine and engine.PrecacheModel then pcall( engine.PrecacheModel, m ) end end
end
if util.PrecacheSound == nil then
  function util.PrecacheSound( s ) if engine and engine.PrecacheGeneric then pcall( engine.PrecacheGeneric, s ) end end
end

-- game library: map/server info + prop-management no-ops. Only the pieces
-- addons touch at file scope or in shared logic; unknown values degrade to
-- honest defaults for this single-player-derived sandbox port.
if game == nil then
  game = {}
  function game.SinglePlayer() return true end
  function game.MaxPlayers() return math.max( 1, ( player and player.GetAll and #player.GetAll() ) or 1 ) end
  function game.GetMap() return ( engine and engine.GetMapEntitiesString and "" ) or "" end
  function game.GetIPAddress() return "127.0.0.1:27015" end
  function game.IsDedicated() return false end
  function game.AddParticles() end
  function game.AddDecal() end
  function game.CleanUpMap() end
  function game.ConsoleCommand( cmd ) if RunConsoleCommand then RunConsoleCommand( cmd ) end end
  function game.GetWorld() return ( ents and ents.GetByIndex and ents.GetByIndex( 0 ) ) or NULL end
end

-- team library: GMod implements this in Lua, so port it faithfully. Team data
-- lives in a local table; scores/membership work against the player list.
if team == nil then
  team = {}
  local teams = {}
  function team.SetUp( id, name, color, joinable )
    local t = teams[ id ] or {}
    t.Name = name; t.Color = color or Color( 255, 255, 255 )
    t.Joinable = joinable ~= false; t.Score = t.Score or 0
    teams[ id ] = t
  end
  function team.Valid( id ) return teams[ id ] ~= nil end
  function team.GetName( id ) local t = teams[ id ] return t and t.Name or "" end
  function team.GetColor( id ) local t = teams[ id ] return t and t.Color or Color( 255, 255, 255 ) end
  function team.SetColor( id, c ) local t = teams[ id ] if t then t.Color = c end end
  function team.GetAllTeams() return teams end
  function team.GetScore( id ) local t = teams[ id ] return t and t.Score or 0 end
  function team.SetScore( id, s ) local t = teams[ id ] if t then t.Score = s end end
  function team.AddScore( id, s ) local t = teams[ id ] if t then t.Score = ( t.Score or 0 ) + s end end
  function team.Joinable( id ) local t = teams[ id ] return t ~= nil and t.Joinable ~= false end
  function team.GetPlayers( id )
    local out = {}
    for _, p in ipairs( player.GetAll() ) do if p.Team and p:Team() == id then out[ #out + 1 ] = p end end
    return out
  end
  function team.NumPlayers( id ) return #team.GetPlayers( id ) end
  function team.BestAutoJoinTeam() return 1 end
  function team.TotalDeaths() return 0 end
  function team.TotalFrags() return 0 end
end

-- client-only draw / surface GMod-name layer. Colors, rects, lines and
-- textured rects map 1:1 onto the native VGUI surface; fonts are created
-- through a name->handle cache so surface.SetFont / draw.SimpleText work.
if CLIENT and surface ~= nil then
  local fonts = {}
  local curfont = nil
  local nativeTextSize = surface.GetTextSize
  local function colorArgs( r, g, b, a )
    if type( r ) == "table" then return r.r or 255, r.g or 255, r.b or 255, r.a or 255 end
    return r or 255, g or 255, b or 255, a or 255
  end

  surface.SetDrawColor = surface.SetDrawColor or function( r, g, b, a )
    local cr, cg, cb, ca = colorArgs( r, g, b, a ) surface.DrawSetColor( cr, cg, cb, ca )
  end
  surface.SetTextColor = surface.SetTextColor or function( r, g, b, a )
    local cr, cg, cb, ca = colorArgs( r, g, b, a ) surface.DrawSetTextColor( cr, cg, cb, ca )
  end
  surface.DrawRect = surface.DrawRect or function( x, y, w, h ) surface.DrawFilledRect( x, y, x + w, y + h ) end
  surface.SetTextPos = surface.SetTextPos or surface.DrawSetTextPos

  if surface.CreateFont ~= nil and surface.SetFontGlyphSet ~= nil then
    local nativeCreate = surface.CreateFont
    function surface.CreateFont( name, data )
      if type( name ) ~= "string" then return nativeCreate() end
      data = data or {}
      local h = nativeCreate()
      local flags = 0x200 -- FONTFLAG_ANTIALIAS
      if data.antialias == false then flags = 0 end
      surface.SetFontGlyphSet( h, data.font or "Tahoma", data.size or 16, data.weight or 500,
        data.blursize or 0, data.scanlines or 0, flags )
      fonts[ name ] = h
      return h
    end
    function surface.SetFont( name )
      local h = fonts[ name ]
      if h ~= nil then curfont = h surface.DrawSetTextFont( h ) end
    end
  end

  -- GMod's surface.GetTextSize( text ) uses the current font; the native form
  -- is GetTextSize( font, text ). Support both so base content and addons work.
  surface.GetTextSize = function( a, b )
    if type( a ) == "string" then
      if curfont == nil then return 0, 0 end
      return nativeTextSize( curfont, a )
    end
    return nativeTextSize( a, b )
  end

  surface.DrawText = surface.DrawText or function( text ) surface.DrawPrintText( tostring( text ) ) end

  if draw == nil then draw = {} end
  draw.NoTexture = draw.NoTexture or function() surface.DrawSetTexture( -1 ) end
  draw.RoundedBox = draw.RoundedBox or function( r, x, y, w, h, color )
    surface.SetDrawColor( color )
    surface.DrawRect( x, y, w, h )
  end
  draw.RoundedBoxEx = draw.RoundedBoxEx or function( r, x, y, w, h, color ) draw.RoundedBox( r, x, y, w, h, color ) end
  draw.SimpleText = draw.SimpleText or function( text, font, x, y, color, xalign, yalign )
    text = tostring( text )
    if font then surface.SetFont( font ) end
    local tw, th = surface.GetTextSize( text )
    tw = tw or 0 th = th or 0
    xalign = xalign or 0 yalign = yalign or 0
    if xalign == 1 then x = x - tw / 2 elseif xalign == 2 then x = x - tw end
    if yalign == 1 then y = y - th / 2 elseif yalign == 4 then y = y - th end
    surface.SetTextColor( color or { r = 255, g = 255, b = 255, a = 255 } )
    surface.SetTextPos( math.floor( x ), math.floor( y ) )
    surface.DrawText( text )
    return tw, th
  end
  draw.Text = draw.Text or function( t )
    return draw.SimpleText( t.text, t.font, t.pos and t.pos[ 1 ] or 0, t.pos and t.pos[ 2 ] or 0,
      t.color, t.xalign, t.yalign )
  end
  draw.DrawText = draw.DrawText or function( text, font, x, y, color, xalign )
    return draw.SimpleText( text, font, x, y, color, xalign )
  end

  -- draw / text alignment enums
  if TEXT_ALIGN_LEFT == nil then
    TEXT_ALIGN_LEFT = 0; TEXT_ALIGN_CENTER = 1; TEXT_ALIGN_RIGHT = 2
    TEXT_ALIGN_TOP = 3; TEXT_ALIGN_BOTTOM = 4
  end

  -- screen dimensions, backed by the native surface.GetScreenSize (wide, tall).
  -- ScreenScale scales a 640-wide reference value to the real resolution.
  if ScrW == nil and surface.GetScreenSize ~= nil then
    function ScrW() local w = surface.GetScreenSize() return w end
    function ScrH() local _, h = surface.GetScreenSize() return h end
  end
  if ScreenScale == nil then function ScreenScale( s ) return s * ( ( ScrW and ScrW() or 640 ) / 640 ) end end
  SScale = SScale or ScreenScale
end
)GLUACOMPAT";

// Time globals. GMod addons call these constantly (often unguarded), but the
// engine exposes no Lua binding for them, so read gpGlobals directly.
static int luasrc_CurTime( lua_State *L )     { lua_pushnumber( L, gpGlobals->curtime ); return 1; }
static int luasrc_RealTime( lua_State *L )    { lua_pushnumber( L, gpGlobals->realtime ); return 1; }
static int luasrc_FrameTime( lua_State *L )   { lua_pushnumber( L, gpGlobals->frametime ); return 1; }
static int luasrc_TickInterval( lua_State *L ){ lua_pushnumber( L, gpGlobals->interval_per_tick ); return 1; }
static int luasrc_TickCount( lua_State *L )   { lua_pushinteger( L, gpGlobals->tickcount ); return 1; }
static int luasrc_SysTime( lua_State *L )     { lua_pushnumber( L, Plat_FloatTime() ); return 1; }

static const luaL_Reg s_GModTimeGlobals[] = {
  { "CurTime", luasrc_CurTime },
  { "RealTime", luasrc_RealTime },
  { "FrameTime", luasrc_FrameTime },
  { "TickInterval", luasrc_TickInterval },
  { "TickCount", luasrc_TickCount },
  { "SysTime", luasrc_SysTime },
  { NULL, NULL }
};

#ifdef CLIENT_DLL
// LocalPlayer(): GMod's most-used client global (viewmodels, HUD, prediction).
// The engine exposes no Lua binding for it, so read C_BasePlayer::GetLocalPlayer
// directly. A NULL result (before spawn) becomes an invalid entity handle,
// matching GMod's NULL, so IsValid( LocalPlayer() ) works during load.
static int luasrc_LocalPlayer( lua_State *L )
{
  lua_pushplayer( L, C_BasePlayer::GetLocalPlayer() );
  return 1;
}
#endif

LUALIB_API void luasrc_openlibs (lua_State *L) {
  const luaL_Reg *lib = luasrclibs;
  for (; lib->func; lib++) {
    lua_pushcfunction(L, lib->func);
    lua_pushstring(L, lib->name);
    lua_call(L, 1, 0);
  }

  // Time globals (CurTime/FrameTime/...) into _G.
  for ( const luaL_Reg *t = s_GModTimeGlobals; t->name; t++ )
  {
    lua_pushcfunction( L, t->func );
    lua_setglobal( L, t->name );
  }

#ifdef CLIENT_DLL
  // LocalPlayer() global (client only).
  lua_pushcfunction( L, luasrc_LocalPlayer );
  lua_setglobal( L, "LocalPlayer" );
#endif

  // The shared util functions (TraceLine, EntitiesInSphere, AddNetworkString,
  // ...) were not reaching the util table via luaopen_UTIL_shared; force them
  // on now so the GMod-compat shims and addons can use them.
  extern void luasrc_EnsureGModUtil( lua_State *L );
  luasrc_EnsureGModUtil( L );

  // Fill in the GMod stdlib gaps after the native libraries are registered.
  if ( luaL_dostring( L, s_pGModCompatPrelude ) != 0 )
  {
    Warning( "GMod compat prelude error: %s\n", lua_tostring( L, -1 ) );
    lua_pop( L, 1 );
  }
}

