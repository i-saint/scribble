using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using UnityEngine;
using UnityEditor;

namespace Assets.Scripts
{
    public partial class TestForm : Form
    {
        TextBox m_drag_target;
        int m_prev_mouse_x;

        public TestForm()
        {
            InitializeComponent();
            TopMost = true;
        }

        public void UpdateText()
        {
            var obj = Selection.activeGameObject;
            if (obj == null) return;
            var trans = obj.GetComponent<Transform>();
            var pos = trans.position;
            var scale = trans.localScale;
            if (!tb_position_x.Focused) tb_position_x.Text = pos.x.ToString();
            if (!tb_position_y.Focused) tb_position_y.Text = pos.y.ToString();
            if (!tb_position_z.Focused) tb_position_z.Text = pos.z.ToString();
            if (!tb_scale_x.Focused) tb_scale_x.Text = scale.x.ToString();
            if (!tb_scale_y.Focused) tb_scale_y.Text = scale.y.ToString();
            if (!tb_scale_z.Focused) tb_scale_z.Text = scale.z.ToString();
        }

        private void CreateCube(object sender, EventArgs e)
        {
            var prefab = AssetDatabase.LoadAssetAtPath("Assets/Prefabs/Cube.prefab", typeof(GameObject));
            if (prefab != null)
            {
                GameObject.Instantiate(prefab);
            }
        }


        private void OnPositionTextChange(object sender, EventArgs e)
        {
            try
            {
                var v = new Vector3(
                    float.Parse(tb_position_x.Text),
                    float.Parse(tb_position_y.Text),
                    float.Parse(tb_position_z.Text));
                var obj = Selection.activeGameObject;
                if (obj == null) return;
                obj.GetComponent<Transform>().position = v;
            }
            catch(Exception)
            {

            }
        }

        private void OnScaleTextChange(object sender, EventArgs e)
        {
            try
            {
                var v = new Vector3(
                    float.Parse(tb_scale_x.Text),
                    float.Parse(tb_scale_y.Text),
                    float.Parse(tb_scale_z.Text));
                var obj = Selection.activeGameObject;
                if (obj == null) return;
                obj.GetComponent<Transform>().localScale = v;
            }
            catch (Exception)
            {

            }
        }
        

        private void BeginDrag(object sender, MouseEventArgs e)
        {
            m_drag_target = sender as TextBox;
            m_prev_mouse_x = e.Location.X;
        }

        private void Drag(object sender, MouseEventArgs e)
        {
            if (m_drag_target != null)
            {
                float v = float.Parse(m_drag_target.Text);
                v += (float)(e.Location.X - m_prev_mouse_x) * 0.1f;
                m_drag_target.Text = v.ToString();
            }
            m_prev_mouse_x = e.Location.X;
        }

        private void EndDrag(object sender, MouseEventArgs e)
        {
            m_drag_target = null;
            m_prev_mouse_x = e.Location.X;
        }

    }
}
