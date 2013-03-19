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

#ifndef _JS_SHARED_H_
#define _JS_SHARED_H_

#include "jsapi.h"
#include "shared.h"
#include "js_objPool.h"

extern JSContext    *js_context;
extern JSObject     *js_gobject;

extern jsObjectPool_t objPoolVector;
extern jsObjectPool_t objPoolQuaternion;
extern jsObjectPool_t objPoolPacket;
extern jsObjectPool_t objPoolAnimState;
extern jsObjectPool_t objPoolHost;
extern jsObjectPool_t objPoolPeer;
extern jsObjectPool_t objPoolNetEvent;
extern jsObjectPool_t objPoolGameActor;
extern jsObjectPool_t objPoolPlane;
extern jsObjectPool_t objPoolInputEvent;

typedef struct js_scrobj_s
{
    char name[MAX_FILEPATH];
    JSScript *script;
    JSObject *obj;
    struct js_scrobj_s *next;
} js_scrobj_t;

jsval J_GetObjectElement(JSContext *cx, JSObject *object, jsint index);
jsval J_CallFunctionOnObject(JSContext *cx, JSObject *object, const char *function);
jsuint J_AllocFloatArray(JSContext *cx, JSObject *object, float **arr, JSBool fixed);
jsuint J_AllocWordArray(JSContext *cx, JSObject *object, word **arr, JSBool fixed);
jsuint J_AllocByteArray(JSContext *cx, JSObject *object, byte **arr, JSBool fixed);
char *J_GetObjectClassName(JSObject *object);

js_scrobj_t *J_FindScript(const char *name);
js_scrobj_t *J_LoadScript(const char *name);
void J_ExecScriptObj(js_scrobj_t *scobj);

#define JS_DEFINEOBJECT(name)                                                       \
    js_obj ##name = J_AddObject(&name ## _class, name ## _functions,                \
    name ## _props, name ## _const, # name, js_context, js_gobject)

#define JS_INITCLASS(name, args)                                                    \
    js_obj ##name = JS_InitClass(js_context, js_gobject, NULL,                      \
    &name ## _class, name ## _construct, args, name ## _props,                      \
    name ## _functions, NULL, name ## _functions_static)

#define JS_INITCLASS_NOCONSTRUCTOR(name, args)                                      \
    js_obj ##name = JS_InitClass(js_context, js_gobject, NULL,                      \
    &name ## _class, NULL, args, name ## _props,                                    \
    name ## _functions, NULL, name ## _functions_static)

#define JS_EXTERNOBJECT(name)                                                       \
    extern JSObject *js_obj ##name;                                                 \
    extern JSClass name ## _class;                                                  \
    extern JSFunctionSpec name ## _functions[];                                     \
    extern JSPropertySpec name ## _props[];                                         \
    extern JSConstDoubleSpec name ## _const[]

#define JS_EXTERNCLASS(name)                                                        \
    extern JSObject *js_obj ##name;                                                 \
    extern JSClass name ## _class;                                                  \
    extern JSFunctionSpec name ## _functions[];                                     \
    extern JSFunctionSpec name ## _functions_static[];                              \
    extern JSPropertySpec name ## _props[];                                         \
    JSBool name ## _construct(JSContext *cx, JSObject *obj, uintN argc,             \
                        jsval *argv, jsval *rval)

#define JS_EXTERNCLASS_NOCONSTRUCTOR(name)                                          \
    extern JSObject *js_obj ##name;                                                 \
    extern JSClass name ## _class;                                                  \
    extern JSFunctionSpec name ## _functions[];                                     \
    extern JSFunctionSpec name ## _functions_static[];                              \
    extern JSPropertySpec name ## _props[];                                         

#define JS_CLASSOBJECT(name)  JSObject *js_obj ##name
#define JS_BEGINCLASS(name) JSClass name ## _class = { # name,
#define JS_ENDCLASS()   }

#define JS_BEGINPROPS(name) JSPropertySpec name ## _props[] =
#define JS_BEGINCONST(name) JSConstDoubleSpec name ## _const[] =
#define JS_BEGINFUNCS(name) JSFunctionSpec name ## _functions[] =
#define JS_BEGINSTATICFUNCS(name) JSFunctionSpec name ## _functions_static[] =

