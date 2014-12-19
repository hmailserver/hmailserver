// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Threading;

namespace hMailServer.Shared
{
    public partial class formErrorMessage : Form
    {
        private Exception _exception;
        private UnhandledExceptionEventArgs _args;
        private ThreadExceptionEventArgs _threadArgs;

        private string _title;
        private string _errorMessage;

        public formErrorMessage(UnhandledExceptionEventArgs args)
        {
            _args = args;

            InitializeComponent();
        }

        public formErrorMessage(ThreadExceptionEventArgs args)
        {
            _threadArgs = args;

            InitializeComponent();
        }

        public formErrorMessage(Exception exception, string title, string errorMessage)
        {
            _exception = exception;
            _title = title;
            _errorMessage = errorMessage;

            InitializeComponent();
        }

        private void buttonClose_Click(object sender, EventArgs e)
        {
            if (_args != null)
            {
                // Global exception. Exit application.
                System.Windows.Forms.Application.Exit();
            }
            else
            {
                this.Close();
            }
        }

        private void formErrorMessage_Shown(object sender, EventArgs e)
        {

            if (_exception == null)
            {
                if (_args == null)
                    _exception = ((Exception)(_threadArgs.Exception));
                else
                    _exception = ((Exception)(_args.ExceptionObject));
            }

            if (string.IsNullOrEmpty(_errorMessage))
                textErrorMessage.Text = _exception.Message;
            else
                textErrorMessage.Text = _errorMessage;

            if (!string.IsNullOrEmpty(_title))
                this.Text = _title;

            string details = _exception.Message + Environment.NewLine + Environment.NewLine;

            details += string.Format("ExceptionType: {0}" + Environment.NewLine, _exception.GetType().Name);
            details += string.Format("HelpLine: {0}" + Environment.NewLine, _exception.HelpLink);
            details += string.Format("Message: {0}" + Environment.NewLine, _exception.Message);
            details += string.Format("Source: {0}" + Environment.NewLine, _exception.Source);
            details += string.Format("StackTrace: {0}" + Environment.NewLine, _exception.StackTrace);
            details += string.Format("TargetSite: {0}" + Environment.NewLine, _exception.TargetSite);
            
            string indent = "\t";
            Exception ie = _exception;
            while (!((ie.InnerException == null)))
            {
                ie = ie.InnerException;
                details += string.Format(indent + "****** Inner Exception ******" + Environment.NewLine);
                details += string.Format(indent + "ExceptionType: {0}" + Environment.NewLine, ie.GetType().Name);
                details += string.Format(indent + "HelpLine: {0}" + Environment.NewLine, ie.HelpLink);
                details += string.Format(indent + "Message: {0}" + Environment.NewLine, ie.Message);
                details += string.Format(indent + "Source: {0}" + Environment.NewLine, ie.Source);
                details += string.Format(indent + "StackTrace: {0}  + Environment.NewLine", ie.StackTrace);
                details += string.Format(indent + "TargetSite: {0}  + Environment.NewLine", ie.TargetSite);
                indent += "\t";
            }

            textErrorDetails.Text = details;
        }
    }
}
