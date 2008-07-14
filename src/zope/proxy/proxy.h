#ifndef _proxy_H_
#define _proxy_H_ 1

#if PY_VERSION_HEX < 0x02050000 && !defined(PY_SSIZE_T_MIN)
typedef int Py_ssize_t;
#define PY_SSIZE_T_MAX INT_MAX
#define PY_SSIZE_T_MIN INT_MIN
typedef Py_ssize_t (*lenfunc)(PyObject *);
typedef PyObject *(*ssizeargfunc)(PyObject *, Py_ssize_t);
typedef PyObject *(*ssizessizeargfunc)(PyObject *, Py_ssize_t, Py_ssize_t);
typedef int(*ssizeobjargproc)(PyObject *, Py_ssize_t, PyObject *);
typedef int(*ssizessizeobjargproc)(PyObject *, Py_ssize_t, Py_ssize_t, PyObject *);
#endif

typedef struct {
    PyObject_HEAD
    PyObject *proxy_object;
} ProxyObject;

#define Proxy_GET_OBJECT(ob)   (((ProxyObject *)(ob))->proxy_object)

typedef struct {
    PyTypeObject *proxytype;
    int (*check)(PyObject *obj);
    PyObject *(*create)(PyObject *obj);
    PyObject *(*getobject)(PyObject *proxy);
} ProxyInterface;


#ifndef PROXY_MODULE

/* These are only defined in the public interface, and are not
 * available within the module implementation.  There we use the
 * classic Python/C API only.
 */

static ProxyInterface *_proxy_api = NULL;

static int
Proxy_Import(void)
{
    if (_proxy_api == NULL) {
        PyObject *m = PyImport_ImportModule("zope.proxy");
        if (m != NULL) {
            PyObject *tmp = PyObject_GetAttrString(m, "_CAPI");
            if (tmp != NULL) {
                if (PyCObject_Check(tmp))
                    _proxy_api = (ProxyInterface *)
                        PyCObject_AsVoidPtr(tmp);
                Py_DECREF(tmp);
            }
        }
    }
    return (_proxy_api == NULL) ? -1 : 0;
}

#define ProxyType               (*_proxy_api->proxytype)
#define Proxy_Check(obj)        (_proxy_api->check((obj)))
#define Proxy_CheckExact(obj)   ((obj)->ob_type == ProxyType)
#define Proxy_New(obj)          (_proxy_api->create((obj)))
#define Proxy_GetObject(proxy)  (_proxy_api->getobject((proxy)))

#endif /* PROXY_MODULE */

#endif /* _proxy_H_ */
