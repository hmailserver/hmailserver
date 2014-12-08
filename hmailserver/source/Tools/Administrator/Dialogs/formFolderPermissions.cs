// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities;
using System.Runtime.InteropServices;
using hMailServer.Shared;

namespace hMailServer.Administrator.Dialogs
{
    public partial class formFolderPermissions : Form
    {
        private hMailServer.IMAPFolder _folder;

        public formFolderPermissions(hMailServer.IMAPFolder folder)
        {
            InitializeComponent();

            EnableDisable();

            _folder = folder;

            ListPermissions();

            new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);

            labelPermissionsFor.Text = "";
            Strings.Localize(this);
        }

        private void ListPermissions()
        {
            listACL.Items.Clear();

            hMailServer.IMAPFolderPermissions permissions = _folder.Permissions;

            for (int i = 0; i < permissions.Count; i++)
            {
                hMailServer.IMAPFolderPermission permission = permissions[i];

                ListPermission(permission);
            }

            Marshal.ReleaseComObject(permissions);

        }

        private hMailServer.Groups GetGroups()
        {
            hMailServer.Settings settings = APICreator.Application.Settings;

            hMailServer.Groups groups = settings.Groups;

            Marshal.ReleaseComObject(settings);

            return groups;
        }

        private string GetPermissionName(hMailServer.IMAPFolderPermission permission)
        {

            string name = "";
            switch (permission.PermissionType)
            {
                case eACLPermissionType.ePermissionTypeUser:
                    {
                        hMailServer.Account account = permission.Account;
                        name = account.Address;
                        break;
                    }
                case eACLPermissionType.ePermissionTypeGroup:
                    {
                        hMailServer.Group group = permission.Group;
                        name = group.Name;
                        break;
                    }
                case eACLPermissionType.ePermissionTypeAnyone:
                    {
                        name = EnumStrings.GetPermissionTypeString(permission.PermissionType);
                        break;

                    }
            }

            return name;
        }

        private ListViewItem ListPermission(hMailServer.IMAPFolderPermission permission)
        {
            string name = "<Deleted>";

            try
            {
                name = GetPermissionName(permission);
            }
            catch (Exception)
            {

            }

            ListViewItem item = listACL.Items.Add(EnumStrings.GetPermissionTypeString(permission.PermissionType));
            item.SubItems.Add(name);
            item.Tag = permission;
            return item;


        }

        private void SaveRightsForPermission()
        {
            if (listACL.SelectedItems.Count == 0)
                return;

            hMailServer.IMAPFolderPermission permission = listACL.SelectedItems[0].Tag as hMailServer.IMAPFolderPermission;

            permission.set_Permission(eACLPermission.ePermissionAdminister, checkPermissionAdminister.Checked);
            permission.set_Permission(eACLPermission.ePermissionCreate, checkPermissionCreateMailbox.Checked);
            permission.set_Permission(eACLPermission.ePermissionDeleteMailbox, checkPermissionDeleteMailbox.Checked);
            permission.set_Permission(eACLPermission.ePermissionExpunge, checkPermissionExpunge.Checked);
            permission.set_Permission(eACLPermission.ePermissionInsert, checkPermissionInsert.Checked);
            permission.set_Permission(eACLPermission.ePermissionLookup, checkPermissionLookup.Checked);
            permission.set_Permission(eACLPermission.ePermissionRead, checkPermissionRead.Checked);
            permission.set_Permission(eACLPermission.ePermissionWriteDeleted, checkPermissionKeepDeletedFlag.Checked);
            permission.set_Permission(eACLPermission.ePermissionWriteOthers, checkPermissionKeepOtherFlags.Checked);
            permission.set_Permission(eACLPermission.ePermissionWriteSeen, checkPermissionKeepSeenFlag.Checked);
            permission.set_Permission(eACLPermission.ePermissionAdminister, checkPermissionAdminister.Checked);

            permission.Save();
        }

        private void ShowRightsForPermission()
        {
            if (listACL.SelectedItems.Count == 0)
                return;

            string permissionsLabel = "Permissions for %s";
            permissionsLabel = permissionsLabel.Replace("%s", listACL.SelectedItems[0].SubItems[1].Text);
            labelPermissionsFor.Text = permissionsLabel;

            IMAPFolderPermission permission = listACL.SelectedItems[0].Tag as IMAPFolderPermission;
            checkPermissionAdminister.Checked = permission.get_Permission(eACLPermission.ePermissionAdminister);
            checkPermissionCreateMailbox.Checked = permission.get_Permission(eACLPermission.ePermissionCreate);
            checkPermissionDeleteMailbox.Checked = permission.get_Permission(eACLPermission.ePermissionDeleteMailbox);
            checkPermissionExpunge.Checked = permission.get_Permission(eACLPermission.ePermissionExpunge);
            checkPermissionInsert.Checked = permission.get_Permission(eACLPermission.ePermissionInsert);
            checkPermissionLookup.Checked = permission.get_Permission(eACLPermission.ePermissionLookup);
            checkPermissionRead.Checked = permission.get_Permission(eACLPermission.ePermissionRead);
            checkPermissionKeepDeletedFlag.Checked = permission.get_Permission(eACLPermission.ePermissionWriteDeleted);
            checkPermissionKeepOtherFlags.Checked = permission.get_Permission(eACLPermission.ePermissionWriteOthers);
            checkPermissionKeepSeenFlag.Checked = permission.get_Permission(eACLPermission.ePermissionWriteSeen);
        }

