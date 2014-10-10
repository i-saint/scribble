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

public class TestCppBehaviour : MonoBehaviour
{
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public extern IntPtr ctor(MonoBehaviour obj);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public extern void dtor(IntPtr o);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public extern void start();

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public extern void update();


    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public extern int memfn1(int a1);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public extern int memfn2(int a1, int a2);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public extern int cmemfn1(int a1);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public extern int cmemfn2(int a1, int a2);


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
        memfn1(1);
        memfn2(1, 2);
        cmemfn1(1);
        cmemfn2(1, 2);
    }

    void ThisFunctionWillBeCalledFromCpp()
    {
        Debug.Log("ThisFunctionWillBeCalledFromCpp()");
    }
}
