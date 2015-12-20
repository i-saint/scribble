using UnityEngine;
using System;
using System.Collections;
using System.Runtime.InteropServices;


public class Memcpy : MonoBehaviour
{
    public delegate void ValueCopyT(ref Matrix4x4 dst, ref Matrix4x4 src, int size);
    public delegate void ArrayCopyT(System.Array dst, System.Array src, int size);
    byte[] m_code_rep_movs_copy_value;
    byte[] m_code_rep_movs_copy_array;
    byte[] m_code_sse_copy_array;

    public ValueCopyT rep_movs_copy_value;
    public ArrayCopyT rep_movs_copy_array;
    public ArrayCopyT sse_copy_array;

    public int m_data_num = 1024 * 256;
    public int m_num_try = 256;
    Matrix4x4[] m_testdata_src;
    Matrix4x4[] m_testdata_dst;

    void Awake()
    {
        m_code_rep_movs_copy_value = new byte[] { 0x8B, 0x4C, 0x24, 0x0C, 0xC1, 0xF9, 0x02, 0x56, 0x8B, 0x74, 0x24, 0x0C, 0x57, 0x8B, 0x7C, 0x24, 0x0C, 0xF3, 0xA5, 0x5F, 0x5E, 0xC3 };
        rep_movs_copy_value = (ValueCopyT)Marshal.GetDelegateForFunctionPointer(Marshal.UnsafeAddrOfPinnedArrayElement(m_code_rep_movs_copy_value, 0), typeof(ValueCopyT));

        m_code_rep_movs_copy_array = new byte[] { 0x8B, 0x4C, 0x24, 0x0C, 0xC1, 0xF9, 0x02, 0x56, 0x8B, 0x74, 0x24, 0x0C, 0x57, 0x8B, 0x7C, 0x24, 0x0C, 0x83, 0xC6, 0x10, 0x83, 0xC7, 0x10, 0xF3, 0xA5, 0x5F, 0x5E, 0xC3 };
        rep_movs_copy_array = (ArrayCopyT)Marshal.GetDelegateForFunctionPointer(Marshal.UnsafeAddrOfPinnedArrayElement(m_code_rep_movs_copy_array, 0), typeof(ArrayCopyT));


        m_code_sse_copy_array = new byte[] { 0x83, 0xEC, 0x10, 0x8B, 0x4C, 0x24, 0x14, 0xBA, 0x10, 0x00, 0x00, 0x00, 0x56, 0x57, 0x8B, 0x7C, 0x24, 0x20, 0x83, 0xC1, 0x10, 0x83, 0xC7, 0x10, 0x89, 0x4C, 0x24, 0x1C, 0x8D, 0x71, 0x0F, 0x8D, 0x47, 0x0F, 0x83, 0xE6, 0xF0, 0x83, 0xE0, 0xF0, 0x89, 0x74, 0x24, 0x14, 0x89, 0x44, 0x24, 0x10, 0x33, 0xC0, 0x3B, 0xF1, 0x0F, 0x45, 0xC2, 0x8B, 0x54, 0x24, 0x24, 0x2B, 0xD0, 0xC1, 0xEA, 0x07, 0x89, 0x54, 0x24, 0x08, 0x85, 0xD2, 0x0F, 0x84, 0x97, 0x00, 0x00, 0x00, 0x8B, 0x44, 0x24, 0x10, 0x8D, 0x4E, 0x30, 0x89, 0x74, 0x24, 0x20, 0x83, 0xC0, 0x20, 0x8B, 0x74, 0x24, 0x10, 0x29, 0x74, 0x24, 0x20, 0x8B, 0x74, 0x24, 0x14, 0x89, 0x54, 0x24, 0x0C, 0x8B, 0x54, 0x24, 0x20, 0x8B, 0xFF, 0xFF, 0x4C, 0x24, 0x0C, 0x8D, 0x80, 0x80, 0x00, 0x00, 0x00, 0x66, 0x0F, 0x6F, 0x88, 0x70, 0xFF, 0xFF, 0xFF, 0x8D, 0x89, 0x80, 0x00, 0x00, 0x00, 0x66, 0x0F, 0x6F, 0x50, 0x80, 0x66, 0x0F, 0x6F, 0x58, 0x90, 0x66, 0x0F, 0x6F, 0x80, 0x60, 0xFF, 0xFF, 0xFF, 0x66, 0x0F, 0x7F, 0x81, 0x50, 0xFF, 0xFF, 0xFF, 0x66, 0x0F, 0x7F, 0x89, 0x60, 0xFF, 0xFF, 0xFF, 0x66, 0x0F, 0x7F, 0x54, 0x02, 0x80, 0x66, 0x0F, 0x7F, 0x59, 0x80, 0x66, 0x0F, 0x6F, 0x48, 0xB0, 0x66, 0x0F, 0x6F, 0x50, 0xC0, 0x66, 0x0F, 0x6F, 0x58, 0xD0, 0x66, 0x0F, 0x6F, 0x40, 0xA0, 0x66, 0x0F, 0x7F, 0x41, 0x90, 0x66, 0x0F, 0x7F, 0x49, 0xA0, 0x66, 0x0F, 0x7F, 0x51, 0xB0, 0x66, 0x0F, 0x7F, 0x59, 0xC0, 0x75, 0x91, 0x8B, 0x4C, 0x24, 0x1C, 0x2B, 0x74, 0x24, 0x1C, 0x33, 0xD2, 0x8B, 0x44, 0x24, 0x14, 0x83, 0xC6, 0x03, 0xC1, 0xEE, 0x02, 0x39, 0x44, 0x24, 0x1C, 0x0F, 0x47, 0xF2, 0x85, 0xF6, 0x74, 0x16, 0x2B, 0x7C, 0x24, 0x1C, 0x8B, 0x04, 0x0F, 0x8D, 0x49, 0x04, 0x42, 0x89, 0x41, 0xFC, 0x3B, 0xD6, 0x72, 0xF2, 0x8B, 0x44, 0x24, 0x14, 0x8B, 0x4C, 0x24, 0x08, 0x33, 0xD2, 0x8B, 0x7C, 0x24, 0x10, 0xC1, 0xE1, 0x07, 0x03, 0xC1, 0x03, 0xF9, 0x8B, 0x4C, 0x24, 0x1C, 0x03, 0x4C, 0x24, 0x24, 0x8B, 0xF1, 0x2B, 0xF0, 0x83, 0xC6, 0x03, 0xC1, 0xEE, 0x02, 0x3B, 0xC1, 0x0F, 0x47, 0xF2, 0x85, 0xF6, 0x74, 0x10, 0x2B, 0xF8, 0x8B, 0x0C, 0x07, 0x8D, 0x40, 0x04, 0x42, 0x89, 0x48, 0xFC, 0x3B, 0xD6, 0x72, 0xF2, 0x5F, 0x5E, 0x83, 0xC4, 0x10, 0xC3 };
        sse_copy_array = (ArrayCopyT)Marshal.GetDelegateForFunctionPointer(Marshal.UnsafeAddrOfPinnedArrayElement(m_code_sse_copy_array, 0), typeof(ArrayCopyT));


        Test_CopyArray("PlainCopyArray", () => { PlainCopyArray(); });
        Test_CopyArray("RepMovsCopyArray", () => { RepMovsCopyArray(); });
        Test_CopyArray("SSECopyArray", () => { SSECopyArray(); });

        Test_CopyValues("PlainCopyValues", () => { PlainCopyValues(); });
        Test_CopyValues("RepMovsCopyValues", () => { RepMovsCopyValues(); });
    }