        private void SelectAll(bool select)
        {
            checkPermissionAdminister.Checked = select;
            checkPermissionCreateMailbox.Checked = select;
            checkPermissionDeleteMailbox.Checked = select;
            checkPermissionExpunge.Checked = select;
            checkPermissionInsert.Checked = select;
            checkPermissionLookup.Checked = select;
            checkPermissionRead.Checked = select;
            checkPermissionKeepDeletedFlag.Checked = select;
            checkPermissionKeepOtherFlags.Checked = select;
            checkPermissionKeepSeenFlag.Checked = select;
        }

        private void EnableDisable()
        {
            bool enable = listACL.SelectedItems.Count > 0;

            checkPermissionAdminister.Enabled = enable;
            checkPermissionCreateMailbox.Enabled = enable;
            checkPermissionDeleteMailbox.Enabled = enable;
            checkPermissionExpunge.Enabled = enable;
            checkPermissionInsert.Enabled = enable;
            checkPermissionLookup.Enabled = enable;
            checkPermissionRead.Enabled = enable;
            checkPermissionKeepDeletedFlag.Enabled = enable;
            checkPermissionKeepOtherFlags.Enabled = enable;
            checkPermissionKeepSeenFlag.Enabled = enable;

            buttonSelectAll.Enabled = enable;
            buttonSelectNone.Enabled = enable;
            buttonDeletePermission.Enabled = enable;

            if (!enable)
                SelectAll(false);
        }

        private void listACL_SelectedIndexChanged(object sender, EventArgs e)
        {
            EnableDisable();
            ShowRightsForPermission();

        }

        private void buttonSelectAll_Click(object sender, EventArgs e)
        {
            SelectAll(true);

        }

        private void buttonSelectNone_Click(object sender, EventArgs e)
        {
            SelectAll(false);
        }

        private void buttonDeleteWhiteList_Click(object sender, EventArgs e)
        {
            foreach (ListViewItem item in listACL.SelectedItems)
            {
                hMailServer.IMAPFolderPermission permission = item.Tag as hMailServer.IMAPFolderPermission;

                permission.Delete();
            }

            ListPermissions();

            EnableDisable();
        }

        private void buttonAddPermission_Click(object sender, EventArgs e)
        {
            SaveRightsForPermission();

            formSelectUsers selectUsersDlg = new formSelectUsers(false, 0);

            if (selectUsersDlg.ShowDialog() == DialogResult.OK)
            {
                listACL.SelectedItems.Clear();


                List<int> selectedItems = selectUsersDlg.GetSelectedIDs();

                eACLPermissionType type = selectUsersDlg.Type;

                if (type == eACLPermissionType.ePermissionTypeAnyone)
                {
                    IMAPFolderPermission permission = _folder.Permissions.Add();
                    permission.PermissionType = eACLPermissionType.ePermissionTypeAnyone;
                    permission.Save();

                    ListViewItem listItem = ListPermission(permission);
                    listItem.Selected = true;
                    listACL.Focus();
                }
                else
                {
                    foreach (int itemID in selectedItems)
                    {
                        IMAPFolderPermission permission = _folder.Permissions.Add();

                        switch (type)
                        {
                            case eACLPermissionType.ePermissionTypeGroup:
                                {
                                    permission.PermissionType = eACLPermissionType.ePermissionTypeGroup;
                                    permission.PermissionGroupID = itemID;

                                    break;
                                }
                            case eACLPermissionType.ePermissionTypeUser:
                                {
                                    permission.PermissionType = eACLPermissionType.ePermissionTypeUser;
                                    permission.PermissionAccountID = itemID;
                                    break;
                                }
                            default:
                                {
                                    throw new Exception("Trying to add unknown object type to permission list");
                                }
                        }

                        permission.Save();

                        ListViewItem listItem = ListPermission(permission);
                        listItem.Selected = true;
                        listACL.Focus();
                    }

                }
            }
        }

        private void buttonClose_Click(object sender, EventArgs e)
        {
            SaveRightsForPermission();
        }

        private void listACL_BeforeSelectedIndexChanged()
        {
            SaveRightsForPermission();
        }

    }
}