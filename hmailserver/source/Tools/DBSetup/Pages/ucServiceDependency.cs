// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.ServiceProcess;
using hMailServer.Shared;

namespace DBSetup.Pages
{
   public partial class ucServiceDependency : UserControl, IWizardPage
   {
      private Dictionary<string, string> _state;
      public ucServiceDependency()
      {
         InitializeComponent();

         ServiceController[] services = ServiceController.GetServices();

         foreach (ServiceController controller in services)
         {
            if (controller.ServiceName.ToLower() == "hmailserver")
               continue;

            comboServices.Items.Add(controller.DisplayName + " (" + controller.ServiceName + ")");
         }

      }

      public void OnShowPage(Dictionary<string, string> state)
      {
         _state = state;

         if (_state.ContainsKey("ServiceDependency"))
            comboServices.Text = _state["ServiceDependency"];
      }

      public bool OnLeavePage(bool next)
      {
         string serviceName = comboServices.Text;

         if (serviceName.Length > 0)
         {
            bool found = false;
            foreach (string s in comboServices.Items)
            {
               if (s == serviceName)
               {
                  found = true;
                  break;
               }
            }

            if (!found)
            {
               MessageBox.Show("The entered service name was not found in the list of services." + Environment.NewLine + "Please select a service from the list", "hMailServer");
               return false;
            }
         }

         _state["ServiceDependency"] = comboServices.Text;

         return true;
      }

      public string Title
      {
         get
         {
            return "Set hMailServer service dependency";
         }
      }

   }
}
