[![Build Status](https://travis-ci.org/bnolan/lucy.svg?branch=master)](https://travis-ci.org/bnolan/lucy)

# Lucy

Lucy is a multiplayer collaborative world where anyone can write scripts (that are 
constrained to prevent abuse) that add things to the world. This world is streamed
over an xml socket as diffs. This collaborative world can be rendered as a webpage,
or as a 2d or 3d world. The goal is to make a simulator that is fun to extend
with your own code.

## Constraints

Each user gets their own lua context. They are memory and process constrained. There
is a small API for accessing the internet via xmlhttprequest. Scripts show their
output by modifying the world state.

Users can only generate a specified number of elements in the world.

## Abuse

Users scripts can be turned off if they are found to be causing abuse, this will
make all the users scripts and elements invisible from the world.

## DOM

The world state is expressed using a level-1 dom. Lua scripts have access to this
API.

## Demo

This is super early stage, so there are no demos available.