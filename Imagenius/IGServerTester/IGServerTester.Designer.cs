namespace IGServerTester
{
    partial class IGServerTester
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
            this.comboRequests = new System.Windows.Forms.ComboBox();
            this.listRequestBuffer = new System.Windows.Forms.ListBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.textTesterPort = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.buttonPing = new System.Windows.Forms.Button();
            this.buttonConnect = new System.Windows.Forms.Button();
            this.textServerPort = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.textServerIP = new System.Windows.Forms.TextBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.textBoxUser = new System.Windows.Forms.TextBox();
            this.labelUser = new System.Windows.Forms.Label();
            this.buttonAdd = new System.Windows.Forms.Button();
            this.textParam4 = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.textParam3 = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.textParam2 = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.textParam1 = new System.Windows.Forms.TextBox();
            this.labelParam1 = new System.Windows.Forms.Label();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.buttonSend = new System.Windows.Forms.Button();
            this.buttonClear = new System.Windows.Forms.Button();
            this.buttonRemove = new System.Windows.Forms.Button();
            this.label6 = new System.Windows.Forms.Label();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.richTextBoxAnswer = new System.Windows.Forms.RichTextBox();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.checkBoxProgress = new System.Windows.Forms.CheckBox();
            this.checkBoxStatus = new System.Windows.Forms.CheckBox();
            this.checkBoxHearthbeat = new System.Windows.Forms.CheckBox();
            this.buttonClearBuffer = new System.Windows.Forms.Button();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.groupBox5.SuspendLayout();
            this.SuspendLayout();
            // 
            // comboRequests
            // 
            this.comboRequests.FormattingEnabled = true;
            this.comboRequests.Location = new System.Drawing.Point(10, 19);
            this.comboRequests.Name = "comboRequests";
            this.comboRequests.Size = new System.Drawing.Size(147, 21);
            this.comboRequests.TabIndex = 0;
            // 
            // listRequestBuffer
            // 
            this.listRequestBuffer.FormattingEnabled = true;
            this.listRequestBuffer.HorizontalScrollbar = true;
            this.listRequestBuffer.Location = new System.Drawing.Point(6, 34);
            this.listRequestBuffer.Name = "listRequestBuffer";
            this.listRequestBuffer.Size = new System.Drawing.Size(402, 108);
            this.listRequestBuffer.TabIndex = 1;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.textTesterPort);
            this.groupBox1.Controls.Add(this.label7);
            this.groupBox1.Controls.Add(this.buttonPing);
            this.groupBox1.Controls.Add(this.buttonConnect);
            this.groupBox1.Controls.Add(this.textServerPort);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.textServerIP);
            this.groupBox1.Location = new System.Drawing.Point(8, 4);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(163, 121);
            this.groupBox1.TabIndex = 2;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Server";
            // 
            // textTesterPort
            // 
            this.textTesterPort.Location = new System.Drawing.Point(54, 64);
            this.textTesterPort.Name = "textTesterPort";
            this.textTesterPort.Size = new System.Drawing.Size(99, 20);
            this.textTesterPort.TabIndex = 7;
            this.textTesterPort.Text = "500";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(4, 67);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(40, 13);
            this.label7.TabIndex = 6;
            this.label7.Text = "Port in:";
            // 
            // buttonPing
            // 
            this.buttonPing.Location = new System.Drawing.Point(86, 92);
            this.buttonPing.Name = "buttonPing";
            this.buttonPing.Size = new System.Drawing.Size(63, 23);
            this.buttonPing.TabIndex = 5;
            this.buttonPing.Text = "Ping";
            this.buttonPing.UseVisualStyleBackColor = true;
            this.buttonPing.Click += new System.EventHandler(this.buttonPing_Click);
            // 
            // buttonConnect
            // 
            this.buttonConnect.Location = new System.Drawing.Point(10, 92);
            this.buttonConnect.Name = "buttonConnect";
            this.buttonConnect.Size = new System.Drawing.Size(63, 23);
            this.buttonConnect.TabIndex = 4;
            this.buttonConnect.Text = "Connect";
            this.buttonConnect.UseVisualStyleBackColor = true;
            this.buttonConnect.Click += new System.EventHandler(this.buttonConnect_Click);
            // 
            // textServerPort
            // 
            this.textServerPort.Location = new System.Drawing.Point(54, 38);
            this.textServerPort.Name = "textServerPort";
            this.textServerPort.Size = new System.Drawing.Size(99, 20);
            this.textServerPort.TabIndex = 3;
            this.textServerPort.Text = "1001";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(4, 41);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(47, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Port out:";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(7, 16);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(26, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "I.P.:";
            // 
            // textServerIP
            // 
            this.textServerIP.Location = new System.Drawing.Point(54, 13);
            this.textServerIP.Name = "textServerIP";
            this.textServerIP.Size = new System.Drawing.Size(99, 20);
            this.textServerIP.TabIndex = 0;
            this.textServerIP.Text = "127.0.0.1";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.textBoxUser);
            this.groupBox2.Controls.Add(this.labelUser);
            this.groupBox2.Controls.Add(this.buttonAdd);
            this.groupBox2.Controls.Add(this.textParam4);
            this.groupBox2.Controls.Add(this.label5);
            this.groupBox2.Controls.Add(this.textParam3);
            this.groupBox2.Controls.Add(this.label4);
            this.groupBox2.Controls.Add(this.textParam2);
            this.groupBox2.Controls.Add(this.label3);
            this.groupBox2.Controls.Add(this.textParam1);
            this.groupBox2.Controls.Add(this.labelParam1);
            this.groupBox2.Controls.Add(this.comboRequests);
            this.groupBox2.Location = new System.Drawing.Point(8, 131);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(163, 170);
            this.groupBox2.TabIndex = 3;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Add request";
            // 
            // textBoxUser
            // 
            this.textBoxUser.Location = new System.Drawing.Point(59, 46);
            this.textBoxUser.Name = "textBoxUser";
            this.textBoxUser.Size = new System.Drawing.Size(47, 20);
            this.textBoxUser.TabIndex = 11;
            this.textBoxUser.Text = "1";
            this.textBoxUser.TextChanged += new System.EventHandler(this.textBoxUser_TextChanged);
            // 
            // labelUser
            // 
            this.labelUser.AutoSize = true;
            this.labelUser.Location = new System.Drawing.Point(19, 49);
            this.labelUser.Name = "labelUser";
            this.labelUser.Size = new System.Drawing.Size(32, 13);
            this.labelUser.TabIndex = 10;
            this.labelUser.Text = "User:";
            // 
            // buttonAdd
            // 
            this.buttonAdd.Location = new System.Drawing.Point(118, 77);
            this.buttonAdd.Name = "buttonAdd";
            this.buttonAdd.Size = new System.Drawing.Size(39, 87);
            this.buttonAdd.TabIndex = 9;
            this.buttonAdd.Text = "Add";
            this.buttonAdd.UseVisualStyleBackColor = true;
            this.buttonAdd.Click += new System.EventHandler(this.buttonAdd_Click);
            // 
            // textParam4
            // 
            this.textParam4.Location = new System.Drawing.Point(59, 145);
            this.textParam4.Name = "textParam4";
            this.textParam4.Size = new System.Drawing.Size(49, 20);
            this.textParam4.TabIndex = 8;
            this.textParam4.Text = "0";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(7, 148);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(46, 13);
            this.label5.TabIndex = 7;
            this.label5.Text = "Param4:";
            // 
            // textParam3
            // 
            this.textParam3.Location = new System.Drawing.Point(59, 123);
            this.textParam3.Name = "textParam3";
            this.textParam3.Size = new System.Drawing.Size(49, 20);
            this.textParam3.TabIndex = 6;
            this.textParam3.Text = "0";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(7, 126);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(46, 13);
            this.label4.TabIndex = 5;
            this.label4.Text = "Param3:";
            // 
            // textParam2
            // 
            this.textParam2.Location = new System.Drawing.Point(59, 100);
            this.textParam2.Name = "textParam2";
            this.textParam2.Size = new System.Drawing.Size(49, 20);
            this.textParam2.TabIndex = 4;
            this.textParam2.Text = "0";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(7, 103);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(46, 13);
            this.label3.TabIndex = 3;
            this.label3.Text = "Param2:";
            // 
            // textParam1
            // 
            this.textParam1.Location = new System.Drawing.Point(59, 77);
            this.textParam1.Name = "textParam1";
            this.textParam1.Size = new System.Drawing.Size(49, 20);
            this.textParam1.TabIndex = 2;
            this.textParam1.Text = "0";
            // 
            // labelParam1
            // 
            this.labelParam1.AutoSize = true;
            this.labelParam1.Location = new System.Drawing.Point(7, 80);
            this.labelParam1.Name = "labelParam1";
            this.labelParam1.Size = new System.Drawing.Size(46, 13);
            this.labelParam1.TabIndex = 1;
            this.labelParam1.Text = "Param1:";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.buttonSend);
            this.groupBox3.Controls.Add(this.buttonClear);
            this.groupBox3.Controls.Add(this.buttonRemove);
            this.groupBox3.Controls.Add(this.label6);
            this.groupBox3.Controls.Add(this.listRequestBuffer);
            this.groupBox3.Location = new System.Drawing.Point(177, 4);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(414, 177);
            this.groupBox3.TabIndex = 4;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Send request";
            // 
            // buttonSend
            // 
            this.buttonSend.Location = new System.Drawing.Point(310, 149);
            this.buttonSend.Name = "buttonSend";
            this.buttonSend.Size = new System.Drawing.Size(84, 19);
            this.buttonSend.TabIndex = 5;
            this.buttonSend.Text = "Send";
            this.buttonSend.UseVisualStyleBackColor = true;
            this.buttonSend.Click += new System.EventHandler(this.buttonSend_Click);
            // 
            // buttonClear
            // 
            this.buttonClear.Location = new System.Drawing.Point(91, 148);
            this.buttonClear.Name = "buttonClear";
            this.buttonClear.Size = new System.Drawing.Size(82, 19);
            this.buttonClear.TabIndex = 4;
            this.buttonClear.Text = "Clear";
            this.buttonClear.UseVisualStyleBackColor = true;
            this.buttonClear.Click += new System.EventHandler(this.buttonClear_Click);
            // 
            // buttonRemove
            // 
            this.buttonRemove.Location = new System.Drawing.Point(5, 148);
            this.buttonRemove.Name = "buttonRemove";
            this.buttonRemove.Size = new System.Drawing.Size(80, 19);
            this.buttonRemove.TabIndex = 3;
            this.buttonRemove.Text = "Remove";
            this.buttonRemove.UseVisualStyleBackColor = true;
            this.buttonRemove.Click += new System.EventHandler(this.buttonRemove_Click);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(6, 18);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(80, 13);
            this.label6.TabIndex = 2;
            this.label6.Text = "Request buffer:";
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.richTextBoxAnswer);
            this.groupBox4.Location = new System.Drawing.Point(12, 307);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(579, 215);
            this.groupBox4.TabIndex = 5;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Answers";
            // 
            // richTextBoxAnswer
            // 
            this.richTextBoxAnswer.Location = new System.Drawing.Point(6, 19);
            this.richTextBoxAnswer.Name = "richTextBoxAnswer";
            this.richTextBoxAnswer.Size = new System.Drawing.Size(554, 211);
            this.richTextBoxAnswer.TabIndex = 0;
            this.richTextBoxAnswer.Text = "";
            // 
            // groupBox5
            // 
            this.groupBox5.Controls.Add(this.checkBoxProgress);
            this.groupBox5.Controls.Add(this.checkBoxStatus);
            this.groupBox5.Controls.Add(this.checkBoxHearthbeat);
            this.groupBox5.Controls.Add(this.buttonClearBuffer);
            this.groupBox5.Location = new System.Drawing.Point(177, 187);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Size = new System.Drawing.Size(414, 112);
            this.groupBox5.TabIndex = 6;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = "Answer control";
            // 
            // checkBoxProgress
            // 
            this.checkBoxProgress.AutoSize = true;
            this.checkBoxProgress.Location = new System.Drawing.Point(135, 21);
            this.checkBoxProgress.Name = "checkBoxProgress";
            this.checkBoxProgress.Size = new System.Drawing.Size(103, 17);
            this.checkBoxProgress.TabIndex = 3;
            this.checkBoxProgress.Text = "Display progress";
            this.checkBoxProgress.UseVisualStyleBackColor = true;
            // 
            // checkBoxStatus
            // 
            this.checkBoxStatus.AutoSize = true;
            this.checkBoxStatus.Location = new System.Drawing.Point(6, 46);
            this.checkBoxStatus.Name = "checkBoxStatus";
            this.checkBoxStatus.Size = new System.Drawing.Size(91, 17);
            this.checkBoxStatus.TabIndex = 2;
            this.checkBoxStatus.Text = "Display status";
            this.checkBoxStatus.UseVisualStyleBackColor = true;
            // 
            // checkBoxHearthbeat
            // 
            this.checkBoxHearthbeat.AutoSize = true;
            this.checkBoxHearthbeat.Location = new System.Drawing.Point(6, 21);
            this.checkBoxHearthbeat.Name = "checkBoxHearthbeat";
            this.checkBoxHearthbeat.Size = new System.Drawing.Size(114, 17);
            this.checkBoxHearthbeat.TabIndex = 1;
            this.checkBoxHearthbeat.Text = "Display hearthbeat";
            this.checkBoxHearthbeat.UseVisualStyleBackColor = true;
            // 
            // buttonClearBuffer
            // 
            this.buttonClearBuffer.Location = new System.Drawing.Point(6, 83);
            this.buttonClearBuffer.Name = "buttonClearBuffer";
            this.buttonClearBuffer.Size = new System.Drawing.Size(75, 23);
            this.buttonClearBuffer.TabIndex = 0;
            this.buttonClearBuffer.Text = "Clear buffer";
            this.buttonClearBuffer.UseVisualStyleBackColor = true;
            this.buttonClearBuffer.Click += new System.EventHandler(this.buttonClearBuffer_Click);
            // 
            // IGServerTester
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(603, 534);
            this.Controls.Add(this.groupBox5);
            this.Controls.Add(this.groupBox4);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "IGServerTester";
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.Text = "IGServer Tester - Disconnected";
            this.Load += new System.EventHandler(this.IGServerTester_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.groupBox4.ResumeLayout(false);
            this.groupBox5.ResumeLayout(false);
            this.groupBox5.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ComboBox comboRequests;
        private System.Windows.Forms.ListBox listRequestBuffer;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TextBox textServerPort;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox textServerIP;
        private System.Windows.Forms.Button buttonPing;
        private System.Windows.Forms.Button buttonConnect;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Label labelParam1;
        private System.Windows.Forms.TextBox textParam4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox textParam3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox textParam2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox textParam1;
        private System.Windows.Forms.Button buttonAdd;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Button buttonSend;
        private System.Windows.Forms.Button buttonClear;
        private System.Windows.Forms.Button buttonRemove;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox textTesterPort;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.RichTextBox richTextBoxAnswer;
        internal System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.TextBox textBoxUser;
        private System.Windows.Forms.Label labelUser;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.CheckBox checkBoxHearthbeat;
        private System.Windows.Forms.Button buttonClearBuffer;
        private System.Windows.Forms.CheckBox checkBoxStatus;
        private System.Windows.Forms.CheckBox checkBoxProgress;
    }
}