#define JS_CONSTRUCTOR(class)   \
    JSBool class ## _construct(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *vp)

#define JS_FASTNATIVE(class, name, args)  \
    JS_FN(# name,    class ## _ ##name,    args, 0, 0)

#define JS_PROP_FUNC_GET(class) \
    static JSBool class ## _getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)

#define JS_PROP_FUNC_SET(class) \
    static JSBool class ## _setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)

#define JS_FINALIZE_FUNC(class) \
    static void class ## _finalize(JSContext *cx, JSObject *obj)

#define JS_FASTNATIVE_BEGIN(class, name)   \
    static JSBool class ## _ ##name (JSContext *cx, uintN argc, jsval *vp)

#define JS_DEFINE_CONST(name, val)   \
    { val, #name, 0, { 0, 0, 0 } }

#define JS_GET_PROPERTY_OBJECT(obj, prop, outObj)                   \
{                                                                   \
    jsval val;                                                      \
    JSBool b;                                                       \
    if(!JS_HasProperty(cx, obj, prop, &b))                          \
        return JS_FALSE;                                            \
    if(!b)                                                          \
        return JS_FALSE;                                            \
    if(!JS_GetProperty(cx, obj, prop, &val))                        \
        return JS_FALSE;                                            \
    if(!JS_ValueToObject(cx, val, &outObj))                         \
        return JS_FALSE;                                            \
}

#define JS_GET_PROPERTY_NUMBER(obj, prop, outnum)                   \
{                                                                   \
    jsval val;                                                      \
    JSBool b;                                                       \
    if(!JS_HasProperty(cx, obj, prop, &b))                          \
        return JS_FALSE;                                            \
    if(!b)                                                          \
        return JS_FALSE;                                            \
    if(!JS_GetProperty(cx, obj, prop, &val))                        \
        return JS_FALSE;                                            \
    if(JSVAL_IS_NULL(val))                                          \
        return JS_FALSE;                                            \
    if(!JS_ValueToNumber(cx, val, &outnum))                         \
        return JS_FALSE;                                            \
}

#define JS_GET_PROPERTY_BOOL(obj, prop, outBool)                    \
{                                                                   \
    jsval val;                                                      \
    JSBool b;                                                       \
    if(!JS_HasProperty(cx, obj, prop, &b))                          \
        return JS_FALSE;                                            \
    if(!b)                                                          \
        return JS_FALSE;                                            \
    if(!JS_GetProperty(cx, obj, prop, &val))                        \
        return JS_FALSE;                                            \
    if(JSVAL_IS_NULL(val))                                          \
        return JS_FALSE;                                            \
    if(!JS_ValueToBoolean(cx, val, &outBool))                       \
        return JS_FALSE;                                            \
}

#define JS_GET_PRIVATE_DATA(obj, class, size, out)                  \
    if(!(out = (size*)JS_GetInstancePrivate(cx, obj, class, NULL))) \
        return JS_FALSE

#define JS_GETQUATERNION(vec, v, a)                                                 \
{                                                                                   \
    JSObject *vobj; if(!JS_ValueToObject(cx, v[a], &vobj)) return JS_FALSE;         \
    if(JSVAL_IS_NULL(v[a])) return JS_FALSE;                                        \
    if(!(JS_InstanceOf(cx, vobj, &Quaternion_class, NULL))) return JS_FALSE;        \
    if(!(vec = (vec4_t*)JS_GetInstancePrivate(cx, vobj, &Quaternion_class, NULL)))  \
        return JS_FALSE;                                                            \
}

#define JS_GETVECTOR2(obj, vec)                     \
{                                                   \
    fint_t x, y, z;                                 \
    int s1, s2;                                     \
    jsval val;                                      \
    JS_GetReservedSlot(cx, obj, 0, &val);           \
    s1 = JSVAL_TO_INT(val);                         \
    JS_GetReservedSlot(cx, obj, 1, &val);           \
    s2 = JSVAL_TO_INT(val);                         \
    x.i = (s1 | (s2 << 16));                        \
    JS_GetReservedSlot(cx, obj, 2, &val);           \
    s1 = JSVAL_TO_INT(val);                         \
    JS_GetReservedSlot(cx, obj, 3, &val);           \
    s2 = JSVAL_TO_INT(val);                         \
    y.i = (s1 | (s2 << 16));                        \
    JS_GetReservedSlot(cx, obj, 4, &val);           \
    s1 = JSVAL_TO_INT(val);                         \
    JS_GetReservedSlot(cx, obj, 5, &val);           \
    s2 = JSVAL_TO_INT(val);                         \
    z.i = (s1 | (s2 << 16));                        \
    vec[0] = x.f;                                   \
    vec[1] = y.f;                                   \
    vec[2] = z.f;                                   \
}

#define JS_SETVECTOR(obj, vec)                                      \
{                                                                   \
    fint_t fx, fy, fz;                                              \
    fx.f = vec[0];                                                  \
    fy.f = vec[1];                                                  \
    fz.f = vec[2];                                                  \
    JS_SetReservedSlot(cx, obj, 0, INT_TO_JSVAL(fx.i & 0xffff));    \
    JS_SetReservedSlot(cx, obj, 1, INT_TO_JSVAL(fx.i >> 16));       \
    JS_SetReservedSlot(cx, obj, 2, INT_TO_JSVAL(fy.i & 0xffff));    \
    JS_SetReservedSlot(cx, obj, 3, INT_TO_JSVAL(fy.i >> 16));       \
    JS_SetReservedSlot(cx, obj, 4, INT_TO_JSVAL(fz.i & 0xffff));    \
    JS_SetReservedSlot(cx, obj, 5, INT_TO_JSVAL(fz.i >> 16));       \
}

#define JS_NEWVECTOR2(vec)                                          \
{                                                                   \
    JSObject *vobj;                                                 \
    if(!(vobj = JS_NewObject(cx, &Vector_class, NULL, NULL)))       \
        return JS_FALSE;                                            \
    JS_SETVECTOR(vobj, vec);                                        \
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(vobj));                     \
}

#define JS_NEWVECTORPOOL(vec)                                       \
{                                                                   \
    JSObject *vobj;                                                 \
    if(!(vobj = JPool_GetFree(&objPoolVector, &Vector_class)))      \
        return JS_FALSE;                                            \
    JS_SETVECTOR(vobj, vec);                                        \
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(vobj));                     \
}

