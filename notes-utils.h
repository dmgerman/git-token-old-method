begin_unit
begin_ifndef
ifndef|#
directive|ifndef
name|NOTES_UTILS_H
end_ifndef
begin_define
DECL|macro|NOTES_UTILS_H
define|#
directive|define
name|NOTES_UTILS_H
end_define
begin_include
include|#
directive|include
file|"notes.h"
end_include
begin_comment
comment|/*  * Create new notes commit from the given notes tree  *  * Properties of the created commit:  * - tree: the result of converting t to a tree object with write_notes_tree().  * - parents: the given parents OR (if NULL) the commit referenced by t->ref.  * - author/committer: the default determined by commit_tree().  * - commit message: msg  *  * The resulting commit SHA1 is stored in result_sha1.  */
end_comment
begin_function_decl
name|void
name|create_notes_commit
parameter_list|(
name|struct
name|notes_tree
modifier|*
name|t
parameter_list|,
name|struct
name|commit_list
modifier|*
name|parents
parameter_list|,
specifier|const
name|char
modifier|*
name|msg
parameter_list|,
name|size_t
name|msg_len
parameter_list|,
name|unsigned
name|char
modifier|*
name|result_sha1
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|commit_notes
parameter_list|(
name|struct
name|notes_tree
modifier|*
name|t
parameter_list|,
specifier|const
name|char
modifier|*
name|msg
parameter_list|)
function_decl|;
end_function_decl
begin_struct
DECL|struct|notes_rewrite_cfg
struct|struct
name|notes_rewrite_cfg
block|{
DECL|member|trees
name|struct
name|notes_tree
modifier|*
modifier|*
name|trees
decl_stmt|;
DECL|member|cmd
specifier|const
name|char
modifier|*
name|cmd
decl_stmt|;
DECL|member|enabled
name|int
name|enabled
decl_stmt|;
DECL|member|combine
name|combine_notes_fn
name|combine
decl_stmt|;
DECL|member|refs
name|struct
name|string_list
modifier|*
name|refs
decl_stmt|;
DECL|member|refs_from_env
name|int
name|refs_from_env
decl_stmt|;
DECL|member|mode_from_env
name|int
name|mode_from_env
decl_stmt|;
block|}
struct|;
end_struct
begin_function_decl
name|struct
name|notes_rewrite_cfg
modifier|*
name|init_copy_notes_for_rewrite
parameter_list|(
specifier|const
name|char
modifier|*
name|cmd
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|copy_note_for_rewrite
parameter_list|(
name|struct
name|notes_rewrite_cfg
modifier|*
name|c
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|from_obj
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|to_obj
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|finish_copy_notes_for_rewrite
parameter_list|(
name|struct
name|notes_rewrite_cfg
modifier|*
name|c
parameter_list|,
specifier|const
name|char
modifier|*
name|msg
parameter_list|)
function_decl|;
end_function_decl
begin_endif
endif|#
directive|endif
end_endif
end_unit
