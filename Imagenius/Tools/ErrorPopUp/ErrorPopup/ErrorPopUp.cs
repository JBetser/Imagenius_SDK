using System;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.HtmlControls;
using AjaxControlToolkit;
using System.ComponentModel;

namespace ErrorPopup
{
    public delegate void TypeOnOkClicked(object sender, EventArgs e);
        
    /// <summary>
    /// Error popup window control
    /// </summary>
    public class ErrorPopUp : Control
    {
        private EventHandlerList m_handlers = new EventHandlerList();

        /// <summary>
        /// Sets the default values
        /// </summary>
        public ErrorPopUp()
        {
            this.Show = false;
            this.Message = "";
            this.Title = "Error Message";
            this.ImageUrl = "";
            this.Ok = "Ok";
            this.ErrorModalPopupCssClass = "errorModalPopup";
            this.TitleCssClass = "title";
            this.MessageCssClass = "message";
            this.ControlCssClass = "control";
            this.ImageCssClass = "image";
            this.ModalBackground = "modalBackground";
            this.Width = 600;
            this.Hieght = 220;
            this.Direction = ContentDirection.LeftToRight;
        }

        private event TypeOnOkClicked OnOkClickedEvent
        {
            add
            {
                m_handlers.AddHandler("OnOkClicked", value);
            }
            remove
            {
                m_handlers.RemoveHandler("OnOkClicked", value);
            }
        } 

        # region Properties 

        /// <summary>
        /// Gets or sets value indicating whether to show control or not
        /// </summary>
        [ClientPropertyName("Show")]
        public bool Show { get { return GetProperty<bool>("Show"); } set { SetProperty("Show", value); } }


        /// <summary>
        /// Gets or sets the message to be displayed
        /// </summary>
        [ClientPropertyName("Message")]
        public string Message { get { return GetProperty<string>("Message"); } set { SetProperty("Message", value); } }

        /// <summary>
        /// Gets or sets the tile of the message
        /// </summary>
        [ClientPropertyName("Title")]
        public string Title { get { return GetProperty<string>("Title"); } set { SetProperty("Title", value); } }

        /// <summary>
        /// Gets or sets the image url
        /// </summary>
        [ClientPropertyName("ImageUrl")]
        public string ImageUrl { get { return GetProperty<string>("ImageUrl"); } set { SetProperty("ImageUrl", value); } }

        /// <summary>
        /// Gets or sets the Ok label
        /// </summary>
        [ClientPropertyName("Ok")]
        public string Ok { get { return GetProperty<string>("Ok"); } set { SetProperty("Ok", value); } }

        /// <summary>
        /// Gets or sets the AJAX toolkit Modal  Popup
        /// </summary>
        [ClientPropertyName("ErrorModalPopupCssClass")]
        public string ErrorModalPopupCssClass { get { return GetProperty<string>("ErrorModalPopupCssClass"); } set { SetProperty("ErrorModalPopupCssClass", value); } }

        /// <summary>
        /// Gets or sets the title css class
        /// </summary>
        [ClientPropertyName("TitleCssClass")]
        public string TitleCssClass { get { return GetProperty<string>("TitleCssClass"); } set { SetProperty("TitleCssClass", value); } }

        /// <summary>
        /// Gets or sets the message css class
        /// </summary>
        [ClientPropertyName("MessageCssClass")]
        public string MessageCssClass { get { return GetProperty<string>("MessageCssClass"); } set { SetProperty("MessageCssClass", value); } }

        /// <summary>
        /// Gets or sets the control css class
        /// </summary>
        [ClientPropertyName("ControlCssClass")]
        public string ControlCssClass { get { return GetProperty<string>("ControlCssClass"); } set { SetProperty("ControlCssClass", value); } }

        /// <summary>
        /// Gets or sets the image css class
        /// </summary>
        [ClientPropertyName("ImageCssClass")]
        public string ImageCssClass { get { return GetProperty<string>("ImageCssClass"); } set { SetProperty("ImageCssClass", value); } }

        /// <summary>
        /// Gets or sets the AJAX toolkit Modal background
        /// </summary>
        [ClientPropertyName("ModalBackground")]
        public string ModalBackground { get { return GetProperty<string>("ModalBackground"); } set { SetProperty("ModalBackground", value); } }

        /// <summary>
        /// Gets or sets the window's width
        /// </summary>
        [ClientPropertyName("Width")]
        public int Width { get { return GetProperty<int>("Width"); } set { SetProperty("Width", value); } }


        /// <summary>
        /// Gets or sets the window's height 
        /// </summary>
        
        [ClientPropertyName("Hieght")]
        [DefaultValue(220)]
        public int Hieght { get { return GetProperty<int>("Hieght"); } set { SetProperty("Hieght", value); } }

        /// <summary>
        /// Gets or sets the OnOkClicked event
        /// </summary>

        [DefaultValue("")] 
        public event TypeOnOkClicked OnOkClicked 
        { 
            add
            {
                this.OnOkClickedEvent += new TypeOnOkClicked(value);
            }
            remove
            {
                this.OnOkClickedEvent -= value;
            }
        }


        //private ContentDirection _Direction = ContentDirection.LeftToRight;
        /// <summary>
        /// Gets or sets the window's direction
        /// </summary>
        [DefaultValue(ContentDirection.LeftToRight)]
        [ClientPropertyName("Direction")]
        public ContentDirection Direction { get { return GetProperty<ContentDirection>("Direction"); } set { SetProperty("Direction", value); } }

        /// <summary>
        /// Helper method to get the value from the ViewState
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="propertyName"></param>
        /// <returns></returns>
        private T GetProperty<T>(string propertyName)
        {
            if (ViewState[propertyName] == null)
            {
                return default(T);
            }
            return (T)ViewState[propertyName];
        }