#define JS_GETQUATERNION2(obj, vec)                 \
{                                                   \
    jsval xval, yval, zval, wval;                   \
    jsdouble x, y, z, w;                            \
    if(!JS_GetProperty(cx, obj, "x", &xval))        \
        return JS_FALSE;                            \
    if(!JS_GetProperty(cx, obj, "y", &yval))        \
        return JS_FALSE;                            \
    if(!JS_GetProperty(cx, obj, "z", &zval))        \
        return JS_FALSE;                            \
    if(!JS_GetProperty(cx, obj, "w", &wval))        \
        return JS_FALSE;                            \
    if(!JS_ValueToNumber(cx, xval, &x))             \
        return JS_FALSE;                            \
    if(!JS_ValueToNumber(cx, yval, &y))             \
        return JS_FALSE;                            \
    if(!JS_ValueToNumber(cx, zval, &z))             \
        return JS_FALSE;                            \
    if(!JS_ValueToNumber(cx, wval, &w))             \
        return JS_FALSE;                            \
    vec[0] = (float)x;                              \
    vec[1] = (float)y;                              \
    vec[2] = (float)z;                              \
    vec[3] = (float)w;                              \
}

#define JS_SETQUATERNION(obj, rot)                      \
{                                                       \
    jsval val;                                          \
    if(!JS_NewDoubleValue(cx, rot[0], &val))            \
        return JS_FALSE;                                \
    if(!JS_SetProperty(cx, obj, "x", &val))             \
        return JS_FALSE;                                \
    if(!JS_NewDoubleValue(cx, rot[1], &val))            \
        return JS_FALSE;                                \
    if(!JS_SetProperty(cx, obj, "y", &val))             \
        return JS_FALSE;                                \
    if(!JS_NewDoubleValue(cx, rot[2], &val))            \
        return JS_FALSE;                                \
    if(!JS_SetProperty(cx, obj, "z", &val))             \
        return JS_FALSE;                                \
    if(!JS_NewDoubleValue(cx, rot[3], &val))            \
        return JS_FALSE;                                \
    if(!JS_SetProperty(cx, obj, "w", &val))             \
        return JS_FALSE;                                \
}

