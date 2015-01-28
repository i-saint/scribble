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

    }
}
