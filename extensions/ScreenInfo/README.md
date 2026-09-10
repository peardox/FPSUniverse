# RezolSteam

Libraries to interrogate the screen for Gamemaker Studio

# Dependancies

Your projects directory probably looks something like this once this repo is cloned

+ projects
  + RezolSteam
  + SomeOtherProject_01
  + SomeOtherProject_02
  + SomeOtherProject_03

You need to add another directory called Steamworks_161_sdk _under_ projects
Download the [Steam v1.61 SDK](https://partner.steamgames.com/downloads/steamworks_sdk_161.zip)
Extract it
Copy the contents of the src folder into Steamworks_161_sdk

After you've done this your directory tree will look more like this
+ projects
  + RezolSteam
  + SomeOtherProject_01
  + SomeOtherProject_02
  + SomeOtherProject_03
  + Steamworks_161_sdk
    + glmgr
    + public
    + redistributable_bin
    + steamworksexample
    + tools
    + Readme.txt

The project requires this layout in order to access the steam SDK. The latest version that GMS supports is 1.61 hence the directory naming. When the GMS Steam Extension gets updated to a newer version that version will be used - i.e. you may end up with a Steamworks_162_sdk as well (Steamworks_161_sdk will be obsolete if/when this happens)

# Important

This stuff uses some third party open source code. You need to grab it with the commands below. If you don't do this compilation WILL FAIL

git submodule update --init --recursive --remote
git submodule update --recursive --remote


### Playing with the source

This repo has access controls which means that you can't make modifications to this repo. Any modifications to the master repo need to be merged by a user with the required permissions (peardox for this repo ATM)

This means that you can't clone this repo, fix a typo, and push it back to master. This will fail and the repo will remain untouched.

Instead you need to create a Fork of this repo in your own, personal GitHub account. GitHub has a Fork button just above the green 'Code' button. Click that and you will create your own private version on your own GitHub. This one you can download and modify freely as well as push back to your own repo.

When you make some changes that need to be applied to the master repo you need to create a Pull Request (universally referred to as a PR). There'll be a notice after your push informing you of this option. If you make a PR your CHANGED code will be provided to the main repo. It can then be applied to the master version if deemed applicable by the owner of the original repo (or delegated users).

As it is often desirable to try out multiple things it is recommended that you create a BRANCH of your own Fork and work on that. Branches are independent of the main fork so you can try out multiple things at the same time without any one messing up the other(s)

### A simple guide to Fork and Branch

To create a branch do something like this...

> git clone https://github.com/MYUSER/RezolSteam.git
>
> cd RezolSteam
>
> git checkout -b MyFancyNewBranch

The -b part of the command creates a new BRANCH

This creates the MyFancyNewBranch version of RezolSteam that you can work on and push. When pushed it will appear as a sub-option in your version of the repo

You can switch between branches simply by checking out without the -b option so...

> git checkout main

This switched you back to the main repo and anything you did in MyFancyNewBranch is no longer present in your version BUT...

> git checkout MyFancyNewBranch

Switches you back to the MyFancyNewBranch version with any changes instact.

For this reason it is good practise to never change anything in main and always work on a BRANCH

There is a different version of the git push stuff needed to push a BRANCH which looks like this

> git push --set-upstream origin MyFancyNewBranch

Don't worry if you forget though as if you try a "git push" git will helpfully tell you what the commant you actually need is (it won't do it automatically just in case). 



# RezolBuild

How to build (Windows)

cmake -B build

cmake --build build --config Release

Test the DLL with .\build\bin\Release\TestDLLInternal.exe and check the output - it should match your attached monitors.

If running a VC command prompt you can use dumpbin /EXPORTS .\build\bin\Release\RezolWindowsAux_x64.dll to examine the result.

The TestApp directory contains a GML test application, building from the root (as above) also installs the latest build of the DLL
into the TestApp/extensions/RexolTest directory to make testing easier (no need to copy DLLs around)

# git modules

### Update with

git submodule update --init --recursive --remote
git submodule update --recursive --remote

##### Created with

git submodule add https://github.com/stbrumme/hash-library src/shared/hash-library
git submodule add https://github.com/pensnarik/consolas-font SteamTest/fonts/consolas