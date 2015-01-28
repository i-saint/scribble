using UnityEngine;
using System.Collections;
using System.Windows.Forms;

public class FormTest : MonoBehaviour
{
    Assets.Scripts.TestForm m_form;

    void OnDestroy()
    {
        if (m_form != null)
        {
            m_form.Close();
            m_form = null;
        }
    }


    void Update()
    {
        if (m_form != null && m_form.IsDisposed)
        {
            m_form = null;
        }
        if (m_form != null)
        {
            m_form.UpdateText();
        }
    }

    void OnGUI()
    {
        if (m_form==null && GUI.Button(new Rect(5.0f, 5.0f, 105.0f, 25.0f), "Show Form"))
        {
            m_form = new Assets.Scripts.TestForm();
            m_form.Show();
        }
    }
}
