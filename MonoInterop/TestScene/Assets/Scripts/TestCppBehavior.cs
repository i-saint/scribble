using UnityEngine;
using System;
using System.Collections;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;


public class mio
{
    [DllImport("mio32")]
    public static extern void mioInitialize();
}

public class TestCppBehavior : MonoBehaviour
{
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public extern IntPtr ctor(MonoBehaviour obj);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public extern void dtor(IntPtr o);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public extern void start();

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public extern void update();


    IntPtr cppobj;

    void OnEnable()
    {
        mio.mioInitialize();
        cppobj = ctor(this);
    }

    void OnDisable()
    {
        dtor(cppobj);
    }

    void Start()
    {
        start();
    }
    
    void Update()
    {
        update();
    }

    void ThisFunctionWillBeCalledFromCpp()
    {
        Debug.Log("ThisFunctionWillBeCalledFromCpp()");
    }
}
