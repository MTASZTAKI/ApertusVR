# HandleScope

The HandleScope class is used to manage the lifetime of object handles
which are created through the use of node-addon-api. These handles
keep an object alive in the heap in order to ensure that the objects
are not collected while native code is using them.
A handle may be created when any new node-addon-api Value or one
of its subclasses is created or returned. For more details refer to
the section titled (Object lifetime management)[object_lifetime_management].

## Methods

### Constructor

Creates a new handle scope.

```cpp
HandleScope HandleScope::New(Napi:Env env);
```

- `[in] Env`: The environment in which to construct the HandleScope object.

Returns a new HandleScope


### Constructor

Creates a new handle scope.

```cpp
HandleScope HandleScope::New(napi_env env, napi_handle_scope scope);
```

- `[in] env`: napi_env in which the scope passed in was created.
- `[in] scope`: pre-existing napi_handle_scope.

Returns a new HandleScope instance which wraps the napi_handle_scope
handle passed in.  This can be used to mix usage of the C N-API
and node-addon-api.

operator HandleScope::napi_handle_scope

```cpp
operator HandleScope::napi_handle_scope() const
```

Returns the N-API napi_handle_scope wrapped by the EscapableHandleScope object.
This can be used to mix usage of the C N-API and node-addon-api by allowing
the class to be used be converted to a napi_handle_scope.

### Destructor
```cpp
~HandleScope();
```

Deletes the HandleScope instance and allows any objects/handles created
in the scope to be collected by the garbage collector.  There is no
guarantee as to when the gargbage collector will do this.

### Env

```cpp
Napi::Env Env() const;
```

Returns the Napi:Env associated with the HandleScope.
