namespace Assets.Scripts
{
    partial class TestForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.lb_position = new System.Windows.Forms.Label();
            this.lb_scale = new System.Windows.Forms.Label();
            this.tb_position_x = new System.Windows.Forms.TextBox();
            this.tb_position_y = new System.Windows.Forms.TextBox();
            this.tb_position_z = new System.Windows.Forms.TextBox();
            this.tb_scale_z = new System.Windows.Forms.TextBox();
            this.tb_scale_y = new System.Windows.Forms.TextBox();
            this.tb_scale_x = new System.Windows.Forms.TextBox();
            this.bu_create_cube = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // lb_position
            // 
            this.lb_position.AutoSize = true;
            this.lb_position.Location = new System.Drawing.Point(12, 14);
            this.lb_position.Name = "lb_position";
            this.lb_position.Size = new System.Drawing.Size(43, 13);
            this.lb_position.TabIndex = 0;
            this.lb_position.Text = "position";
            // 
            // lb_scale
            // 
            this.lb_scale.AutoSize = true;
            this.lb_scale.Location = new System.Drawing.Point(12, 40);
            this.lb_scale.Name = "lb_scale";
            this.lb_scale.Size = new System.Drawing.Size(32, 13);
            this.lb_scale.TabIndex = 1;
            this.lb_scale.Text = "scale";
            // 
            // tb_position_x
            // 
            this.tb_position_x.Location = new System.Drawing.Point(61, 11);
            this.tb_position_x.Name = "tb_position_x";
            this.tb_position_x.Size = new System.Drawing.Size(60, 20);
            this.tb_position_x.TabIndex = 2;
            this.tb_position_x.Text = "0";
            this.tb_position_x.TextChanged += new System.EventHandler(this.OnPositionTextChange);
            // 
            // tb_position_y
            // 
            this.tb_position_y.Location = new System.Drawing.Point(127, 11);
            this.tb_position_y.Name = "tb_position_y";
            this.tb_position_y.Size = new System.Drawing.Size(60, 20);
            this.tb_position_y.TabIndex = 3;
            this.tb_position_y.Text = "0";
            this.tb_position_y.TextChanged += new System.EventHandler(this.OnPositionTextChange);
            // 
            // tb_position_z
            // 
            this.tb_position_z.Location = new System.Drawing.Point(193, 12);
            this.tb_position_z.Name = "tb_position_z";
            this.tb_position_z.Size = new System.Drawing.Size(60, 20);
            this.tb_position_z.TabIndex = 4;
            this.tb_position_z.Text = "0";
            this.tb_position_z.TextChanged += new System.EventHandler(this.OnPositionTextChange);
            // 
            // tb_scale_z
            // 
            this.tb_scale_z.Location = new System.Drawing.Point(193, 38);
            this.tb_scale_z.Name = "tb_scale_z";
            this.tb_scale_z.Size = new System.Drawing.Size(60, 20);
            this.tb_scale_z.TabIndex = 7;
            this.tb_scale_z.Text = "1";
            this.tb_scale_z.TextChanged += new System.EventHandler(this.OnScaleTextChange);
            // 
            // tb_scale_y
            // 
            this.tb_scale_y.Location = new System.Drawing.Point(127, 37);
            this.tb_scale_y.Name = "tb_scale_y";
            this.tb_scale_y.Size = new System.Drawing.Size(60, 20);
            this.tb_scale_y.TabIndex = 6;
            this.tb_scale_y.Text = "1";
            this.tb_scale_y.TextChanged += new System.EventHandler(this.OnScaleTextChange);
            // 
            // tb_scale_x
            // 
            this.tb_scale_x.Location = new System.Drawing.Point(61, 37);
            this.tb_scale_x.Name = "tb_scale_x";
            this.tb_scale_x.Size = new System.Drawing.Size(60, 20);
            this.tb_scale_x.TabIndex = 5;
            this.tb_scale_x.Text = "1";
            this.tb_scale_x.TextChanged += new System.EventHandler(this.OnScaleTextChange);
            // 
            // bu_create_cube
            // 
            this.bu_create_cube.Location = new System.Drawing.Point(15, 81);
            this.bu_create_cube.Name = "bu_create_cube";
            this.bu_create_cube.Size = new System.Drawing.Size(75, 23);
            this.bu_create_cube.TabIndex = 8;
            this.bu_create_cube.Text = "Create Cube";
            this.bu_create_cube.UseVisualStyleBackColor = true;
            this.bu_create_cube.Click += new System.EventHandler(this.CreateCube);
            // 
            // TestForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(284, 261);
            this.Controls.Add(this.bu_create_cube);
            this.Controls.Add(this.tb_scale_z);
            this.Controls.Add(this.tb_scale_y);
            this.Controls.Add(this.tb_scale_x);
            this.Controls.Add(this.tb_position_z);
            this.Controls.Add(this.tb_position_y);
            this.Controls.Add(this.tb_position_x);
            this.Controls.Add(this.lb_scale);
            this.Controls.Add(this.lb_position);
            this.Name = "TestForm";
            this.Text = "TestForm";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label lb_position;
        private System.Windows.Forms.Label lb_scale;
        private System.Windows.Forms.TextBox tb_position_x;
        private System.Windows.Forms.TextBox tb_position_y;
        private System.Windows.Forms.TextBox tb_position_z;
        private System.Windows.Forms.TextBox tb_scale_z;
        private System.Windows.Forms.TextBox tb_scale_y;
        private System.Windows.Forms.TextBox tb_scale_x;
        private System.Windows.Forms.Button bu_create_cube;
    }
}