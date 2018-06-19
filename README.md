1Engine: primary repo
=====================

EHS internal engine. This the **primary repo** with all code that is able to be legally public.

**For those with low bandwidth: this will use more than 200 MB of bandwidth to download. This will eventually change as ancient 3rd party binaries are removed from the history.**

1Engine is a simple single-thread engine meant for extremely simple 2D games and overly complicated 3D games. Cross-platform support is planned.

## Prerequisites

For Windows, the following are required:
* Visual Studio 2015 or later (2017 is fine, but expect some wierdness with msvc 141).
* Some version of the DirectX SDK (Windows 8 SDK is fine).

You can compile and run without these with generous applications of self-inflicted pain and suffering, but it is not recommended.

Currently supported platforms:
* Windows x86 MSVC
* Windows x86 MSClang
* Windows x64 MSVC (Compiles but renderer module does not link)

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
      * ``renderer`` opengl 4 renderer support
	  * ``engine-common`` simple game framework
	  * ``deploy`` backend for creating executables
	* ``_devtools`` programs & code for tools needed for development
    * ``.res-1`` engine common resources
	* ``.res-0`` base game resources
	* ``.res+N`` where N is any integer, nondestructive file replacement support
	* ``addons`` plugin support folder
* ``/oneEngine-render-vk`` vulkan 1 renderer support
* ``/oneEngine-render-11`` directX 11 renderer support

## Repos

* ``primary`` you're looking at it. public.
* ``experimental`` scripting language, networking, other rendering pipelines (like consoles and stuff). ask for access, as bunch of the stuff legally cannot be on github.

## Legal Info

You're welcome to download and mess around. If you want to filch code, please ask. Now, seriously that isn't very enforcable (there's quite a lot of filched code in 1Engine itself) but it would be polite to ask. You can go under the eventual list of "Games Made With/Filched Code" and benefit from the 1-person-per-month traffic of that.

You're welcome to try to make your own games or projects with this, as long as you don't make a considerable profit from them. We're talking, "puts food on the table" profit. We want you to starve.

Also, stuff that doesn't belong to us doesn't belong to us. We use a lot of 3rd party code, which we will eventually list here and give credit where credit is due.
Also, it's entirely possible that there is some code or code history in here by mistake. If that is the case, please contact us immediately and we will remove it.
