
                                       Mapbase Multiplayer v7.2 - Source 2013
                                https://github.com/FriskTheFallenHuman/mapbase-mp
                                        https://www.moddb.com/mods/mapbase

This repository contains code from Mapbase, a modification of the Source 2013 SDK which serves as a combined package
of general-purpose improvements, fixes, and utility features for mods.

Mapbase's main content in this repository may include:

- 80+ custom entities (new logic entities, filters, etc.)
- Hundreds of Inputs/Outputs/KeyValues additions and modifications.
- Custom SDK_ shaders with improvements adapted from Alien Swarm SDK code, including projected texture fixes and radial fog.
- Custom VScript implementation based on public Squirrel API and Alien Swarm SDK interfaces/descriptions.
- Additional gameplay control for Half-Life 2 mods, including grenade features for more NPCs and singleplayer respawning.
- More map-specific capabilities for maps unable to branch into their own mods, e.g. MP mods or map compilations.
- View rendering changes for drawing 3D skyboxes and RT-based entities.
- Countless other fixes and improvements.
- HL2MP Fixes and improvements.

For more information, view this [page.](https://github.com/mapbase-source/source-sdk-2013/wiki/Introduction-to-Mapbase)

Mapbase is an open-source project and its contents can be distributed and used at the discretion of its users. However, this project contains content from
a vast number of different sources which have their own licensing or attribution requirements. We try to handle most of that ourselves, but users who plan on
distributing Mapbase content are expected to comply with certain rules.

### 🚧 Up-to-date information about Mapbase content usage and credit are addressed in this article on Mapbase's [wiki.](https://github.com/mapbase-source/source-sdk-2013/wiki/Using-Mapbase-Content) 🚧


# EXTERNAL CONTENT USED IN MAPBASE

The Mapbase project is a combination of original code from its creators, code contributed by other Source modders, and code borrowed from open-source articles
and repositories (especially ones which are specifically published as free source code). One of Mapbase's goals is to make the most popular fixes and the most obscure
or complicated code changes accessible and easy to use for level designers and other kinds of Source modders who would otherwise have no idea how to implement them.

* **DISCLAIMER**: Mapbase has a strict no-leak-content policy and only allows content created directly by contributors or content originating from open-source repositories.
If you believe any content in Mapbase originates from any leak or unauthorized source (Valve or otherwise), please contact Blixibon immediately.
Mapbase is intended to be usable by everyone, including licensed Source projects and Steam mods.

Mapbase uses content from the following non-Source SDK 2013 Valve games or SDKs:

- Alien Swarm SDK (Used to backport features and code from newer branches of Source into a Source 2013/Half-Life 2 environment)
- Source SDK 2007 Code (Used to implement some of Tony Sergi's code changes)
- Alien Swarm (Used to port assets from the aforementioned SDK code features, e.g. game instructor icons)
- Left 4 Dead (Used to port certain animations as well as assets from the aforementioned SDK code features, e.g. particle rain)
- Half-Life: Source (Used to port friction tool textures)

Valve allows assets from these titles to be distributed for modding purposes. Note that ported assets are only used in the release build, not the code repository.

Mapbase may also contain new third-party software distributed under specific licensing. Please see the bottom of thirdpartylegalnotices.txt for more information.

## Here's a list of Mapbase's other external code sources:

- [95Navigator/insolence-2013](https://github.com/95Navigator/insolence-2013): Initial custom shader code and projected texture improvements; also used to implement ASW SDK particle precipitation code.
  - [Biohazard90/g-string_2013](https://github.com/Biohazard90/g-string_2013): Custom shadow filters, included indirectly via Insolence repo.
  - [KyleGospo/City-17-Episode-One-Source](https://github.com/KyleGospo/City-17-Episode-One-Source): Brush phong and projected texture changes, included indirectly via Insolence repo.
- [DownFall-Team/DownFall](https://github.com/DownFall-Team/DownFall): Multiple skybox code and fix for ent_fire delay not using floats; Also used as a guide to port certain Alien Swarm SDK changes to Source 2013,
  including radial fog, rope code, and treesway.
- [momentum-mod/game](https://github.com/momentum-mod/game): Used as a guide to port `postprocess_controller` and `env_dof_controller` to Source 2013 from the Alien Swarm SDK & New Content Mounter.
- [DeathByNukes/source-sdk-2013](https://github.com/DeathByNukes/source-sdk-2013): VBSP manifest fixes.
- [entropy-zero/source-sdk-2013](https://github.com/entropy-zero/source-sdk-2013): `skill_changed` game event.
- [source-sdk-2013-ce/tree/v142](https://github.com/Nbc66/source-sdk-2013-ce/tree/v142): Base for VS2019 toolset support.
- [source-sdk-2013-ce/tree/master](https://github.com/Nbc66/source-sdk-2013-ce/tree/master): Shader api hack.
- [hrgve/hl2dm-bugfix](https://github.com/hrgve/hl2dm-bugfix): Most of our HL2MP fixes comes from this repo.
- [Adrianilloo/SourceSDK2013](https://github.com/Adrianilloo/SourceSDK2013): More fixes for HL2MP.
- [zm-reborn/zmr-game](https://github.com/zm-reborn/zmr-game): Misc Fixes & Improvements to the HL2MPAnimstates & New C_Hands system.
- [weaponcubemap/source-sdk-2013](https://github.com/weaponcubemap/source-sdk-2013): More Fixes to HL2MP and fixes for null pointers.
- [reactivedrop_public_src/reactivedrop_beta](https://github.com/ReactiveDrop/reactivedrop_public_src/tree/reactivedrop_beta): Added LanguagePreference

## Valve Developer Community (VDC) sources:

- [Dynamic RTT Shadow Angles](https://developer.valvesoftware.com/wiki/Dynamic_RTT_shadow_angles_in_Source_2007): Dynamic RTT shadow angles by Saul Rennison.
- [Parallax Corrected Cubemaps](https://developer.valvesoftware.com/wiki/Parallax_Corrected_Cubemaps): Parallax corrected cubemaps implementation from Brian Charles.
- [Adding The Game Instructor](https://developer.valvesoftware.com/wiki/Adding_the_Game_Instructor): ASW SDK game instructor adapted to Source 2013 by Kolesias123; was implemented based on a translated article
- [Brush Ladders](https://developer.valvesoftware.com/wiki/Brush_ladders): Functional `func_ladders` in Source 2013.
- [CAreaPortalOneWay](https://developer.valvesoftware.com/wiki/CAreaPortalOneWay): `func_areaportal_oneway`
- [Implementing Discord RPC](https://developer.valvesoftware.com/wiki/Implementing_Discord_RPC): Discord RPC implementation; Mapbase has its own framework originally based on this article.
- [Rain Splashes](https://developer.valvesoftware.com/wiki/Rain_splashes): NOTE: This code is not completely used in Mapbase, but may still exist in its codebase.
- [Hand Viewmodels](https://developer.valvesoftware.com/wiki/Hand_Viewmodels): NOTE: This code is not completely supported by default because Mapbase does not yet have weapon viewmodels which support
  interchangeable arms; this may change in the future
- [General SDK Snippets & Fixes](https://developer.valvesoftware.com/wiki/General_SDK_Snippets_%26_Fixes): Various snippets on the article, including the dropship gun fix.
- [Memory Leak Fixes](https://developer.valvesoftware.com/wiki/Memory_Leak_Fixes): Most of these snippets were applied in later SDK updates, but some were not and have been added to Mapbase.
- [Env_projectedtexture fixes](https://developer.valvesoftware.com/wiki/Env_projectedtexture/fixes): Misc. `env_projectedtexture` fixes; Some of these are superceded by Alien Swarm-based changes and not used.
- [scenes.image](https://developer.valvesoftware.com/wiki/Scenes.image): Original raw VCD file support; Code was improved for Mapbase and the article was later updated with it.
- [Extending Prop Sphere](https://developer.valvesoftware.com/wiki/Extending_Prop_Sphere): `prop_sphere` customization.
- [TF2 Glow Effect (2013_SDK)](https://developer.valvesoftware.com/wiki/TF2_Glow_Effect_(2013_SDK)): Glow effect.
- [CFuncMoveLinear ParentingFix](https://developer.valvesoftware.com/wiki/CFuncMoveLinear_ParentingFix): `func_movelinear` parenting fix; Code was improved for Mapbase and the article was later updated with it.
- [Viewmodel Prediction Fix](https://developer.valvesoftware.com/wiki/Viewmodel_Prediction_Fix): Viewmodel prediction fix.
- [Owner: Collisions_with_owner](https://developer.valvesoftware.com/wiki/Owner#Collisions_with_owner): `FSOLID_COLLIDE_WITH_OWNER` flag.
- [Npc_clawscanner: Strider_Scout_Issue](https://developer.valvesoftware.com/wiki/Npc_clawscanner#Strider_Scout_Issue): `npc_clawscanner` strider scout fix.
- [Ambient generic: stop and toggle fix](https://developer.valvesoftware.com/wiki/Ambient_generic:_stop_and_toggle_fix): Fixes for stopping/toggling ambient_generic.
- [Func_clip_vphysics](https://developer.valvesoftware.com/wiki/Func_clip_vphysics): `"Start Disabled"` keyvalue fix.
- [Importing CSS Weapons_Into HL2](https://developer.valvesoftware.com/wiki/Importing_CSS_Weapons_Into_HL2): CS:S viewmodel chirality.

## Direct contributions:

- [Pull Request #3](https://github.com/mapbase-source/source-sdk-2013/pull/3): `"playvideo"` command playback fix from Avanate.
- [Pull Request #5](https://github.com/mapbase-source/source-sdk-2013/pull/5): Custom VScript implementation by ReDucTor; was placed into feature branch before being merged in a subsequent PR.
- [Pull Request #60](https://github.com/mapbase-source/source-sdk-2013/pull/60): Adjustment by RoyaleNoir to one of Saul's VDC changes.
- [Pull Request #84](https://github.com/mapbase-source/source-sdk-2013/pull/84): CS:S viewmodel chirality from 1upD.
- [Pull Request #116](https://github.com/mapbase-source/source-sdk-2013/pull/116): vgui_movie_display mute keyvalue from Alivebyte/rzkid.
- [Pull Request #140](https://github.com/mapbase-source/source-sdk-2013/pull/140): `logic_substring` entity and icon created by moofemp.
- [Pull Request #143](https://github.com/mapbase-source/source-sdk-2013/pull/143): Propper features for VBSP from Matty-64.
- [Pull Request #174](https://github.com/mapbase-source/source-sdk-2013/pull/174): Fix for multiply defined symbols in later toolsets from und.
- [Pull Request #201](https://github.com/mapbase-source/source-sdk-2013/issues/201): `env_projectedtexture` shadow filter keyvalue from celisej567.
- [Pull Request #193](https://github.com/mapbase-source/source-sdk-2013/pull/193): RTB:R `info_particle_system_coordinate` by arbabf and Iridium77.
- [Pull Request #193](https://github.com/mapbase-source/source-sdk-2013/pull/193): Infinite `prop_interactable` cooldown by arbabf.
- Demo autorecord code provided by Klems
- cc_emit crash fix provided by 1upD
- Custom HL2 ammo crate models created by Rykah (Textures created by Blixibon; This is asset-based and, aside from the SLAM crate, not reflected in the code)
- Combine lock hardware on door01_left.mdl created by Kralich (This is asset-based and not reflected in the code)
- `npc_vehicledriver` fixes provided by CrAzY
- `npc_combine` cover behavior patches provided by iohnnyboy
- `logic_playmovie` icon created by URAKOLOUY5 (This is asset-based and not reflected in the code)

## Contributions from samisalreadytaken:

- [Pull Request #47](https://github.com/mapbase-source/source-sdk-2013/pull/47): VScript utility/consistency changes.
- [Pull Request #59](https://github.com/mapbase-source/source-sdk-2013/pull/59): New VScript functions and singletons based on API documentation in later Source/Source 2 games.
- [Pull Request #80](https://github.com/mapbase-source/source-sdk-2013/pull/80): More VScript changes, including support for extremely flexible client/server messaging.
- [Pull Request #105](https://github.com/mapbase-source/source-sdk-2013/pull/105): VScript fixes and optimizations, Vector class extensions, custom convars/commands.
- [Pull Request #114](https://github.com/mapbase-source/source-sdk-2013/pull/114): VScript fixes and extensions.
- [Pull Request #122](https://github.com/mapbase-source/source-sdk-2013/pull/122): Minor VScript-related adjustments.
- [Pull Request #148](https://github.com/mapbase-source/source-sdk-2013/pull/148): Minor fixup.
- [Pull Request #167](https://github.com/mapbase-source/source-sdk-2013/pull/167): Security fixes.
- [Pull Request #168](https://github.com/mapbase-source/source-sdk-2013/pull/168): Squirrel update.
- [Pull Request #169](https://github.com/mapbase-source/source-sdk-2013/pull/169): VScript VGUI.
- [Pull Request #171](https://github.com/mapbase-source/source-sdk-2013/pull/171): VScript documentation sorting.
- [Pull Request #173](https://github.com/mapbase-source/source-sdk-2013/pull/173): VScript fixes and optimizations.
- [Pull Request #192](https://github.com/mapbase-source/source-sdk-2013/pull/192): VScript hook manager and fixes.
- [Pull Request #206](https://github.com/mapbase-source/source-sdk-2013/pull/206): Fix `CScriptNetMsgHelper::WriteEntity()`.
- [Pull Request #213](https://github.com/mapbase-source/source-sdk-2013/pull/213): VScript HUD visibility control, optimizations for 3D skybox angles/fake worldportals.
- [Pull Request #221](https://github.com/mapbase-source/source-sdk-2013/pull/221): VScript serialisation rewrite.
- [Pull Request #217](https://github.com/mapbase-source/source-sdk-2013/pull/217): Vscript vgui hud viewport parent.

## Contributions from z33ky:

- [Pull Request #21](https://github.com/mapbase-source/source-sdk-2013/pull/21): Various GCC/Linux compilation fixes.
- [Pull Request #95](https://github.com/mapbase-source/source-sdk-2013/pull/95): Additional GCC/Linux compilation fixes.
- [Pull Request #117](https://github.com/mapbase-source/source-sdk-2013/pull/117): Additional GCC/Linux compilation fixes.
- [Pull Request #124](https://github.com/mapbase-source/source-sdk-2013/pull/124): Memory error fixes.
- [Pull Request #130](https://github.com/mapbase-source/source-sdk-2013/pull/130): Memory error fixes.
- [Pull Request #131](https://github.com/mapbase-source/source-sdk-2013/pull/131): `env_projectedtexture` target shadows fix.
- [Pull Request #132](https://github.com/mapbase-source/source-sdk-2013/pull/132): Console error fix
- [Pull Request #152](https://github.com/mapbase-source/source-sdk-2013/pull/152): Additional GCC/Linux compilation fixes.
- [Pull Request #159](https://github.com/mapbase-source/source-sdk-2013/pull/159): Additional GCC/Linux compilation fixes.
- [Pull Request #162](https://github.com/mapbase-source/source-sdk-2013/pull/162): VS2019 exception specification fix.
- [Pull Request #170](https://github.com/mapbase-source/source-sdk-2013/pull/170): HL2 non-Episodic build fix.

## Contributions from Petercov:

- [Pull Request #182](https://github.com/mapbase-source/source-sdk-2013/pull/182): NPCs load dynamic interactions from all animation MDLs.
- [Pull Request #184](https://github.com/mapbase-source/source-sdk-2013/pull/184): Projected texture horizontal FOV shadow fix.
- [Pull Request #185](https://github.com/mapbase-source/source-sdk-2013/pull/185): Fix enemyfinders becoming visible when they wake.
- [Pull Request #186](https://github.com/mapbase-source/source-sdk-2013/pull/186): Fix for brightly glowing teeth.
- [Pull Request #230](https://github.com/mapbase-source/source-sdk-2013/pull/230): Caption Fixes.
- [Pull Request #231](https://github.com/mapbase-source/source-sdk-2013/pull/231): Fixed sentence source bug.

## Contributions from arbabf:

- [Pull Request #229](https://github.com/mapbase-source/source-sdk-2013/pull/229): Extend `point_bugbait` functionality.

## Contributions from Mechami:

- [Pull Request #248](https://github.com/mapbase-source/source-sdk-2013/pull/248): Fixes warnings about conversions between `soundlevel_t` and `float`.

## Contributions from AgentAgrimar:

- [Pull Request #237](https://github.com/mapbase-source/source-sdk-2013/pull/237): Assign Held Physcannon Entity to Pass into Commander Goal Trace.

## Contributions from Mr0maks:

- [Pull Request #245](https://github.com/mapbase-source/source-sdk-2013/pull/245): Fix ViewPunch random for AR2.

## Contributions from Crimson-X1:

- [Pull Request #236](https://github.com/mapbase-source/source-sdk-2013/pull/236): Toggleable "Prop Sprinting" Mechanic.

# Other sources:

- Various code from [Source SDK 2013](https://github.com/ValveSoftware/source-sdk-2013/pulls) pull requests on the GitHub repository:
  - [Source SDK 2013 - Pull Request #441](https://github.com/ValveSoftware/source-sdk-2013/pull/441): Smooth scrape sound oversight fix.
  - [Source SDK 2013 - Pull Request #436](https://github.com/ValveSoftware/source-sdk-2013/pull/436): VRAD debug counter fix + thread bump.
  - [Source SDK 2013 - Pull Request #416](https://github.com/ValveSoftware/source-sdk-2013/pull/416): Ragdoll null pointer dereference fix.
  - [Source SDK 2013 - Pull Request #401](https://github.com/ValveSoftware/source-sdk-2013/pull/401): `func_rot_button` "Starts locked" flag fix.
  - [Source SDK 2013 - Pull Request #391](https://github.com/ValveSoftware/source-sdk-2013/pull/391): VBSP func_detail smoothing group fix.
  - [Source SDK 2013 - Pull Request #362](https://github.com/ValveSoftware/source-sdk-2013/pull/362): `npc_manhack npc_maker` fix; Adjusted for formatting and save/restore in Mapbase.
  - [Source SDK 2013 - Pull Request #334](https://github.com/ValveSoftware/source-sdk-2013/pull/334): Added buffer security checks in `sentence.cpp`.
  - [Source SDK 2013 - Pull Request #229](https://github.com/ValveSoftware/source-sdk-2013/pull/229): Adding include guards to `collision_set.h` per issue #228.
  - [Source SDK 2013 - Pull Request #515](https://github.com/ValveSoftware/source-sdk-2013/pull/515): Fixed server physics when server has custom tickrate.
  - [Source SDK 2013 - Pull Request #552](https://github.com/ValveSoftware/source-sdk-2013/pull/552): Fix incorrect plane calculation in `IntersectRayWithOBB`.
  - [Source SDK 2013 - Pull Request #532](https://github.com/ValveSoftware/source-sdk-2013/pull/532): Fix for being unable to detonate the satchel when holding a detonator (i.e. having a satchel in the world) and ready tripmine in hands.
  - [Source SDK 2013 - Pull Request #515](https://github.com/ValveSoftware/source-sdk-2013/pull/515): Fixed server physics when server has custom tickrate.
  - [Source SDK 2013 - Pull Request #437](https://github.com/ValveSoftware/source-sdk-2013/pull/437): Fix two grenade bugs.
  - [Source SDK 2013 - Pull Request #376](https://github.com/ValveSoftware/source-sdk-2013/pull/376): Move cursor to the actual center of the chat box.
  - [Source SDK 2013 - Pull Request #331](https://github.com/ValveSoftware/source-sdk-2013/pull/331): Lag compensation now supports pose parameters.
  - [Source SDK 2013 - Pull Request #316](https://github.com/ValveSoftware/source-sdk-2013/pull/316): Added `EntityVectors` material proxy.
  - [Source SDK 2013 - Pull Request #363](https://github.com/ValveSoftware/source-sdk-2013/pull/363): Fix false positive in `netadr_t::IsReservedAdr`.
  - [Source SDK 2013 - Pull Request #367](https://github.com/ValveSoftware/source-sdk-2013/pull/367): Fix segfault in `NDebugOverlay::Circle` when debugoverlay is nullptr.
  - [Source SDK 2013 - Pull Request #277](https://github.com/ValveSoftware/source-sdk-2013/pull/277): Fix assert failure when dying by crashing a vehicle.
  - [Source SDK 2013 - Pull Request #324](https://github.com/ValveSoftware/source-sdk-2013/pull/324): Updated `triggers.cpp - point_viewcontrol`.
  - [Source SDK 2013 - Pull Request #381](https://github.com/ValveSoftware/source-sdk-2013/pull/381): Fix dereferencing null if `m_iActiveSound` is no longer valid index by save restore.
  - [Source SDK 2013 - Pull Request #434](https://github.com/ValveSoftware/source-sdk-2013/pull/434): Fixed bug in `CCommand` constructor that could append junk to the command string.
- [Source++ Commit](https://github.com/Petercov/Source-PlusPlus/commit/ecdf50c48cd31dec4dbdb7fea2d0780e7f0dd8ec): used as a guide for porting the Alien Swarm SDK response system.
- [Momentum Mod](https://github.com/momentum-mod/game/blob/1d066180b3bf74830c51e6914d46c40b0bea1fc2/mp/src/game/server/player.cpp#L6543):
- spec_goto fix.
- Poison zombie barnacle crash fix implemented based on a snippet from HL2: Plus posted by Agent Agrimar on Discord (Mapbase makes the barnacle recognize it as poison just like poison headcrabs)
- [Gamebanana](https://gamebanana.com/skins/172192): Airboat handling fix; This is asset-based and not reflected in the code.
- Vortigaunt LOS fix originally created by dky.tehkingd.u for HL2:CE
- [Combine Icon](https://combineoverwiki.net/wiki/File:Combine_main_symbol.svg): `"icon_combine"` instructor icon in `"materials/vgui/hud/gameinstructor_hl2_1"`; This is asset-based and not reflected in the code.
- [Momentum Mod - Pull Request #1150](https://github.com/momentum-mod/game/pull/1150) and [Momentum Mod - Pull Request #1161](https://github.com/momentum-mod/game/pull/1161): Momentum Mod Content Mounting system.
- [Source SDK 2013 Issue #3347](https://github.com/ValveSoftware/Source-1-Games/issues/3347): Used as a guide for fixing showinfo command.
- [Source SDK 2013 Issue #404](https://github.com/ValveSoftware/source-sdk-2013/issues/404): Models go spastic after some time.
- [Strata Engine Issue #700](https://github.com/StrataSource/Engine/issues/700): Small typo in `CLogicBranch`.
- [Source SDK 2013 - Pull Request #560](https://github.com/ValveSoftware/source-sdk-2013/pull/560): Client Crash: Creating bugbait at particle limit.

If there is anything missing from this list, please contact Krispy.

### ✅ Aside from the content list above, Mapbase has more descriptive and up-to-date credits on [this](https://github.com/mapbase-source/source-sdk-2013/wiki/Mapbase-Credits) wiki article. ✅

## Other relevant articles:

* [Disclaimers.](https://github.com/mapbase-source/source-sdk-2013/wiki/Mapbase-Disclaimers)
* [Frequently Asked Questions (FAQ)](https://github.com/mapbase-source/source-sdk-2013/wiki/Frequently-Asked-Questions-(FAQ))

### The original Source SDK 2013 License can be found [here.](https://github.com/ValveSoftware/source-sdk-2013/blob/master/LICENSE)

                      In memory of Holly Liberatore (moofemp)