#define JS_NEWQUATERNION(rot)                                       \
{                                                                   \
    JSObject *vobj;                                                 \
    if(!(vobj = JS_NewObject(cx, &Quaternion_class, NULL, NULL)))   \
        return JS_FALSE;                                            \
    JS_SETQUATERNION(vobj, rot);                                    \
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(vobj));                     \
}

#define JS_NEWQUATERNIONPOOL(rot)                                       \
{                                                                       \
    JSObject *vobj;                                                     \
    if(!(vobj = JPool_GetFree(&objPoolQuaternion, &Quaternion_class)))  \
        return JS_FALSE;                                                \
    JS_SETQUATERNION(vobj, rot);                                        \
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(vobj));                         \
}

#define JS_GETMATRIX(mtx, v, a)                                                     \
{                                                                                   \
    JSObject *vobj; if(!JS_ValueToObject(cx, v[a], &vobj)) return JS_FALSE;         \
    if(JSVAL_IS_NULL(v[a])) return JS_FALSE;                                        \
    if(!(JS_InstanceOf(cx, vobj, &Matrix_class, NULL))) return JS_FALSE;            \
    if(!(mtx = (mtx_t*)JS_GetInstancePrivate(cx, vobj, &Matrix_class, NULL)))       \
        return JS_FALSE;                                                            \
}

#define JS_GETNETEVENT(obj) JS_GET_PRIVATE_DATA(obj, &NetEvent_class, ENetEvent, ev)
#define JS_GETNETPEER(obj)  JS_GET_PRIVATE_DATA(obj, &Peer_class, ENetPeer, peer)
#define JS_GETPACKET(obj)   JS_GET_PRIVATE_DATA(obj, &Packet_class, ENetPacket, packet)
#define JS_GETHOST(obj)     JS_GET_PRIVATE_DATA(obj, &Host_class, ENetHost, host)

#define JS_CHECKNUMBER(a)                                                           \
    if(!JSVAL_IS_INT(v[a]) && !JSVAL_IS_DOUBLE(v[a]))                               \
        return JS_FALSE

#define JS_GETNUMBER(val, v, a)                                                     \
    if(JSVAL_IS_NULL(v[a]))                                                         \
        return JS_FALSE;                                                            \
    JS_ValueToNumber(cx, v[a], &val)

#define JS_CHECKINTEGER(a)                                                          \
    if(!JSVAL_IS_INT(v[a]))                                                         \
        return JS_FALSE

#define JS_GETINTEGER(val, a)                                                       \
    JS_CHECKINTEGER(a);                                                             \
    val = JSVAL_TO_INT(v[a])

#define JS_GETBOOL(val, v, a)                                                       \
    JS_ValueToBoolean(cx, v[a], &val);                                              \
    if(JSVAL_IS_NULL(v[a]))                                                         \
        return JS_FALSE

#define JS_GETOBJECT(val, v, a)                                                     \
    JS_ValueToObject(cx, v[a], &val);                                               \
    if(JSVAL_IS_NULL(v[a]))                                                         \
        return JS_FALSE

#define JS_GETSTRING(str, bytes, v, a)                                              \
    if(!(str = JS_ValueToString(cx, v[a])) ||                                       \
        !(bytes = JS_EncodeString(cx, str)))                                        \
        return JS_FALSE

#define JS_THISVECTOR(vec)                                                          \
{                                                                                   \
    JSObject *obj = JS_THIS_OBJECT(cx, vp);                                         \
    JS_GETVECTOR2(obj, vec);                                                        \
}

#define JS_THISMATRIX(mtx, v)                                                       \
    if(!(mtx = (mtx_t*)JS_GetInstancePrivate(cx, JS_THIS_OBJECT(cx, v),             \
        &Matrix_class, NULL)))                                                      \
        return JS_FALSE

#define JS_THISPLANE(pl, v)                                                         \
    if(!(pl = (plane_t*)JS_GetInstancePrivate(cx, JS_THIS_OBJECT(cx, v),            \
        &Plane_class, NULL)))                                                       \
        return JS_FALSE

