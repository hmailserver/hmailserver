alter table hm_imapfolders add column folderspecialuse int not null default 0;

-- folderspecialuse is a bitmask matching HM::IMAPFolder::SpecialUseFlags (IMAPFolder.h):
-- 1=Archive, 2=Drafts, 4=Junk, 8=Sent, 16=Trash
update hm_imapfolders set folderspecialuse = 8 where folderparentid = -1 and foldername = 'Sent';

update hm_imapfolders set folderspecialuse = 2 where folderparentid = -1 and foldername = 'Drafts';

update hm_imapfolders set folderspecialuse = 16 where folderparentid = -1 and foldername = 'Trash';

update hm_imapfolders set folderspecialuse = 4 where folderparentid = -1 and foldername = 'Junk';

update hm_imapfolders set folderspecialuse = 1 where folderparentid = -1 and foldername = 'Archive';

insert into hm_settings (settingname, settingstring, settinginteger) values ('CreateDefaultSpecialUseFolders', '', 1);

update hm_dbversion set value = 5709;
