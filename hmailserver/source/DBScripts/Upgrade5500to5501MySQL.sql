insert into hm_settings (settingname, settingstring, settinginteger) values ('SmtpDeliveryConnectionSecurity', '', 2);

insert into hm_settings (settingname, settingstring, settinginteger) values ('VerifyRemoteSslCertificate', '', 1);

update hm_dbversion set value = 5501;

