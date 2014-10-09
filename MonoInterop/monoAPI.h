#ifndef monoAPI_h
#define monoAPI_h

#pragma comment(lib,"mono.lib")
#define MONO_API __declspec(dllimport)

extern "C" {

typedef void* MonoDomain;
typedef void* MonoAssembly;
typedef void* MonoImage;
typedef void* MonoType;
typedef void* MonoClass;
typedef void* MonoClassField;
typedef void* MonoObject;
typedef void* MonoMethodDesc;
typedef void* MonoMethod;
typedef void* MonoString;
typedef int gboolean;
typedef void* gpointer;
typedef const void* gconstpointer;

MONO_API MonoDomain* mono_domain_get();
MONO_API MonoAssembly* mono_domain_assembly_open(MonoDomain *domain, const char *assemblyName);
MONO_API MonoImage* mono_assembly_get_image(MonoAssembly *assembly);
MONO_API MonoMethodDesc* mono_method_desc_new(const char *methodString, gboolean useNamespace);
MONO_API MonoMethodDesc* mono_method_desc_free(MonoMethodDesc *desc);
MONO_API MonoMethod* mono_method_desc_search_in_image(MonoMethodDesc *methodDesc, MonoImage *image);
MONO_API MonoObject* mono_runtime_invoke(MonoMethod *method, void *obj, void **params, MonoObject **exc);
MONO_API MonoClass* mono_class_from_name(MonoImage *image, const char *namespaceString, const char *classnameString);
MONO_API MonoMethod* mono_class_get_methods(MonoClass*, gpointer* iter);
MONO_API MonoString* mono_string_new(MonoDomain *domain, const char *text);

MONO_API const char* mono_type_get_name(MonoType *type);
MONO_API const char* mono_method_get_name(MonoMethod *method);
MONO_API const char* mono_class_get_name(MonoClass *klass);
MONO_API const char* mono_field_get_name(MonoClassField *field);
MONO_API void mono_add_internal_call(const char *name, gconstpointer method);

} // extern "C"
#endif // monoAPI_h
