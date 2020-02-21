1Engine: primary repo
=====================

1Engine is a simple single-thread engine meant for extremely simple 2D games and overly complicated 3D games. Cross-platform support is planned.

The original EHS engine, now mostly passion project. This the **primary repo** with all code that is able to be legally public.

**For those with low bandwidth: this will use more than 200 MB of bandwidth to download. This will eventually change as ancient 3rd party binaries are removed from the history.**

## Prerequisites

For Windows, the following are required:
* Visual Studio 2015 or later.
* Some version of the DirectX or Windows SDK (used for Xbox controller support).
* Relevant SDK for the rendering API being used (except OpenGL, that just magically works).
* Python 3 (used for recompiling shaders on before runtime)

You can compile and run without these with generous applications of self-inflicted pain and suffering, but it is not recommended.

Currently supported platforms:
* Windows x64 MSVC (In progress...)
* Windows x64 MSClang (In progress...)

Current graphics API support:
* **OpenGL 4.7 (Default)** - Functional graphics & compute. Incorrect synchronization. Can (maybe) fall back to 4.5.
* **DirectX 11** - In progress. Requires Windows 8.1 SDK.
* **Vulkan 1.X** - In progress. Requires Vulkan SDK, available from LunarG.

## Branches

There are three main prefixes to branches:

* ``master`` Current stable.
* ``testing`` Current thing in internal testing. This is the branch to be on.
* ``lab`` Current thing in development. This is where the kool kids are at.

Each prefix may be suffixed by author and subfoldered by project or descriptor. For instance, ``lab_jboren/dec2017_ld40_features`` refers to an in-development branch by "jboren", with new features created for Ludum Dare 40. When considered stable or complete, these suffixed or subfoldered branches will be merged into their main branch. This behavior is consistent even under the exceptions for actual large game projects.

Large game projects are mostly the same, but instead are prefixed by the game internal ID or name, ie ``game/branch``. For example, [AFTER](http://epichousestudios.com/after/), or game internal ID "M01," will instead by named ``M01/master``, ``M01/testing``, and ``M01/lab``. The contents of each branch are then described by their name.

## Folder Structure

* ``/oneEngine`` actual engine things
  * ``oneGame`` engine code, game code, project files, and resources
    * ``libs`` 3rd party binary libraries
    * ``source-lib`` 3rd party headers and source for library support
    * ``source`` game and engine source code (this is where the magic happens)
      * ``core`` ``core-ext`` common code for all 1Engine support
      * ``engine`` game engine backend
      * ``audio`` audio engine backend. uses OpenAL Soft, limited optional FMOD support
      * ``physical`` animation and physics backend
      * ``gpuw`` graphics API wrapper(s)
      * ``renderer`` hybrid forward renderer
      * ``engine-common`` simple game framework
      * ``engine2d`` ``render2d`` 2d game framework extensions & 2d deferred renderer
      * ``deploy`` backend for creating executables
      * ``tool`` backend for IPC tools needed for development. replaces ``_devtools``
      * ``after`` ``after-editor`` ``ld40`` ``m04`` ``m04-editor`` ``tool-suite`` game and tool projects
    * ``_devtools`` programs & code for tools needed for development. **This will be phasing out**
    * ``.res-1`` engine common resources
    * ``.res-0`` base game resources
    * ``.res+N`` where N is any integer, nondestructive file replacement support
    * ``addons`` plugin support folder

## Repos

* ``primary`` you're looking at it. public.
* ``experimental`` scripting language, networking, other rendering pipelines (like consoles and stuff). ask for access, as bunch of the stuff legally cannot be on github.

## Legal Info

You're welcome to download and mess around. If you want to filch code, feel free to ask. There's quite a lot of filched code in 1Engine itself. If you filch code, you can go under the eventual list of "Games Made With/Filched Code" and benefit from the 1-person-per-month traffic of that. (It's been measureed over several years, the traffic actually is about 1 non-bot person per month.)

You're welcome to try to make your own games or projects with this, free of charge.

Also, stuff that doesn't belong to us doesn't belong to us. This engine a lot of 3rd party code, which someone will eventually list here and give credit where credit is due.
If there is any code in this repository that you think should not be here, please contact @skarik immediately and he will evalutate and, if needed, remove it.