    void Test_CopyArray(string test_name, Action act)
    {
        // reset test data
        m_testdata_src = new Matrix4x4[m_data_num];
        m_testdata_dst = new Matrix4x4[m_data_num];
        for (int i = 0; i < m_data_num; ++i)
        {
            m_testdata_src[i].m03 = i;
        }

        float begin_time = Time.realtimeSinceStartup;
        for (int i = 0; i < m_num_try; ++i)
        {
            act.Invoke();
        }
        float elapsed = Time.realtimeSinceStartup - begin_time;

        Debug.Log("Test: " + test_name + "\n" + "elapsed: " + elapsed + " sec");

        // error check
        for (int i = 0; i < m_data_num; ++i)
        {
            if (m_testdata_dst[i].m03 != m_testdata_src[i].m03)
            {
                Debug.Log("data mismatch!");
                break;
            }
        }
    }

    void PlainCopyArray()
    {
        for (int i = 0; i < m_data_num; ++i)
        {
            m_testdata_dst[i] = m_testdata_src[i];
        }
    }

    void RepMovsCopyArray()
    {
        rep_movs_copy_array(m_testdata_dst, m_testdata_src, m_data_num * 64);
    }

    void SSECopyArray()
    {
        sse_copy_array(m_testdata_dst, m_testdata_src, m_data_num * 64);
    }



