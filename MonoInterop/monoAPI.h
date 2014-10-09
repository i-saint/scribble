#ifndef monoAPI_h
#define monoAPI_h

#pragma comment(lib,"mono.lib")
#define MONO_API __declspec(dllimport)

extern "C" {

typedef int                 gboolean;
typedef void*               gpointer;
typedef const void*         gconstpointer;
typedef __int8              gint8;
typedef unsigned __int8     guint8;
typedef __int16             gint16;
typedef unsigned __int16    guint16;
typedef __int32             gint32;
typedef unsigned __int32    guint32;
typedef __int64             gint64;
typedef unsigned __int64    guint64;
typedef float               gfloat;
typedef double              gdouble;
typedef unsigned __int16    gunichar2;

typedef void* MonoDomain;
typedef void* MonoAssembly;
typedef void* MonoImage;
typedef void* MonoType;
typedef void* MonoClass;
typedef void* MonoClassField;
typedef void* MonoProperty;
typedef void* MonoObject;
typedef void* MonoMethod;
typedef void* MonoMethodDesc;
typedef void* MonoMethodSignature;
typedef void* MonoString;

MONO_API MonoDomain*    mono_domain_get();
MONO_API MonoAssembly*  mono_domain_assembly_open(MonoDomain *domain, const char *assemblyName);
MONO_API MonoImage*     mono_assembly_get_image(MonoAssembly *assembly);
MONO_API MonoString*    mono_string_new(MonoDomain *domain, const char *text);
MONO_API void           mono_add_internal_call(const char *name, gconstpointer method);

MONO_API const char*            mono_type_get_name(MonoType *type);

MONO_API const char*            mono_field_get_name(MonoClassField *field);
MONO_API MonoType*              mono_field_get_type(MonoClassField *field);
MONO_API void                   mono_field_get_value(MonoObject *obj, MonoClassField *field, void *value);
MONO_API void                   mono_field_set_value(MonoObject *obj, MonoClassField *field, void *value);

MONO_API const char*            mono_property_get_name(MonoProperty *prop);
MONO_API MonoMethod*            mono_property_get_set_method(MonoProperty *prop);
MONO_API MonoMethod*            mono_property_get_get_method(MonoProperty *prop);

MONO_API const char*            mono_method_get_name(MonoMethod *method);
MONO_API MonoMethodDesc*        mono_method_desc_new(const char *methodString, gboolean useNamespace);
MONO_API MonoMethodDesc*        mono_method_desc_free(MonoMethodDesc *desc);
MONO_API MonoMethod*            mono_method_desc_search_in_image(MonoMethodDesc *methodDesc, MonoImage *image);
MONO_API MonoMethodSignature*   mono_method_signature(MonoMethod *m);
MONO_API MonoObject*            mono_runtime_invoke(MonoMethod *method, void *obj, void **params, MonoObject **exc);

MONO_API MonoClass*             mono_class_from_name(MonoImage *image, const char *namespaceString, const char *classnameString);
MONO_API const char*            mono_class_get_name(MonoClass *klass);
MONO_API MonoType*              mono_class_get_type(MonoClass *klass);
MONO_API MonoMethod*            mono_class_get_methods(MonoClass*, gpointer* iter);
MONO_API MonoMethod*            mono_class_get_method_from_name(MonoClass *klass, const char *name, int param_count);
MONO_API MonoClassField*        mono_class_get_field_from_name(MonoClass *klass, const char *name);
MONO_API MonoProperty*          mono_class_get_property_from_name(MonoClass *klass, const char *name);

MONO_API guint32                mono_signature_get_param_count(MonoMethodSignature *sig);

MONO_API MonoDomain*            mono_object_get_domain(MonoObject *obj);
MONO_API MonoClass*             mono_object_get_class(MonoObject *obj);


} // extern "C"
#endif // monoAPI_h
