alter table hm_imapfolders add column folderspecialuse int not null default 0;

-- folderspecialuse is a bitmask matching HM::IMAPFolder::SpecialUseFlags (IMAPFolder.h):
-- 1=All, 2=Archive, 4=Drafts, 8=Flagged, 16=Junk, 32=Sent, 64=Trash
update hm_imapfolders set folderspecialuse = 32 where folderparentid = -1 and foldername = 'Sent';

update hm_imapfolders set folderspecialuse = 4 where folderparentid = -1 and foldername = 'Drafts';

update hm_imapfolders set folderspecialuse = 64 where folderparentid = -1 and foldername = 'Trash';

update hm_imapfolders set folderspecialuse = 16 where folderparentid = -1 and foldername = 'Junk';

update hm_imapfolders set folderspecialuse = 2 where folderparentid = -1 and foldername = 'Archive';

update hm_dbversion set value = 5709;