#define JS_INSTVECTOR(c, vp, vec)                                                   \
{                                                                                   \
    JSObject *nobj;                                                                 \
    if(!(nobj = JS_NewObject(cx, &Vector_class, NULL, c)))                          \
        return JS_FALSE;                                                            \
    if(!(JS_SetPrivate(cx, nobj, vec)))                                             \
        return JS_FALSE;                                                            \
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(nobj));                                     \
}

#define JS_INSTQUATERNION(c, vp, rot)                                               \
{                                                                                   \
    JSObject *nobj;                                                                 \
    if(!(nobj = JS_NewObject(cx, &Quaternion_class, NULL, c)))                      \
        return JS_FALSE;                                                            \
    if(!(JS_SetPrivate(cx, nobj, rot)))                                             \
        return JS_FALSE;                                                            \
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(nobj));                                     \
}

#define JS_INSTPLANE(vp, pl)                                                        \
{                                                                                   \
    JSObject *nobj;                                                                 \
    if(!(nobj = JS_NewObject(cx, &Plane_class, NULL, NULL)))                        \
        return JS_FALSE;                                                            \
    if(!(JS_SetPrivate(cx, nobj, pl)))                                              \
        return JS_FALSE;                                                            \
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(nobj));                                     \
}

#define JS_NEWOBJECT_SETPRIVATE(data, class)                                        \
{                                                                                   \
    JSObject *object;                                                               \
    if(!(object = JS_NewObject(cx, class, NULL, NULL)) ||                           \
        !(JS_SetPrivate(cx, object, data)))                                         \
    {                                                                               \
        return JS_FALSE;                                                            \
    }                                                                               \
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(object));                                   \
}

#define JS_NEWOBJECTPOOL(data, class)                                               \
{                                                                                   \
    JSObject *object;                                                               \
    if(!(object = JPool_GetFree(&objPool ##class, &class ## _class)) ||             \
        !(JS_SetPrivate(cx, object, data)))                                         \
    {                                                                               \
        return JS_FALSE;                                                            \
    }                                                                               \
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(object));                                   \
}

#define JS_CHECKARGS(n)                                                             \
    jsval *v = JS_ARGV(cx, vp);                                                     \
    if(argc != n) return JS_FALSE

#define JS_ARG(a) v[a]

#define JS_RETURNOBJECT(vp)                                                         \
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(JS_THIS_OBJECT(cx, vp)))

#define JS_RETURNSTRING(vp, string)                                                 \
    JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, string)))

#define JS_RETURNBOOLEAN(vp, boolean)                                               \
    JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(boolean))

JS_EXTERNOBJECT(Sys);
JS_EXTERNOBJECT(NInput);
JS_EXTERNOBJECT(GL);
JS_EXTERNOBJECT(Net);
JS_EXTERNOBJECT(NClient);
JS_EXTERNOBJECT(NServer);
JS_EXTERNOBJECT(NRender);
JS_EXTERNOBJECT(NGame);
JS_EXTERNOBJECT(Angle);
JS_EXTERNOBJECT(Physics);
JS_EXTERNOBJECT(Level);
JS_EXTERNCLASS(Vector);
JS_EXTERNCLASS(Quaternion);
JS_EXTERNCLASS(Matrix);
JS_EXTERNCLASS(AnimState);
JS_EXTERNCLASS(Canvas);
JS_EXTERNCLASS(Font);
JS_EXTERNCLASS(Controller);
JS_EXTERNOBJECT(Snd);
JS_EXTERNCLASS_NOCONSTRUCTOR(NetEvent);
JS_EXTERNCLASS_NOCONSTRUCTOR(Packet);
JS_EXTERNCLASS_NOCONSTRUCTOR(Peer);
JS_EXTERNCLASS_NOCONSTRUCTOR(Host);
JS_EXTERNCLASS_NOCONSTRUCTOR(Model);
JS_EXTERNCLASS_NOCONSTRUCTOR(Animation);
JS_EXTERNCLASS_NOCONSTRUCTOR(Texture);
JS_EXTERNCLASS_NOCONSTRUCTOR(Plane);
JS_EXTERNCLASS_NOCONSTRUCTOR(GameActor);
JS_EXTERNCLASS_NOCONSTRUCTOR(Component);
JS_EXTERNCLASS_NOCONSTRUCTOR(InputEvent);

#endif