    void Test_CopyValues(string test_name, Action act)
    {
        // reset test data
        m_testdata_src = new Matrix4x4[m_data_num];
        m_testdata_dst = new Matrix4x4[m_data_num];
        m_testdata_src[0].m03 = 100.0f;

        float begin_time = Time.realtimeSinceStartup;
        for (int i = 0; i < m_num_try; ++i)
        {
            act.Invoke();
        }
        float elapsed = Time.realtimeSinceStartup - begin_time;

        Debug.Log("Test: " + test_name + "\n" + "elapsed: " + elapsed + " sec");

        // error check
        for (int i = 0; i < m_data_num; ++i)
        {
            if (m_testdata_dst[i].m03 != m_testdata_src[0].m03)
            {
                Debug.Log("data mismatch!");
                break;
            }
        }
    }

    void PlainCopyValues()
    {
        Matrix4x4 r = m_testdata_src[0];
        for (int i = 0; i < m_data_num; ++i)
        {
            m_testdata_dst[i] = r;
        }
    }

    void RepMovsCopyValues()
    {
        Matrix4x4 r = m_testdata_src[0];
        for (int i = 0; i < m_data_num; ++i)
        {
            rep_movs_copy_value(ref m_testdata_dst[i], ref r, 64);
        }
    }
}

/*
result: (on my machine)

 * Test: PlaneCopy
elapsed: 1.280228 sec

Test: RepMovsCopy
elapsed: 0.2081757 sec

Test: SSECopy
elapsed: 0.2939224 sec

*/



/*
 * rep_movs_copy_value:
mov         ecx,dword ptr [esp+12h]  
sar         ecx,2  
push        esi  
mov         esi,dword ptr [esp+0Ch]  
push        edi  
mov         edi,dword ptr [esp+0Ch]  
rep movs    dword ptr es:[edi],dword ptr [esi]  
pop         edi  
pop         esi  
ret
*/

/*
 * rep_movs_copy_array:
mov         ecx,dword ptr [esp+12h]  
sar         ecx,2  
push        esi  
mov         esi,dword ptr [esp+0Ch]  
push        edi  
mov         edi,dword ptr [esp+0Ch]  
add         esi,10h  
add         edi,10h  
rep movs    dword ptr es:[edi],dword ptr [esi]  
pop         edi  
pop         esi  
ret
*/

/*
 * sse_copy_array:
__forceinline void actual_copy(uint128 *dst_, uint128 *src_, size_t size_byte)
{
    uint128 *dst = (uint128*)(((size_t)dst_ + 0xF) & ~0x0F);
    uint128 *src = (uint128*)(((size_t)src_ + 0xF) & ~0x0F);

    size_t n = (size_byte - (dst != dst_ ? 16 : 0)) >> 4;
    size_t blocks = n >> 3;
    for (size_t i = 0; i < blocks; ++i) {
        uint128 *d = dst + (i * 8);
        uint128 *s = src + (i * 8);
        uint128 t0 = s[0];
        uint128 t1 = s[1];
        uint128 t2 = s[2];
        uint128 t3 = s[3];
        d[0] = t0;
        d[1] = t1;
        d[2] = t2;
        d[3] = t3;
        t0 = s[4];
        t1 = s[5];
        t2 = s[6];
        t3 = s[7];
        d[4] = t0;
        d[5] = t1;
        d[6] = t2;
        d[7] = t3;
    }

    int *cdst = (int*)dst_;
    int *csrc = (int*)src_;
    int *cdst_end = (int*)dst;
    while (cdst < cdst_end) { *(cdst++) = *(csrc++); }
    cdst = (int*)(dst + blocks*8);
    csrc = (int*)(src + blocks*8);
    cdst_end = (int*)((char*)dst_ + size_byte);
    while (cdst < cdst_end) { *(cdst++) = *(csrc++); }
}

void sse_copy(MonoArray *dst, MonoArray *src, int size)
{
    char *d = (char*)dst->vector;
    char *s = (char*)src->vector;
    actual_copy((uint128*)d, (uint128*)s, size);
}

*/