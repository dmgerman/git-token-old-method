begin_unit
begin_include
include|#
directive|include
file|"wt-status.h"
end_include
begin_include
include|#
directive|include
file|"color.h"
end_include
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"object.h"
end_include
begin_include
include|#
directive|include
file|"dir.h"
end_include
begin_include
include|#
directive|include
file|"commit.h"
end_include
begin_include
include|#
directive|include
file|"diff.h"
end_include
begin_include
include|#
directive|include
file|"revision.h"
end_include
begin_include
include|#
directive|include
file|"diffcore.h"
end_include
begin_decl_stmt
DECL|variable|wt_status_use_color
name|int
name|wt_status_use_color
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|wt_status_colors
specifier|static
name|char
name|wt_status_colors
index|[]
index|[
name|COLOR_MAXLEN
index|]
init|=
block|{
literal|""
block|,
comment|/* WT_STATUS_HEADER: normal */
literal|"\033[32m"
block|,
comment|/* WT_STATUS_UPDATED: green */
literal|"\033[31m"
block|,
comment|/* WT_STATUS_CHANGED: red */
literal|"\033[31m"
block|,
comment|/* WT_STATUS_UNTRACKED: red */
block|}
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|parse_status_slot
specifier|static
name|int
name|parse_status_slot
parameter_list|(
specifier|const
name|char
modifier|*
name|var
parameter_list|,
name|int
name|offset
parameter_list|)
block|{
if|if
condition|(
operator|!
name|strcasecmp
argument_list|(
name|var
operator|+
name|offset
argument_list|,
literal|"header"
argument_list|)
condition|)
return|return
name|WT_STATUS_HEADER
return|;
if|if
condition|(
operator|!
name|strcasecmp
argument_list|(
name|var
operator|+
name|offset
argument_list|,
literal|"updated"
argument_list|)
condition|)
return|return
name|WT_STATUS_UPDATED
return|;
if|if
condition|(
operator|!
name|strcasecmp
argument_list|(
name|var
operator|+
name|offset
argument_list|,
literal|"changed"
argument_list|)
condition|)
return|return
name|WT_STATUS_CHANGED
return|;
if|if
condition|(
operator|!
name|strcasecmp
argument_list|(
name|var
operator|+
name|offset
argument_list|,
literal|"untracked"
argument_list|)
condition|)
return|return
name|WT_STATUS_UNTRACKED
return|;
name|die
argument_list|(
literal|"bad config variable '%s'"
argument_list|,
name|var
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|color
specifier|static
specifier|const
name|char
modifier|*
name|color
parameter_list|(
name|int
name|slot
parameter_list|)
block|{
return|return
name|wt_status_use_color
condition|?
name|wt_status_colors
index|[
name|slot
index|]
else|:
literal|""
return|;
block|}
end_function
begin_function
DECL|function|wt_status_prepare
name|void
name|wt_status_prepare
parameter_list|(
name|struct
name|wt_status
modifier|*
name|s
parameter_list|)
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|head
decl_stmt|;
name|s
operator|->
name|is_initial
operator|=
name|get_sha1
argument_list|(
literal|"HEAD"
argument_list|,
name|sha1
argument_list|)
condition|?
literal|1
else|:
literal|0
expr_stmt|;
name|head
operator|=
name|resolve_ref
argument_list|(
name|git_path
argument_list|(
literal|"HEAD"
argument_list|)
argument_list|,
name|sha1
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|s
operator|->
name|branch
operator|=
name|head
condition|?
name|strdup
argument_list|(
name|head
operator|+
name|strlen
argument_list|(
name|get_git_dir
argument_list|()
argument_list|)
operator|+
literal|1
argument_list|)
else|:
name|NULL
expr_stmt|;
name|s
operator|->
name|reference
operator|=
literal|"HEAD"
expr_stmt|;
name|s
operator|->
name|amend
operator|=
literal|0
expr_stmt|;
name|s
operator|->
name|verbose
operator|=
literal|0
expr_stmt|;
name|s
operator|->
name|commitable
operator|=
literal|0
expr_stmt|;
block|}
end_function
begin_decl_stmt
DECL|function|wt_status_print_header
specifier|static
name|void
name|wt_status_print_header
argument_list|(
specifier|const
name|char
operator|*
expr|main
argument_list|,
specifier|const
name|char
operator|*
name|sub
argument_list|)
block|{
specifier|const
name|char
modifier|*
name|c
init|=
name|color
argument_list|(
name|WT_STATUS_HEADER
argument_list|)
decl_stmt|;
name|color_printf_ln
argument_list|(
name|c
argument_list|,
literal|"# %s:"
argument_list|,
expr|main
argument_list|)
expr_stmt|;
name|color_printf_ln
argument_list|(
name|c
argument_list|,
literal|"#   (%s)"
argument_list|,
name|sub
argument_list|)
expr_stmt|;
name|color_printf_ln
argument_list|(
name|c
argument_list|,
literal|"#"
argument_list|)
expr_stmt|;
block|}
end_decl_stmt
begin_function
DECL|function|wt_status_print_trailer
specifier|static
name|void
name|wt_status_print_trailer
parameter_list|(
name|void
parameter_list|)
block|{
name|color_printf_ln
argument_list|(
name|color
argument_list|(
name|WT_STATUS_HEADER
argument_list|)
argument_list|,
literal|"#"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|wt_status_print_filepair
specifier|static
name|void
name|wt_status_print_filepair
parameter_list|(
name|int
name|t
parameter_list|,
name|struct
name|diff_filepair
modifier|*
name|p
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|c
init|=
name|color
argument_list|(
name|t
argument_list|)
decl_stmt|;
name|color_printf
argument_list|(
name|color
argument_list|(
name|WT_STATUS_HEADER
argument_list|)
argument_list|,
literal|"#\t"
argument_list|)
expr_stmt|;
switch|switch
condition|(
name|p
operator|->
name|status
condition|)
block|{
case|case
name|DIFF_STATUS_ADDED
case|:
name|color_printf
argument_list|(
name|c
argument_list|,
literal|"new file: %s"
argument_list|,
name|p
operator|->
name|one
operator|->
name|path
argument_list|)
expr_stmt|;
break|break;
case|case
name|DIFF_STATUS_COPIED
case|:
name|color_printf
argument_list|(
name|c
argument_list|,
literal|"copied: %s -> %s"
argument_list|,
name|p
operator|->
name|one
operator|->
name|path
argument_list|,
name|p
operator|->
name|two
operator|->
name|path
argument_list|)
expr_stmt|;
break|break;
case|case
name|DIFF_STATUS_DELETED
case|:
name|color_printf_ln
argument_list|(
name|c
argument_list|,
literal|"deleted: %s"
argument_list|,
name|p
operator|->
name|one
operator|->
name|path
argument_list|)
expr_stmt|;
break|break;
case|case
name|DIFF_STATUS_MODIFIED
case|:
name|color_printf
argument_list|(
name|c
argument_list|,
literal|"modified: %s"
argument_list|,
name|p
operator|->
name|one
operator|->
name|path
argument_list|)
expr_stmt|;
break|break;
case|case
name|DIFF_STATUS_RENAMED
case|:
name|color_printf
argument_list|(
name|c
argument_list|,
literal|"renamed: %s -> %s"
argument_list|,
name|p
operator|->
name|one
operator|->
name|path
argument_list|,
name|p
operator|->
name|two
operator|->
name|path
argument_list|)
expr_stmt|;
break|break;
case|case
name|DIFF_STATUS_TYPE_CHANGED
case|:
name|color_printf
argument_list|(
name|c
argument_list|,
literal|"typechange: %s"
argument_list|,
name|p
operator|->
name|one
operator|->
name|path
argument_list|)
expr_stmt|;
break|break;
case|case
name|DIFF_STATUS_UNKNOWN
case|:
name|color_printf
argument_list|(
name|c
argument_list|,
literal|"unknown: %s"
argument_list|,
name|p
operator|->
name|one
operator|->
name|path
argument_list|)
expr_stmt|;
break|break;
case|case
name|DIFF_STATUS_UNMERGED
case|:
name|color_printf
argument_list|(
name|c
argument_list|,
literal|"unmerged: %s"
argument_list|,
name|p
operator|->
name|one
operator|->
name|path
argument_list|)
expr_stmt|;
break|break;
default|default:
name|die
argument_list|(
literal|"bug: unhandled diff status %c"
argument_list|,
name|p
operator|->
name|status
argument_list|)
expr_stmt|;
block|}
name|printf
argument_list|(
literal|"\n"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|wt_status_print_updated_cb
specifier|static
name|void
name|wt_status_print_updated_cb
parameter_list|(
name|struct
name|diff_queue_struct
modifier|*
name|q
parameter_list|,
name|struct
name|diff_options
modifier|*
name|options
parameter_list|,
name|void
modifier|*
name|data
parameter_list|)
block|{
name|struct
name|wt_status
modifier|*
name|s
init|=
name|data
decl_stmt|;
name|int
name|shown_header
init|=
literal|0
decl_stmt|;
name|int
name|i
decl_stmt|;
if|if
condition|(
name|q
operator|->
name|nr
condition|)
block|{ 	}
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|q
operator|->
name|nr
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
name|q
operator|->
name|queue
index|[
name|i
index|]
operator|->
name|status
operator|==
literal|'U'
condition|)
continue|continue;
if|if
condition|(
operator|!
name|shown_header
condition|)
block|{
name|wt_status_print_header
argument_list|(
literal|"Updated but not checked in"
argument_list|,
literal|"will commit"
argument_list|)
expr_stmt|;
name|s
operator|->
name|commitable
operator|=
literal|1
expr_stmt|;
name|shown_header
operator|=
literal|1
expr_stmt|;
block|}
name|wt_status_print_filepair
argument_list|(
name|WT_STATUS_UPDATED
argument_list|,
name|q
operator|->
name|queue
index|[
name|i
index|]
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|shown_header
condition|)
name|wt_status_print_trailer
argument_list|()
expr_stmt|;
block|}
end_function
begin_function
DECL|function|wt_status_print_changed_cb
specifier|static
name|void
name|wt_status_print_changed_cb
parameter_list|(
name|struct
name|diff_queue_struct
modifier|*
name|q
parameter_list|,
name|struct
name|diff_options
modifier|*
name|options
parameter_list|,
name|void
modifier|*
name|data
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
if|if
condition|(
name|q
operator|->
name|nr
condition|)
name|wt_status_print_header
argument_list|(
literal|"Changed but not updated"
argument_list|,
literal|"use git-update-index to mark for commit"
argument_list|)
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|q
operator|->
name|nr
condition|;
name|i
operator|++
control|)
name|wt_status_print_filepair
argument_list|(
name|WT_STATUS_CHANGED
argument_list|,
name|q
operator|->
name|queue
index|[
name|i
index|]
argument_list|)
expr_stmt|;
if|if
condition|(
name|q
operator|->
name|nr
condition|)
name|wt_status_print_trailer
argument_list|()
expr_stmt|;
block|}
end_function
begin_function
DECL|function|wt_status_print_initial
name|void
name|wt_status_print_initial
parameter_list|(
name|struct
name|wt_status
modifier|*
name|s
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|read_cache
argument_list|()
expr_stmt|;
if|if
condition|(
name|active_nr
condition|)
block|{
name|s
operator|->
name|commitable
operator|=
literal|1
expr_stmt|;
name|wt_status_print_header
argument_list|(
literal|"Updated but not checked in"
argument_list|,
literal|"will commit"
argument_list|)
expr_stmt|;
block|}
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|active_nr
condition|;
name|i
operator|++
control|)
block|{
name|color_printf
argument_list|(
name|color
argument_list|(
name|WT_STATUS_HEADER
argument_list|)
argument_list|,
literal|"#\t"
argument_list|)
expr_stmt|;
name|color_printf_ln
argument_list|(
name|color
argument_list|(
name|WT_STATUS_UPDATED
argument_list|)
argument_list|,
literal|"new file: %s"
argument_list|,
name|active_cache
index|[
name|i
index|]
operator|->
name|name
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|active_nr
condition|)
name|wt_status_print_trailer
argument_list|()
expr_stmt|;
block|}
end_function
begin_function
DECL|function|wt_status_print_updated
specifier|static
name|void
name|wt_status_print_updated
parameter_list|(
name|struct
name|wt_status
modifier|*
name|s
parameter_list|)
block|{
name|struct
name|rev_info
name|rev
decl_stmt|;
specifier|const
name|char
modifier|*
name|argv
index|[]
init|=
block|{
name|NULL
block|,
name|NULL
block|,
name|NULL
block|}
decl_stmt|;
name|argv
index|[
literal|1
index|]
operator|=
name|s
operator|->
name|reference
expr_stmt|;
name|init_revisions
argument_list|(
operator|&
name|rev
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|setup_revisions
argument_list|(
literal|2
argument_list|,
name|argv
argument_list|,
operator|&
name|rev
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|rev
operator|.
name|diffopt
operator|.
name|output_format
operator||=
name|DIFF_FORMAT_CALLBACK
expr_stmt|;
name|rev
operator|.
name|diffopt
operator|.
name|format_callback
operator|=
name|wt_status_print_updated_cb
expr_stmt|;
name|rev
operator|.
name|diffopt
operator|.
name|format_callback_data
operator|=
name|s
expr_stmt|;
name|rev
operator|.
name|diffopt
operator|.
name|detect_rename
operator|=
literal|1
expr_stmt|;
name|run_diff_index
argument_list|(
operator|&
name|rev
argument_list|,
literal|1
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|wt_status_print_changed
specifier|static
name|void
name|wt_status_print_changed
parameter_list|(
name|struct
name|wt_status
modifier|*
name|s
parameter_list|)
block|{
name|struct
name|rev_info
name|rev
decl_stmt|;
specifier|const
name|char
modifier|*
name|argv
index|[]
init|=
block|{
name|NULL
block|,
name|NULL
block|}
decl_stmt|;
name|init_revisions
argument_list|(
operator|&
name|rev
argument_list|,
literal|""
argument_list|)
expr_stmt|;
name|setup_revisions
argument_list|(
literal|1
argument_list|,
name|argv
argument_list|,
operator|&
name|rev
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|rev
operator|.
name|diffopt
operator|.
name|output_format
operator||=
name|DIFF_FORMAT_CALLBACK
expr_stmt|;
name|rev
operator|.
name|diffopt
operator|.
name|format_callback
operator|=
name|wt_status_print_changed_cb
expr_stmt|;
name|rev
operator|.
name|diffopt
operator|.
name|format_callback_data
operator|=
name|s
expr_stmt|;
name|run_diff_files
argument_list|(
operator|&
name|rev
argument_list|,
literal|0
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|wt_status_print_untracked
specifier|static
name|void
name|wt_status_print_untracked
parameter_list|(
specifier|const
name|struct
name|wt_status
modifier|*
name|s
parameter_list|)
block|{
name|struct
name|dir_struct
name|dir
decl_stmt|;
specifier|const
name|char
modifier|*
name|x
decl_stmt|;
name|int
name|i
decl_stmt|;
name|int
name|shown_header
init|=
literal|0
decl_stmt|;
name|memset
argument_list|(
operator|&
name|dir
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|dir
argument_list|)
argument_list|)
expr_stmt|;
name|dir
operator|.
name|exclude_per_dir
operator|=
literal|".gitignore"
expr_stmt|;
name|x
operator|=
name|git_path
argument_list|(
literal|"info/exclude"
argument_list|)
expr_stmt|;
if|if
condition|(
name|file_exists
argument_list|(
name|x
argument_list|)
condition|)
name|add_excludes_from_file
argument_list|(
operator|&
name|dir
argument_list|,
name|x
argument_list|)
expr_stmt|;
name|read_directory
argument_list|(
operator|&
name|dir
argument_list|,
literal|"."
argument_list|,
literal|""
argument_list|,
literal|0
argument_list|)
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|dir
operator|.
name|nr
condition|;
name|i
operator|++
control|)
block|{
comment|/* check for matching entry, which is unmerged; lifted from 		 * builtin-ls-files:show_other_files */
name|struct
name|dir_entry
modifier|*
name|ent
init|=
name|dir
operator|.
name|entries
index|[
name|i
index|]
decl_stmt|;
name|int
name|pos
init|=
name|cache_name_pos
argument_list|(
name|ent
operator|->
name|name
argument_list|,
name|ent
operator|->
name|len
argument_list|)
decl_stmt|;
name|struct
name|cache_entry
modifier|*
name|ce
decl_stmt|;
if|if
condition|(
literal|0
operator|<=
name|pos
condition|)
name|die
argument_list|(
literal|"bug in wt_status_print_untracked"
argument_list|)
expr_stmt|;
name|pos
operator|=
operator|-
name|pos
operator|-
literal|1
expr_stmt|;
if|if
condition|(
name|pos
operator|<
name|active_nr
condition|)
block|{
name|ce
operator|=
name|active_cache
index|[
name|pos
index|]
expr_stmt|;
if|if
condition|(
name|ce_namelen
argument_list|(
name|ce
argument_list|)
operator|==
name|ent
operator|->
name|len
operator|&&
operator|!
name|memcmp
argument_list|(
name|ce
operator|->
name|name
argument_list|,
name|ent
operator|->
name|name
argument_list|,
name|ent
operator|->
name|len
argument_list|)
condition|)
continue|continue;
block|}
if|if
condition|(
operator|!
name|shown_header
condition|)
block|{
name|wt_status_print_header
argument_list|(
literal|"Untracked files"
argument_list|,
literal|"use \"git add\" to add to commit"
argument_list|)
expr_stmt|;
name|shown_header
operator|=
literal|1
expr_stmt|;
block|}
name|color_printf
argument_list|(
name|color
argument_list|(
name|WT_STATUS_HEADER
argument_list|)
argument_list|,
literal|"#\t"
argument_list|)
expr_stmt|;
name|color_printf_ln
argument_list|(
name|color
argument_list|(
name|WT_STATUS_UNTRACKED
argument_list|)
argument_list|,
literal|"%.*s"
argument_list|,
name|ent
operator|->
name|len
argument_list|,
name|ent
operator|->
name|name
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|wt_status_print_verbose
specifier|static
name|void
name|wt_status_print_verbose
parameter_list|(
name|struct
name|wt_status
modifier|*
name|s
parameter_list|)
block|{
name|struct
name|rev_info
name|rev
decl_stmt|;
specifier|const
name|char
modifier|*
name|argv
index|[]
init|=
block|{
name|NULL
block|,
name|NULL
block|,
name|NULL
block|}
decl_stmt|;
name|argv
index|[
literal|1
index|]
operator|=
name|s
operator|->
name|reference
expr_stmt|;
name|init_revisions
argument_list|(
operator|&
name|rev
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|setup_revisions
argument_list|(
literal|2
argument_list|,
name|argv
argument_list|,
operator|&
name|rev
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|rev
operator|.
name|diffopt
operator|.
name|output_format
operator||=
name|DIFF_FORMAT_PATCH
expr_stmt|;
name|rev
operator|.
name|diffopt
operator|.
name|detect_rename
operator|=
literal|1
expr_stmt|;
name|run_diff_index
argument_list|(
operator|&
name|rev
argument_list|,
literal|1
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|wt_status_print
name|void
name|wt_status_print
parameter_list|(
name|struct
name|wt_status
modifier|*
name|s
parameter_list|)
block|{
if|if
condition|(
name|s
operator|->
name|branch
operator|&&
name|strcmp
argument_list|(
name|s
operator|->
name|branch
argument_list|,
literal|"refs/heads/master"
argument_list|)
condition|)
name|color_printf_ln
argument_list|(
name|color
argument_list|(
name|WT_STATUS_HEADER
argument_list|)
argument_list|,
literal|"# On branch %s"
argument_list|,
name|s
operator|->
name|branch
argument_list|)
expr_stmt|;
if|if
condition|(
name|s
operator|->
name|is_initial
condition|)
block|{
name|color_printf_ln
argument_list|(
name|color
argument_list|(
name|WT_STATUS_HEADER
argument_list|)
argument_list|,
literal|"#"
argument_list|)
expr_stmt|;
name|color_printf_ln
argument_list|(
name|color
argument_list|(
name|WT_STATUS_HEADER
argument_list|)
argument_list|,
literal|"# Initial commit"
argument_list|)
expr_stmt|;
name|color_printf_ln
argument_list|(
name|color
argument_list|(
name|WT_STATUS_HEADER
argument_list|)
argument_list|,
literal|"#"
argument_list|)
expr_stmt|;
name|wt_status_print_initial
argument_list|(
name|s
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|wt_status_print_updated
argument_list|(
name|s
argument_list|)
expr_stmt|;
name|discard_cache
argument_list|()
expr_stmt|;
block|}
name|wt_status_print_changed
argument_list|(
name|s
argument_list|)
expr_stmt|;
name|wt_status_print_untracked
argument_list|(
name|s
argument_list|)
expr_stmt|;
if|if
condition|(
name|s
operator|->
name|verbose
operator|&&
operator|!
name|s
operator|->
name|is_initial
condition|)
name|wt_status_print_verbose
argument_list|(
name|s
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|s
operator|->
name|commitable
condition|)
name|printf
argument_list|(
literal|"%s\n"
argument_list|,
name|s
operator|->
name|amend
condition|?
literal|"# No changes"
else|:
literal|"nothing to commit"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|git_status_config
name|int
name|git_status_config
parameter_list|(
specifier|const
name|char
modifier|*
name|k
parameter_list|,
specifier|const
name|char
modifier|*
name|v
parameter_list|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|k
argument_list|,
literal|"status.color"
argument_list|)
condition|)
block|{
name|wt_status_use_color
operator|=
name|git_config_colorbool
argument_list|(
name|k
argument_list|,
name|v
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|k
argument_list|,
literal|"status.color."
argument_list|,
literal|13
argument_list|)
condition|)
block|{
name|int
name|slot
init|=
name|parse_status_slot
argument_list|(
name|k
argument_list|,
literal|13
argument_list|)
decl_stmt|;
name|color_parse
argument_list|(
name|v
argument_list|,
name|k
argument_list|,
name|wt_status_colors
index|[
name|slot
index|]
argument_list|)
expr_stmt|;
block|}
return|return
name|git_default_config
argument_list|(
name|k
argument_list|,
name|v
argument_list|)
return|;
block|}
end_function
end_unit
