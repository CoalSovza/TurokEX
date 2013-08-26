// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// Copyright(C) 2012 Samuel Villarreal
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
//
// DESCRIPTION: Javascript System Class
//
//-----------------------------------------------------------------------------

#include "js.h"
#include "js_shared.h"
#include "common.h"
#include "system.h"
#include "client.h"
#include "zone.h"
#include "gl.h"
#include "fx.h"
#include "sound.h"
#include "parse.h"
#include "filesystem.h"

JS_CLASSOBJECT(Sys);

JS_FASTNATIVE_BEGIN(Sys, print)
{
    uintN i;
    JSString *str;
    char *bytes;
    jsval *v = JS_ARGV(cx, vp);

    for (i = 0; i < argc; i++)
    {
        JS_GETSTRING(str, bytes, v, i);

        common.Printf("%s\n", bytes);
        JS_free(cx, bytes);
    }

    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

JS_FASTNATIVE_BEGIN(Sys, log)
{
    uintN i;
    JSString *str;
    char *bytes;
    jsval *v = JS_ARGV(cx, vp);

    for (i = 0; i < argc; i++)
    {
        JS_GETSTRING(str, bytes, v, i);

        common.DPrintf("%s\n", bytes);
        JS_free(cx, bytes);
    }

    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

JS_FASTNATIVE_BEGIN(Sys, error)
{
    uintN i;
    JSString *str;
    char *bytes;
    jsval *v = JS_ARGV(cx, vp);

    for (i = 0; i < argc; i++)
    {
        JS_GETSTRING(str, bytes, v, i);

        if(JS_IsExceptionPending(cx))
            JS_ReportPendingException(cx);

        JS_ReportError(cx, bytes);
        JS_free(cx, bytes);
    }

    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

JS_FASTNATIVE_BEGIN(Sys, ms)
{
    //return JS_NewNumberValue(cx, sysMain.GetMS(), vp);
    JS_SET_RVAL(cx, vp, INT_TO_JSVAL(sysMain.GetMS()));
    return JS_TRUE;
}

JS_FASTNATIVE_BEGIN(Sys, time)
{
    //return JS_NewNumberValue(cx, client.time, vp);
    JS_SET_RVAL(cx, vp, INT_TO_JSVAL(client.GetTime()));
    return JS_TRUE;
}

JS_FASTNATIVE_BEGIN(Sys, deltatime)
{
    return JS_NewDoubleValue(cx, client.GetRunTime(), vp);
}

JS_FASTNATIVE_BEGIN(Sys, fixedTime)
{
    return JS_NewDoubleValue(cx, (1000.0f / cvarClientFPS.GetFloat()) / 1000.0f, vp);
}

JS_FASTNATIVE_BEGIN(Sys, ticks)
{
    //return JS_NewNumberValue(cx, client.tics, vp);
    JS_SET_RVAL(cx, vp, INT_TO_JSVAL(client.GetTicks()));
    return JS_TRUE;
}

JS_FASTNATIVE_BEGIN(Sys, rand)
{
    if(argc > 0)
    {
        JS_CHECKARGS(1);
        JS_CHECKINTEGER(0);

        //return JS_NewNumberValue(cx,
            //(jsdouble)Random_Max(JSVAL_TO_INT(JS_ARG(0))), vp);

        JS_SET_RVAL(cx, vp, INT_TO_JSVAL(Random_Max(JSVAL_TO_INT(JS_ARG(0)))));
        return JS_TRUE;
    }

    //return JS_NewNumberValue(cx, (jsdouble)Random_Int(), vp);
    JS_SET_RVAL(cx, vp, INT_TO_JSVAL(Random_Int()));
    return JS_TRUE;
}

JS_FASTNATIVE_BEGIN(Sys, fRand)
{
    return JS_NewDoubleValue(cx, (jsdouble)Random_Float(), vp);
}

JS_FASTNATIVE_BEGIN(Sys, cRand)
{
    return JS_NewDoubleValue(cx, (jsdouble)Random_CFloat(), vp);
}

JS_FASTNATIVE_BEGIN(Sys, getCvar)
{
    JSString *str;
    char *bytes;
    kexCvar *cvar;

    JS_CHECKARGS(1);
    JS_GETSTRING(str, bytes, v, 0);

    cvar = cvarManager.Get(bytes);
    JS_free(cx, bytes);

    if(!cvar)
        return JS_FALSE;

    return JS_NewDoubleValue(cx, cvar->GetFloat(), vp);
}

JS_FASTNATIVE_BEGIN(Sys, callCmd)
{
    JSString *str;
    char *bytes;

    JS_CHECKARGS(1);
    JS_GETSTRING(str, bytes, v, 0);

    command.Execute(bytes);
    JS_free(cx, bytes);

    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

JS_FASTNATIVE_BEGIN(Sys, addCommand)
{
    JSObject *obj;
    JSString *str;
    char *bytes;

    JS_CHECKARGS(2);

    JS_GETSTRING(str, bytes, v, 0);
    JS_GETOBJECT(obj, v, 1);

    if(obj == NULL)
        return JS_FALSE;

    if(JS_ObjectIsFunction(cx, obj))
        command.Add(bytes, obj);
    
    JS_free(cx, bytes);

    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

JS_FASTNATIVE_BEGIN(Sys, addCvar)
{
    JSString *str1;
    JSString *str2;
    char *bytes1;
    char *bytes2;
    kexCvar *cvar;

    JS_CHECKARGS(2);

    JS_GETSTRING(str1, bytes1, v, 0);
    JS_GETSTRING(str2, bytes2, v, 1);

    cvar = new kexCvar(bytes1, CVF_BOOL|CVF_CONFIG|CVF_ALLOCATED, bytes2, "TODO");

    if(cvar == NULL)
    {
        JS_free(cx, bytes1);
        JS_free(cx, bytes2);

        return JS_FALSE;
    }

    JS_SET_RVAL(cx, vp, v[1]);
    return JS_TRUE;
}

JS_FASTNATIVE_BEGIN(Sys, runScript)
{
    JSString *str;
    js_scrobj_t *jfile;
    char *bytes;

    JS_CHECKARGS(1);
    JS_GETSTRING(str, bytes, v, 0);

    if(!(jfile = J_LoadScript(bytes)))
    {
        JS_ReportPendingException(cx);
        JS_ReportError(cx, "Unable to load %s", bytes);
        JS_free(cx, bytes);

        return JS_FALSE;
    }

    J_ExecScriptObj(jfile);
    JS_free(cx, bytes);

    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

JS_FASTNATIVE_BEGIN(Sys, dependsOn)
{
    JSString *str;
    char *bytes;
    js_scrobj_t *jfile;

    JS_CHECKARGS(1);
    JS_GETSTRING(str, bytes, v, 0);

    if(J_FindScript(bytes) == NULL)
    {
        if(!(jfile = J_LoadScript(bytes)))
        {
            JS_ReportPendingException(cx);
            JS_ReportError(cx, "Unable to load %s", bytes);
            JS_free(cx, bytes);

            return JS_FALSE;
        }

        J_ExecScriptObj(jfile);
    }

    JS_free(cx, bytes);

    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

JS_FASTNATIVE_BEGIN(Sys, unsetEnumerate)
{
    JSObject *obj;
    JSString *str;
    char *bytes;
    uintN attributes;
    JSBool found;

    JS_CHECKARGS(2);
    JS_GETOBJECT(obj, v, 0);
    JS_GETSTRING(str, bytes, v, 1);

    JS_GetPropertyAttributes(cx, obj, bytes, &attributes, &found);
    attributes &= ~JSPROP_ENUMERATE;
    JS_SetPropertyAttributes(cx, obj, bytes, attributes, &found);
    JS_free(cx, bytes);

    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

JS_FASTNATIVE_BEGIN(Sys, cacheTexture)
{
    JSString *str;
    char *bytes;
    texture_t *texture;

    JS_CHECKARGS(1);
    JS_GETSTRING(str, bytes, v, 0);

    texture = Tex_CacheTextureFile(bytes, DGL_CLAMP, true);
    JS_free(cx, bytes);

    if(texture == NULL)
        JS_SET_RVAL(cx, vp, JSVAL_NULL);
    else
    {
        JSObject *obj = J_NewObjectEx(cx, &Texture_class, NULL, NULL);

        if(obj == NULL)
            return JS_FALSE;

        JS_AddRoot(cx, &obj);
        JS_DefineProperty(cx, obj, "width", INT_TO_JSVAL(texture->width),
            NULL, NULL, JSPROP_ENUMERATE|JSPROP_READONLY);
        JS_DefineProperty(cx, obj, "height", INT_TO_JSVAL(texture->height),
            NULL, NULL, JSPROP_ENUMERATE|JSPROP_READONLY);
        JS_DefineProperty(cx, obj, "origWidth", INT_TO_JSVAL(texture->origwidth),
            NULL, NULL, JSPROP_ENUMERATE|JSPROP_READONLY);
        JS_DefineProperty(cx, obj, "origHeight", INT_TO_JSVAL(texture->origheight),
            NULL, NULL, JSPROP_ENUMERATE|JSPROP_READONLY);
        JS_DefineProperty(cx, obj, "clampMode", INT_TO_JSVAL(texture->clampmode),
            NULL, NULL, JSPROP_ENUMERATE|JSPROP_READONLY);
        JS_SetPrivate(cx, obj, texture);
        JS_RemoveRoot(cx, &obj);

        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));
    }

    return JS_TRUE;
}

JS_FASTNATIVE_BEGIN(Sys, loadModel)
{
    JSString *str;
    char *bytes;
    kmodel_t *model;

    JS_CHECKARGS(1);
    JS_GETSTRING(str, bytes, v, 0);

    model = Kmesh_Load(bytes);
    JS_free(cx, bytes);

    if(model == NULL)
        JS_SET_RVAL(cx, vp, JSVAL_NULL);
    else
    {
        JS_NEWOBJECT_SETPRIVATE(model, &Model_class);
    }

    return JS_TRUE;
}

JS_FASTNATIVE_BEGIN(Sys, loadAnimation)
{
    JSObject *object;
    JSString *str;
    char *bytes;
    kmodel_t *model;
    anim_t *animation;

    JS_CHECKARGS(2);
    JS_GETOBJECT(object, v, 0);
    JS_GETSTRING(str, bytes, v, 1);
    JS_GET_PRIVATE_DATA(object, &Model_class, kmodel_t, model);

    animation = Mdl_GetAnim(model, bytes);
    JS_free(cx, bytes);

    if(model == NULL)
        JS_SET_RVAL(cx, vp, JSVAL_NULL);
    else
    {
        JS_NEWOBJECT_SETPRIVATE(animation, &Animation_class);
    }

    return JS_TRUE;
}

JS_FASTNATIVE_BEGIN(Sys, spawnFx)
{
    JSString *str;
    char *bytes;
    gActor_t *actor;
    JSObject *actorobj;
    JSObject *dirobj;
    JSObject *destobj;
    JSObject *rotobj;
    JSObject *plobj;
    vec3_t dir;
    vec3_t dest;
    vec4_t rot;
    plane_t *plane;
    fx_t *fx;
    jsval *v;

    if(argc < 5 || argc > 8)
        return JS_FALSE;

    v = JS_ARGV(cx, vp);
    actor = NULL;

    JS_GETSTRING(str, bytes, v, 0);

    if(!JSVAL_IS_NULL(v[1]))
    {
        JS_GETOBJECT(actorobj, v, 1);
        if(!(actor = (gActor_t*)JS_GetInstancePrivate(cx, actorobj, &GameActor_class, NULL)))
            actor = NULL;
    }

    JS_GETOBJECT(destobj, v, 2);

    if(!JSVAL_IS_NULL(v[3]))
    {
        JS_GETOBJECT(rotobj, v, 3);
    }

    plane = NULL;

    if(!JSVAL_IS_NULL(v[4]))
    {
        JS_GETOBJECT(plobj, v, 4);
        if(!(plane = (plane_t*)JS_GetInstancePrivate(cx, plobj, &Plane_class, NULL)))
            plane = NULL;
    }

    JS_GETVECTOR2(destobj, dest);

    Vec_Set4(rot, 0, 0, 0, 1);

    if(!JSVAL_IS_NULL(v[3]))
        JS_GETQUATERNION2(rotobj, rot);

    Vec_Set3(dir, 0, 0, 0);

    if(argc >= 6)
    {
        if(!JSVAL_IS_NULL(v[5]))
        {
            JS_GETOBJECT(dirobj, v, 5);
            JS_GETVECTOR2(dirobj, dir);
        }
    }

    fx = FX_Spawn(bytes, actor, dir, dest, rot, plane);
    JS_free(cx, bytes);

    if(fx != NULL)
    {
        if(argc == 7)
        {
            JSString *str;
            char *bytes;

            JS_GETSTRING(str, bytes, v, 6);
            Snd_PlayShader(bytes, (gActor_t*)fx);
            JS_free(cx, bytes);
        }

        if(argc == 8)
        {
            JS_GETBOOL(fx->bAttachToSource, v, 7);
        }
    }

   JS_SET_RVAL(cx, vp, JSVAL_VOID);
   return JS_TRUE;
}

JS_FASTNATIVE_BEGIN(Sys, newTextFile)
{
    JSObject *obj;
    JSString *str;
    char *bytes;
    FILE *f;
    
    JS_CHECKARGS(1);
    JS_GETSTRING(str, bytes, v, 0);

    f = fopen(bytes, "w");
    JS_free(cx, bytes);

    obj = J_NewObjectEx(cx, NULL, NULL, NULL);
    JS_SetPrivate(cx, obj, f);
    
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));
    return JS_TRUE;
}

JS_FASTNATIVE_BEGIN(Sys, writeTextFile)
{
    JSObject *obj;
    JSString *str;
    char *bytes;
    FILE *f;
    
    JS_CHECKARGS(2);
    JS_GETOBJECT(obj, v, 0);
    JS_GETSTRING(str, bytes, v, 1);

    if(!(f = (FILE*)JS_GetPrivate(cx, obj)))
        JS_WARNING();

    fprintf(f, bytes);
    JS_free(cx, bytes);
    
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

JS_FASTNATIVE_BEGIN(Sys, readTextFile)
{
    JSString *str;
    char *bytes;
    char *buffer;
    int len;

    JS_CHECKARGS(1);
    JS_GETSTRING(str, bytes, v, 0);

    len = fileSystem.ReadExternalTextFile(bytes, (byte**)&buffer);
    JS_free(cx, bytes);

    if(len == -1)
        JS_SET_RVAL(cx, vp, JSVAL_NULL);
    else
    {
        JS_RETURNSTRING(vp, buffer);
        Z_Free(buffer);
    }

    return JS_TRUE;
}

JS_FASTNATIVE_BEGIN(Sys, closeTextFile)
{
    JSObject *obj;
    FILE *f;
    
    JS_CHECKARGS(1);
    JS_GETOBJECT(obj, v, 0);

    if(!(f = (FILE*)JS_GetPrivate(cx, obj)))
        JS_WARNING();

    fclose(f);
    JS_SetPrivate(cx, obj, NULL);
    
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

JS_BEGINCLASS(Sys)
    0,                                          // flags
    JS_PropertyStub,                            // addProperty
    JS_PropertyStub,                            // delProperty
    JS_PropertyStub,                            // getProperty
    JS_PropertyStub,                            // setProperty
    JS_EnumerateStub,                           // enumerate
    JS_ResolveStub,                             // resolve
    JS_ConvertStub,                             // convert
    JS_FinalizeStub,                            // finalize
    JSCLASS_NO_OPTIONAL_MEMBERS                 // getObjectOps etc.
JS_ENDCLASS();

JS_BEGINPROPS(Sys)
{
    { NULL, 0, 0, NULL, NULL }
};

JS_BEGINCONST(Sys)
{
    { 0, 0, 0, { 0, 0, 0 } }
};

JS_BEGINFUNCS(Sys)
{
    JS_FASTNATIVE(Sys, print, 0),
    JS_FASTNATIVE(Sys, log, 0),
    JS_FASTNATIVE(Sys, error, 0),
    JS_FASTNATIVE(Sys, ms, 0),
    JS_FASTNATIVE(Sys, time, 0),
    JS_FASTNATIVE(Sys, deltatime, 0),
    JS_FASTNATIVE(Sys, fixedTime, 0),
    JS_FASTNATIVE(Sys, ticks, 0),
    JS_FASTNATIVE(Sys, rand, 1),
    JS_FASTNATIVE(Sys, fRand, 0),
    JS_FASTNATIVE(Sys, cRand, 0),
    JS_FASTNATIVE(Sys, getCvar, 1),
    JS_FASTNATIVE(Sys, callCmd, 1),
    JS_FASTNATIVE(Sys, addCommand, 2),
    JS_FASTNATIVE(Sys, addCvar, 2),
    JS_FASTNATIVE(Sys, runScript, 1),
    JS_FASTNATIVE(Sys, dependsOn, 1),
    JS_FASTNATIVE(Sys, unsetEnumerate, 2),
    JS_FASTNATIVE(Sys, cacheTexture, 1),
    JS_FASTNATIVE(Sys, loadModel, 1),
    JS_FASTNATIVE(Sys, loadAnimation, 2),
    JS_FASTNATIVE(Sys, readTextFile, 1),
    JS_FASTNATIVE(Sys, spawnFx, 8),
    JS_FASTNATIVE(Sys, newTextFile, 1),
    JS_FASTNATIVE(Sys, writeTextFile, 2),
    JS_FASTNATIVE(Sys, readTextFile, 1),
    JS_FASTNATIVE(Sys, closeTextFile, 1),
    JS_FS_END
};