using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.Script.Serialization;
using System.ComponentModel;

public class CallBackEventArgs : EventArgs
{
    internal CallBackEventArgs(Object callBackArgument)
    {
        this._callBackArgument = callBackArgument;
    }
    private Object _callBackArgument;
    public Object CallBackArgument
    {
        get { return _callBackArgument; }
        set { _callBackArgument = value; }
    }
}

[NonVisualControl()]
[DefaultEvent("CallBack")]
public class PostBackControl : WebControl, IPostBackEventHandler
{
    private Boolean _deserializeCallBackArgument = true;
    /// <summary>
    /// Gets or sets a value indicating whether the argument has to be JSON deserialized.
    /// </summary>
    /// <value>
    /// 	<c>true</c> if the argument must be JSON Deserialized otherwise, <c>false</c>.
    /// </value>
    /// <author>Cyril</author>
    [DefaultValue(true)]
    public Boolean DeserializeCallBackArgument
    {
        get { return _deserializeCallBackArgument; }
        set { _deserializeCallBackArgument = value; }
    }
    /// <summary>
    /// Occurs when the JavaScript call it.
    /// </summary>
    public event EventHandler<CallBackEventArgs> CallBack;
    /// <summary>
    /// When implemented by a class, enables a server control to process an event raised when a form is posted to the server.
    /// </summary>
    /// <param name="eventArgument">A <see cref="T:System.String"></see> that represents an optional event argument to be passed to the event handler.</param>
    public void RaisePostBackEvent(string eventArgument)
    {
        if (CallBack != null)
        {
            if (_deserializeCallBackArgument)
            {
                CallBack.Invoke(this, new CallBackEventArgs(new JavaScriptSerializer().DeserializeObject(eventArgument)));
            }
            else
            {
                CallBack.Invoke(this, new CallBackEventArgs(eventArgument));
            }
        }
    }
    /// <summary>
    /// Gets the call back function.
    /// </summary>
    /// <param name="arguments">The arguments, will be evaluated by javascript.</param>
    public String GetCallBackFunction(String arguments)
    {
        if (String.IsNullOrEmpty(arguments))
        {
            arguments = "''";
        }
        this.EnsureID();
        // Page.ClientScript.GetPostBackEventReference ne convient pas car elle convertis l'argument en String JavaScript
        // on ne peut donc pas intéragir directement avec JavaScript.
        return String.Format("__doPostBack('{0}',{1});", this.UniqueID, arguments);
    }
}