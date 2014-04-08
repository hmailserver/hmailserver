update hm_dnsbl set sblresult = '127.0.0.2-8|127.0.0.10-11' where sbldnshost = 'zen.spamhaus.org' and sblresult = '127.0.0.*';

update hm_dnsbl set sblresult = '127.0.0.2' where sbldnshost = 'bl.spamcop.net' and sblresult = '127.0.0.*';

update hm_dbversion set value = 5320;