        /// <summary>
        /// Helper method to set the value to the ViewState
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="propertyName"></param>
        /// <param name="value"></param>
        private void SetProperty<T>(string propertyName, T value)
        {
            ViewState[propertyName] = value;
        }
        #endregion 

        #region OnPreRender 

        /// <summary>
        /// Fires PreRender event and register the embedded Css style and renders the control
        /// </summary>
        /// <param name="e"></param>
        protected override void OnPreRender(EventArgs e)
        {
            if (Show)
            {
                // Include Css Style embedded resource Url
                string tempLink = "<link rel='stylesheet' text='text/css' href='{0}' />";
                string location = Page.ClientScript.GetWebResourceUrl(this.GetType(), "ErrorPopup.Style.css");
                LiteralControl include = new LiteralControl(String.Format(tempLink, location));
                ((System.Web.UI.HtmlControls.HtmlHead)Page.Header).Controls.Add(include);

                // Include the Javascript embedded resource Url
                string srptLoc = Page.ClientScript.GetWebResourceUrl(this.GetType(), "ErrorPopup.JS.js");
                AddJavaScriptInclude(srptLoc);

                // Render the control
                RenderControl();
            }

            base.OnPreRender(e);
        }

        /// <summary>
        /// Adds a JavaScript reference to the HTML head tag
        /// </summary>
        public virtual void AddJavaScriptInclude(string url)
        {
            HtmlGenericControl script = new HtmlGenericControl("script");
            script.Attributes["type"] = "text/javascript";
            script.Attributes["src"] = url;
            Page.Header.Controls.Add(script);
        }


        #endregion 

        #region Render Control

        /// <summary>
        ///  Renders the error popup control
        /// </summary>
        private void RenderControl()
        {
            // Popup Panel
            Panel PopupControl = new Panel();
            PopupControl.ID = "PopupControl";
            PopupControl.Direction = this.Direction;
            PopupControl.Style.Add("display", "none");

            // Drag Popup panel
            Panel PopupDragHandle = new Panel();
            PopupDragHandle.ID = "PopupDragHandle";
            PopupDragHandle.CssClass = this.ErrorModalPopupCssClass;
            PopupDragHandle.Width = new Unit(this.Width);
            PopupDragHandle.Height = new Unit(this.Hieght);

            // Title
            HtmlGenericControl titleDiv = new HtmlGenericControl("div");
            titleDiv.Attributes.Add("class", this.TitleCssClass);
            titleDiv.InnerText = this.Title;
            PopupDragHandle.Controls.Add(titleDiv);

            // Image
            HtmlGenericControl imageDiv = new HtmlGenericControl("div");
            imageDiv.Attributes.Add("class", this.ImageCssClass);
            HtmlImage image = new HtmlImage();

            if (String.IsNullOrEmpty(this.ImageUrl))
                this.ImageUrl = this.Page.ClientScript.GetWebResourceUrl(this.GetType(), "ErrorPopup.error.gif");

            image.Src = this.ImageUrl;
            imageDiv.Controls.Add(image);
            PopupDragHandle.Controls.Add(imageDiv);

            // Message
            HtmlGenericControl errorDiv = new HtmlGenericControl("div");
            errorDiv.Attributes.Add("class", this.MessageCssClass);
            errorDiv.InnerHtml = this.Message;
            PopupDragHandle.Controls.Add(errorDiv);

            // Button
            HtmlGenericControl buttonDiv = new HtmlGenericControl("div");
            buttonDiv.Attributes.Add("class", this.ControlCssClass);
            Button OkButton = new Button();
            OkButton.ID = "OkButton";
            OkButton.Click += new EventHandler(OkButton_Click);
            OkButton.Text = this.Ok;
            buttonDiv.Controls.Add(OkButton);
            PopupDragHandle.Controls.Add(buttonDiv);

            // Add controls
            PopupControl.Controls.Add(PopupDragHandle);
            this.Controls.Add(PopupControl);

            // Add AJAX toolkit Modal Popup Control
            LinkButton LinkButton_Dummy = new LinkButton();
            LinkButton_Dummy.ID = "LinkButton_Dummy";
            this.Controls.Add(LinkButton_Dummy);

            AjaxControlToolkit.ModalPopupExtender errorPopupExtender = new AjaxControlToolkit.ModalPopupExtender();
            errorPopupExtender.TargetControlID = "LinkButton_Dummy";
            errorPopupExtender.PopupControlID = "PopupControl";
            errorPopupExtender.BackgroundCssClass = this.ModalBackground;
            errorPopupExtender.OkControlID = "OkButton";
            // Fix an issue with FireFox, when postback the modal popup
            // flickers along with it. Resetting PopupControl display to none through
            // javascript solves the problem.
            errorPopupExtender.OnOkScript = "RemoveMe('" + PopupControl.ClientID + "')";
            errorPopupExtender.PopupDragHandleControlID = "PopupDragHandle";
            errorPopupExtender.DropShadow = true;
            this.Controls.Add(errorPopupExtender);
            errorPopupExtender.Show();
        }

        void OkButton_Click(object sender, EventArgs e)
        {
            TypeOnOkClicked handler = (TypeOnOkClicked)m_handlers["OnOkClicked"];
            if (handler == null)
                return;
            handler.Invoke(sender, e);
        }

        /// <summary>
        /// Shows the control
        /// </summary>
        public void ShowControl()
        {
            this.Show = true;
        }

        /// <summary>
        /// Hides the control
        /// </summary>
        public void HideControl()
        {
            this.Show = false;
        }
        #endregion 
    }
}
