// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// Copyright(C) 2007-2012 Samuel Villarreal
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
//
//-----------------------------------------------------------------------------

#ifndef _ACTOR_H_
#define _ACTOR_H_

#include "render.h"
#include "level.h"

typedef struct
{
    int                 type;
    char                name[32];
    union
    {
        int     i;
        float   f;
        void    *p;
        char    *c;
    } val;
} propKey_t;

typedef struct gActor_s
{
    vec3_t              origin;
    vec4_t              rotation;
    vec3_t              scale;
    kbool               bOrientOnSlope;
    kbool               bStatic;
    kbool               bCollision;
    kbool               bTouch;
    kbool               bClientOnly;
    kbool               bHidden;
    bbox_t              bbox;
    float               angles[3];
    char                name[64];
    int                 plane;
    float               radius;
    float               height;
    float               centerHeight;
    float               viewHeight;
    mtx_t               matrix;
    mtx_t               rotMtx;
    gObject_t           *components;
	gObject_t           *iterator;
    int                 refcount;
    propKey_t           *properties;
    int                 numProperties;
    kmodel_t            *model;
    animstate_t         animState;
    int                 variant;
    char                **textureSwaps;
    float               timestamp;
    struct gActor_s     *prev;
    struct gActor_s     *next;
} gActor_t;

typedef struct gActorTemplate_s
{
    char                    name[MAX_FILEPATH];
    gActor_t                actor;
    char                    **components;
    unsigned int            numComponents;
    struct gActorTemplate_s *next;
} gActorTemplate_t;

void Actor_Setup(gActor_t *actor);
void Actor_SetTarget(gActor_t **self, gActor_t *target);
void Actor_UpdateTransform(gActor_t *actor);
void Actor_CallEvent(gActor_t *actor, const char *function, gActor_t *instigator);
void Actor_LocalTick(void);
void Actor_Tick(void);
kbool Actor_HasComponent(gActor_t *actor, const char *component);
void Actor_OnTouchEvent(gActor_t *actor, gActor_t *instigator);
void Actor_AddIntegerProperty(gActor_t *actor, const char *name, int id, int value);
void Actor_AddFloatProperty(gActor_t *actor, const char *name, int id, float value);
void Actor_AddDataProperty(gActor_t *actor, const char *name, int id, void *value);
void Actor_AddStringProperty(gActor_t *actor, const char *name, int id, char *value);
void Actor_Remove(gActor_t *actor);
gActor_t *Actor_Spawn(const char *classname, float x, float y, float z,
                      float yaw, float pitch, int plane);

#endif
