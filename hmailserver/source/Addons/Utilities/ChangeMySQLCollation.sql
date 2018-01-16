ALTER TABLE hm_message_metadata CONVERT TO CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

ALTER TABLE hm_message_metadata MODIFY metadata_from VARCHAR(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
ALTER TABLE hm_message_metadata MODIFY metadata_subject VARCHAR(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
ALTER TABLE hm_message_metadata MODIFY metadata_to VARCHAR(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci; 
ALTER TABLE hm_message_metadata MODIFY metadata_cc VARCHAR(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci; 